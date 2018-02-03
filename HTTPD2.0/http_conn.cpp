#include <iostream>
#include "http_conn.h"

/* 定义 HTTP 响应的一些状态信息 */
const char * 0k_200_title = "OK";
const char * error_400_title = "Bad Request";
const char * error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char * error_403_title = "Forbidden";
const char * error_403_form = "You do not have permission to get file from this server.\n";
const char * error_404_title = "Not Found";
const char * error_404_form = "The requested file was not found on this server.\n";
const char * error_504_title = "Internal Error";
const char * error_504_form = "There was an unusual problem serving the requested file.\n";

/* 网站根目录 */
const char * doc_root = "./wwwroot/index.html";

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLCOK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

void addfd( int empollfd, int fd, bool one_shot )
{
    epoll_event event;
    envnt.data.fd = fd;
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
    event.events = ev | EPOLLET | EPOLLSHOT | EPOLLRDHUPa;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

void http_conn::close_conn( bool real_close )
{
    if ( real_close && (m_sockfd != -1) ) {
        removefd( epollfd, m_sockfd );
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

http_conn::LINE_STATUS http http_conn::parse_line( void )
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
bool http_conn::read( viod )
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
    m_url = strpbrk( text, '\r' );
    if ( m_url == NULL) {
        return BAD_REQUEST;
    }
    *m_url++ = '\0';
    
    char * method = text;
    if ( strncasecmp( method, "GET" ) == 0 ) {
        m_method = GET;
    } else if ( strncasecmp( method, "POST" ) == 0 ) {
        m_method = POST;
    } else {
        m_method = BAD_REQUEST;
    }

    m_url += strspn( m_url, '\t' );
}

