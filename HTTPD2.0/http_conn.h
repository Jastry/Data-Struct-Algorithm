#ifndef __HTTPCONNECTION_H__
#define __HTTPCINNECTION_H__

#include <unistd.h>
#include <signal.h>
#include <sys.types.h>

#include <errno.h>
#include "locker.h"

class http_conn {
public:
    /* 文件名的最大长度 */
    static const int FILENAME_LEN = 200;

    /* 读缓冲区的大小 */
    static const int READ_BUFFER_SIZE = 2048;

    /* 写缓冲区的大小 */
    static const int WRITE_BUFFER_SIZE = 1024;

    /* HTTP 请求方法仅支持 GET */
    enum METHOD { GET = 0, POST, HEAD, PUT, DELETE,
                  TRACE, OPTIONS, CONNECT, PATCH};

    /* 解析客户请求时，主状态机所处状态 */
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0,
                       CHECK_STATE_HEADER,
                       CHECK_STATE_CONTENT };

    /* 服务器处理 HTTP 请求的可能结果 */
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST,
                     NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST,
                     INTERNAL_ERROR, CLOSED_CONNECTION
        
    };

    /* 行的读取状态 */
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN};

public:
    http_conn(){}
    ~http_conn(){}

public:
    /* 初始化新接受的连接 */
    void init( int sockfd, const sockaddr_in& addr );

    /* 关闭连接 */
    void close_conn( bool real_close = true );

    /* 处理客户请求 */
    void process( void );

    /* 非阻塞读操作 */
    bool read( void );

    /* 非阻塞写操作 */
    bool write( void );

private:
    /* 初始化连接 */
    void init( void );

    /* 解析 HTTP 请求 */
    HTTP_CODE process_read( void );

    /* 填充 HTTP 应答 */
    bool process_write( HTTP_CODE ret );

    /* 下面一组函数由 process_read 调用，用来分析 HTTP 请求 */
    HTTP_CODE parse_request_line( char * text );
    HTTP_CODE parse_headers( char * text );
    HTTP_CODE parse_content( char * text );
    HTTP_CODE do_request( void );
    char * get_line( void ) { return m_read_buf + m_start_line; }
    LINE_STATUS parse_line( void );

    /* 下面一组函数被 process_write 调用，用来填充HTTP应答 */
    void unmap();
    bool add_reponse( const char * format, ... );
    bool add_content( const char * content );
    bool add_headers( int content_length );
    bool add_linger( void );
    bool add_black_line( void );

public:

    /* 
     * 所有 socket 上的事件都被注册到一个 epoll 内核事件中，
     * 所以将 epoll 文件描述符设置为静态的
     */
    static int m_epollfd;

    /* 统计用户数量 */
    static int m_user_count;

private:
    /* 该 HTTP 连接的 socket 和对方的 socket 地址 */
    int m_sockfd;
    sockaddr_in m_address;

    /* 读缓冲区 */
    char m_read_buf[ READ_BUFFER_SIZE ];

    /* 标识读缓冲区已经读到客户数据的最后一个字节的下一个位置 */
    int m_read_idx;
       
    /* 当前正在分析的字符在读缓冲区的位置 */
    int m_checked_idx;
    
    /* 当前正在解析的行的起始位置 */
    int m_start_line;

    /* 写缓冲区 */
    char m_write_buf [ WRITE_BUFFER_SIZE ];

    /* 写缓冲区待发送的字节数 */
    int m_write_idx;


    /* 主状态机当前所处的状态 */
    CHECK_STAT m_check_state;

    /* 请求方方发 */
    METHOD m_method;


    /* 
     * 客户请求目标的完整路径，其内容等于 doc_root + m_url
     * doc_root 是网站根目录
     * m_url 为客户请求的文件名
     */
    char m_real_file[ FILENAME_LEN ];
    char * m_url;

    /* HTTP 版本号 我们进支持 HTTP/1.1 */
    char * m_version;

    /* 主机名 */
    char * m_host;

    /* HTTP 请求的消息体长度 */
    int m_content_length;

    /* HTTP 请求是否要求保持连接 */
    bool m_linger;

    /* 客户请求的目标文件被 mmap 到内存中的起始位置 */
    char * m_file_address;

    /* 目标文件的状态，通过它我们可以判断文件是否存在、是否为目录、是否可读、并获得文件大小等信息 */
    struct stat m_file_stat;

    /* 我们将采用 writev 来执行写操作，所以定义下面两个成员，其中 m_iv_count 表示被写内存块儿的数量 */
    struct iovec m_iv[2];
    int m_iv_count;
};

#endif
