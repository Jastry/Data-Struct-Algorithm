#include <iostream>
#include "http_conn.h"

/* 定义 HTTP 响应的一些状态信息 */
const char * ok_200_title = "OK";
const char * error_400_title = "Bad Request";
const char * error_400_form = "Your request has bad syntax or is interently impossible to satisfy.\n";
const char * error_403_title = "Forbidden";
const char * error_403_form = "You do not have permission to get file from this server.\n";
const char * error_404_title = "Not Found";
const char * error_404_form = "The requested file was not found on this server.\n";
const char * error_500_title = "Internal Error";
const char * error_500_form = "There was an unusual problem serving the requested file.\n";

/* 网站根目录 */
const char * doc_root = ".";

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

void addfd( int epollfd, int fd, bool one_shot )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if ( one_shot ) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

void removefd( int epollfd, int fd )
{
    epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, NULL );
    close( fd );
}

void modfd( int epollfd, int fd, int ev )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

void http_conn::close_conn( bool real_close )
{
    if ( real_close && (m_sockfd != -1) ) {
        removefd( m_epollfd, m_sockfd );
        m_sockfd = -1;
        
        --m_user_count; //关闭一个连接时，将客户总量减一
    }
}

void http_conn::init( int sockfd, const sockaddr_in& addr )
{
    m_sockfd = sockfd;
    m_address = addr;

    /* 如下两行是为了避免 TIME_WAIT 状态，仅用于调试，实际使用是应该去掉 */
    int reuse = 1;
    setsockopt( m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse) );
    addfd( m_epollfd, m_sockfd, true );
    ++m_user_count;

    init();
}

void http_conn::init( void )
{
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_linger = false;

    m_method = GET;
    m_url = NULL;
    m_version = NULL;
    m_content_length = 0;
    m_host = NULL;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    memset( m_read_buf, '\0', sizeof(m_read_buf) );
    memset( m_write_buf, '\0', sizeof(m_write_buf) );
    memset( m_real_file, '\0', sizeof(m_real_file) );
}

http_conn::LINE_STATUS http_conn::parse_line( void )
{
    char temp;
    for (; m_checked_idx < m_read_idx; ++m_checked_idx) {
        
        temp = m_read_buf[ m_checked_idx ];
        
        if ( temp == '\r' ) {
            
            if ( ( m_checked_idx + 1 ) == m_read_idx )
                return LINE_OPEN;

            else if ( m_read_buf[ m_checked_idx + 1 ] == '\n' ) {
                
                /*____________________________________*/
                m_read_buf[ m_checked_idx++ ] = '\0';
                m_read_buf[ m_checked_idx++ ] = '\0';
                /*____________________________________*/
                return LINE_OK;
            }
            return LINE_BAD;
        } else if ( temp == '\n' ) {

            if ( ( m_checked_idx > 1 ) && ( m_read_buf[ m_checked_idx - 1 ] == '\r' ) ) {
                m_read_buf[ m_checked_idx-1 ] = '\0';
                m_read_buf[ m_checked_idx++ ] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }//for
    return LINE_OPEN;
}

/* 循环读取客户数据，直到无数据可读或对方关闭连接 */
bool http_conn::read( void )
{
    if ( m_read_idx >= READ_BUFFER_SIZE ) {
        return false;
    }
    
    int bytes_read = 0;
    while ( true ) {
        bytes_read = recv( m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0 );
        if ( bytes_read == -1 ) {
            if ( errno == EAGAIN || errno == EWOULDBLOCK )
                break;
            return false;
        } else if ( bytes_read == 0 ) {
            /* 客户端关闭连接 */
            return false;
        }
        m_read_idx += bytes_read;
    } //while
    return true;
}

/* 解析 HTTP 请求行,获得请求方法，目标 url， 以及 HTTP 版本号 */
http_conn::HTTP_CODE http_conn::parse_request_line( char * text )
{
    /* The strpbrk() function locates the first occurrence in the string s of any of the bytes in the string accept.*/
    m_url = strpbrk( text, " \t" );
    if ( m_url == NULL) {
        return BAD_REQUEST;
    }
    *m_url++ = '\0';
    
    char * method = text;
    if ( strcasecmp( method, "GET" ) == 0 ) {
        m_method = GET;
    } else if ( strcasecmp( method, "POST" ) == 0 ) {
        m_method = POST;
        m_check_state = CHECK_STATE_CONTENT;
    } else {
        return BAD_REQUEST;
    }

    m_url += strspn( m_url, " \t" );
    m_version = strpbrk( m_url, " \t" );
    if ( !m_version ) {
        return BAD_REQUEST;
    }
    *m_version++ = '\0'; 
    m_version += strspn( m_version, " \t" );

    if ( strcasecmp( m_version, "HTTP/1.1" ) != 0 ) {
        return BAD_REQUEST;
    }

    if ( strncasecmp( m_url, "http://", 7 ) == 0 ) {
        m_url += 7;
    
        /* The strchr() function returns a pointer to the first occurrence of the character c in the string s. */
        m_url = strchr( m_url, '/');
        printf("m_url is %s\n", m_url);
    }

    if ( !m_url || m_url[ 0 ] != '/') {
        return BAD_REQUEST;
    }

    m_check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

/* 解析 HTTP 请求的一个头部信息 */
http_conn::HTTP_CODE http_conn::parse_headers( char * text )
{
    /* 遇到空行表示头部字段解析完毕 */
    if ( text[0] == '\0' ) {
        /* 
         * 如果 HTTP 请求有消息体，则还需要读取 m_content_length 字节的消息体，
         * 状态机转移到 CHECK_STATE_CONTENT 状态 
         */
        if ( m_content_length != 0) {
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }

        /* 否则说明我们已经得到了一个完整的 HTTP 请求 */
        return GET_REQUEST;
    }

    /* 处理 Connection 头部字段 */
    else if ( strncasecmp( text, "Connection", 11 ) == 0 ) {
        text += 11;
        /************************
         *                      *
         *   注意是"空格 /t"    *
         *                      *
         ************************/
        text += strspn( text, " \t" );
        if ( strcasecmp( text, "keep-alive" ) == 0 ) {
            m_linger = true;
        }
    }

    /* 处理 Content-Length 头部字段 */
    else if ( strncasecmp( text, "Content-Length:", 15 ) == 0 ) {
        text += 15;
        text += strspn( text," \t" );
        m_content_length = atol( text );
        m_check_state = CHECK_STATE_CONTENT;
    }

    /* 处理 host 头部消息 */
    else if ( strncasecmp( text, "Host:", 5 ) == 0 ) {
        text += 5;
        text += strspn( text, " \t" );
        m_host = text;
    }

    /* 其他方法打印出来*/
    else {
        //printf( "oop! Unknow header %s\n", text );
    }
    
    return NO_REQUEST;
}

/* 我们没有真正的解析 HTTP 请求的消息体，只是判断他是否被完整的读入了 */
http_conn::HTTP_CODE http_conn::parse_content( char * text )
{
    if ( m_read_idx >= ( m_content_length + m_checked_idx ) ) {
        text[ m_content_length ] = '\0';
        return GET_REQUEST;
    }
}


/* 分析主状态机 */
http_conn::HTTP_CODE http_conn::process_read( void )
{
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char * text = 0;

    while ( ( ( m_check_state == CHECK_STATE_CONTENT ) && (line_status == LINE_OK) ) || ( ( line_status = parse_line() ) == LINE_OK ) ) {
        text = get_line();
        m_start_line = m_checked_idx;
        printf(" got 1 http line: %s\n", text);

        switch ( m_check_state ) {

            /* 解析请求行，获取方法、url 和 HTTP 版本 */
            case ( CHECK_STATE_REQUESTLINE ):
            {
                ret = parse_request_line( text );
                if ( ret == BAD_REQUEST ) {
                    return BAD_REQUEST;
                }
                break;
            }
            
            /* 解析头部，获取 Conntion 和 Content-Length */
            case ( CHECK_STATE_HEADER ):
            {
                ret = parse_headers( text );
                if ( ret == BAD_REQUEST ) {
                    return BAD_REQUEST;
                } else if ( ret == GET_REQUEST ) {
                    
                    /* 处理请求 */
                    return do_request();
                } //else if ( ret == POST_REQUEST )
                line_status = LINE_OPEN;
                break;
            }
            
            /* 解析文本 */
            case ( CHECK_STATE_CONTENT ):
            {
                ret = parse_content( text );
                if ( ret == GET_REQUEST ) {
                    return do_request();
                }
                line_status = LINE_OPEN;
                break;
            }

            default:
            {
                printf(" case m_check_state default\n");
                return INTERNAL_ERROR;
            }
        }//switch

    }//while
    return NO_REQUEST;
}

/* 
 * 当得到一个完整的、正确的 http 请求时，我们就分析目标文件的属性。
 * 如果目标文件存在、对用户可读、且不是目录，则使用 mmap 将文件
 * 映射到内存地址 m_file_address 处，并告诉调用者文件获取成功 
 */
http_conn::HTTP_CODE http_conn::do_request( void )
{
    strcpy( m_real_file, doc_root );
    int len = strlen( doc_root );
    strncpy( m_real_file + len, m_url, FILENAME_LEN-len-1 );
    printf(" m_real_file is: %s\n", m_real_file);

    if ( stat(m_real_file, &m_file_stat) < 0 ) {
        printf("没有此文件\n");
        return NO_REQUEST;
    }

    if ( ! ( m_file_stat.st_mode & S_IROTH ) ) {
        return FORBIDDEN_REQUEST;
    }

    if ( S_ISDIR( m_file_stat.st_mode ) ) {

        printf(" 请求的是网页目录: %s\n", m_real_file);
        return BAD_REQUEST;
    }

    int fd = open( m_real_file, O_RDONLY );

    /* 将文件内容映射到地址 m_file_address 位置处 */
    m_file_address = ( char* )mmap( 0, m_file_stat.st_size, PROT_READ, 
                                    MAP_PRIVATE, fd, 0);

    close( fd );
    return FILE_REQUEST;
}

/* 对映射区执行 munmap 操作 */
/*
       mmap()  creates  a  new  mapping in the virtual address space of the calling process.  The starting address for the new mapping is
       specified in addr.  The length argument specifies the length of the mapping.

       If addr is NULL, then the kernel chooses the address at which to create the mapping; this is the most portable method of  creating
       a  new  mapping.   If addr is not NULL, then the kernel takes it as a hint about where to place the mapping; on Linux, the mapping
       will be created at a nearby page boundary.  The address of the new mapping is returned as the result of the call.

       The contents of a file mapping (as opposed to an anonymous mapping; see MAP_ANONYMOUS below), are initialized using  length  bytes
       starting  at  offset offset in the file (or other object) referred to by the file descriptor fd.  offset must be a multiple of the
       page size as returned by sysconf(_SC_PAGE_SIZE).
 */

void http_conn::unmap( void )
{
    if ( m_file_address ) 
    {
        munmap( m_file_address, m_file_stat.st_size );
        m_file_address = NULL;
    }
}

/* 写 HTTP 响应 */
bool http_conn::write( void )
{
    int temp = 0;
    int bytes_have_send = 0;
    
    /* m_write_idx 写缓冲区待发送的字节数 */
    int bytes_to_send = m_write_idx;
    if ( bytes_to_send == 0 ) {
        modfd( m_epollfd, m_sockfd, EPOLLIN );
        init();
        return true;
    }

    while (1) {
        printf("%s\n", m_write_buf);
        temp = writev( m_sockfd, m_iv, m_iv_count );
        printf("temp is %d\n", temp);
        if (temp <= -1) {
            /*
             * 如果 TCP 写缓冲区没有空间，则等待下一轮 EPOLLOUT 事件，
             * 虽然在此期间服务器无法立即接收到同一客户的下一个请求
             * 但这样做可以保证连接的完整性
             */
            if ( errno == EAGAIN ) {
                modfd( m_epollfd, m_sockfd, EPOLLOUT );
                return true;
            }

            unmap();
            return false;
        }

        bytes_to_send -= temp;
        bytes_have_send += temp;
        if ( bytes_to_send <= bytes_have_send ) {
            /* 
             * 发送 HTTP 响应成功，
             * 根据   HTTP 请求中的 Connection 字段决定是否立即关闭连接 
             */
            unmap();
            if ( m_linger ) {
                init();
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return true;
            } else {
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return false;
            }
        }
    }//while 
}

/* 往读缓冲区写入待发送的数据 */
bool http_conn::add_response( const char * format, ... )
{
    if ( m_write_idx >= WRITE_BUFFER_SIZE) {
        return false;
    }

    va_list arg_list;
    va_start( arg_list, format );
    int len = vsnprintf( m_write_buf + m_write_idx, WRITE_BUFFER_SIZE - 1 - m_write_idx, format, arg_list );
    if ( len >= ( WRITE_BUFFER_SIZE - 1 - m_write_idx ) ) {
        return false;
    }
    m_write_idx += len;
    va_end( arg_list );
    return true;
}

bool http_conn::add_status_line( int status, const char * title ) 
{
    return add_response( "%s %d %s\r\n", "HTTP/1.1", status, title );
}

bool http_conn::add_headers( int content_len )
{
    add_content_length( content_len );
    add_linger();
    add_blank_line();

}

bool http_conn::add_content_length( int content_len )
{
    return add_response( "Content-Length: %d\r\n", content_len );
}

bool http_conn::add_linger( void )
{
    return add_response( "Connection: %s\r\n", (m_linger == true ) ? "keep_alive" : "close" );
}

bool http_conn::add_blank_line( void )
{
    return add_response("%s", "\r\n");
}

    
bool http_conn::add_content( const char * content )
{
    return add_response("%s", content);
}

/* 根据服务器处理 HTTP 请求结果，决定返回给客户端的内容 */
bool http_conn::process_write( HTTP_CODE ret )
{
    switch (ret) {

        /* 内置错误 */
        case (INTERNAL_ERROR):
        {
            add_status_line( 500, error_500_title );
            add_headers( strlen(error_500_form) );
            if ( !add_content( error_500_form ) ) {
                return false;
            }
            break;
        }
        
        /* 错误请求 */
        case (BAD_REQUEST):
        {
            add_status_line( 400, error_400_title );
            add_headers( strlen( error_400_form ) );
            if ( !add_content( error_400_form ) ) {
                return false;
            }
            break;
        }

        /* 没有客户请求的资源 */
        case (NO_RESOURCE):
        {
            add_status_line( 404, error_404_title );
            add_headers( strlen( error_404_form ) );
            if ( !add_content( error_404_form ) ) {
                return false;
            }
            break;
        }
        
        /* 服务器拒绝客户访问 */
        case ( FORBIDDEN_REQUEST ):
        {
            add_status_line( 403, error_403_title );
            add_headers( strlen( error_403_form ) );
            if ( !add_content( error_403_form ) ) {
                return false;
            }
            break;
        }

        /**/
        case ( FILE_REQUEST ):
        {
            add_status_line( 200, ok_200_title );

            if ( m_file_stat.st_size != 0 ) {
                add_headers( m_file_stat.st_size );
                m_iv[ 0 ].iov_base = m_write_buf;
                m_iv[ 0 ].iov_len = m_write_idx;
                m_iv[ 1 ].iov_base = m_file_address;
                m_iv[ 1 ].iov_len = m_file_stat.st_size;
                m_iv_count = 2;
                return true;
            } else {
                const char * ok_string = "<html><body> hello ckr </body></html>";
                add_headers( strlen( ok_string ) );
                if ( !add_content( ok_string ) ) {
                    return false;
                }
            }
        }

        default:
            return false;
    }//switch
    m_iv[ 0 ].iov_base = m_write_buf;
    m_iv[ 0 ].iov_len = m_write_idx;
    m_iv_count = 1;
    return true;
}

/* 由线程池中的工作线程调用，这时处理 HTTP请求的入口函数 */
void http_conn::process( void )
{
    HTTP_CODE read_ret = process_read();
    if ( read_ret == NO_REQUEST ) {
        printf("NO_REQUEST\n");
        modfd( m_epollfd, m_sockfd, EPOLLIN );
        return;
    }
    bool write_ret = process_write( read_ret );
    if ( !write_ret ) {
        printf("没有将事件更改为读关心\n");
        close_conn();
    }
    //printf("将事件设置成 EPOLLOUT\n");
    modfd( m_epollfd, m_sockfd, EPOLLOUT );
}
