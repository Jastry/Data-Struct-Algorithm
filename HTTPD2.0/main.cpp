#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "locker.h"
#include "thread_pool.h"
#include "http_conn.h"

#define MAX_FD 65535
#define MAX_EVENT_NUMBER 10000

extern int addfd( int epollfd, int fd, bool one_shot );
extern int removefd( int epollfd, int fd );

void addsig( int sig, void(handler)(int), bool restart = true )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof(sa) );
    sa.sa_handler = handler;
    if ( restart ) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}

void show_error( int connfd, const char * info )
{
    printf( "%s", info );
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}

int main(int argc, char * argv[])
{
    if ( argc <= 2 ) {
        printf( "usage : \n\t %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }

    const char * ip = argv[1];
    int port = atoi( argv[2] );

    /* 忽略 SIGPIPE 信号 */
    addsig( SIGPIPE, SIG_IGN );

    /* 创建线程池 */
    threadpool< http_conn >* pool = NULL;
    try {
        pool = new threadpool< http_conn >;
    } catch( ... ) {
        printf(" in main catch\n ");
        return 1;
    }
    
    /* 预先为每个可能的客户连接分配一个 http_conn 对象 */
    http_conn *users = new http_conn[MAX_FD];
    assert( users );
    int user_count = 0;
    
    int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );
    struct linger tmp = {1, 0};
    setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof(address) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    ret = bind( listenfd, (struct sockaddr*)&address, sizeof(address) );
    assert( ret >= 0 );

    ret = listen( listenfd, 5 );
    assert( ret >= 0 );

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 92 );
    assert( epollfd != -1 );
    addfd( epollfd, listenfd, false );
    http_conn::m_epollfd = epollfd;

    while ( true ) {
        
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ( number < 0 ) && errno != EINTR ) {
            printf( "%s\n", "epoll failure" );
            break;
        }

        for ( int i = 0; i < number; ++i ) {
            int sockfd = events[ i ].data.fd;
            if ( listenfd == sockfd ) {
                /* 新用户连接 */
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( sockfd, (struct sockaddr*)&client_address, 
                                     &client_addrlength );

                if ( connfd < 0 ) {
                    printf( "errno is: %d\n", errno );
                    continue;
                }
                if ( http_conn::m_user_count >= MAX_FD ) {
                    show_error( connfd, "Internal server busy" );
                    continue;
                }
                
                printf("get a new client ip: %s, port :%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                /* 初始化客户连接 */
                users[ connfd ].init( connfd, client_address );
            } 

            else if ( events[ i ].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) ) {
                /* 如果有异常直接关闭客户端 */
                users[ sockfd ].close_conn();
            }

            else if ( events[ i ].events & EPOLLIN ) {
                /* 根据读取的结果决定是将任务加到线程池， 还是丢弃 */
                if ( users[ sockfd ].read() ) {
                    printf( "添加到任务池\n" );
                    pool->append( users + sockfd );
                } else {
                    printf( "关闭连接\n" );
                    users[ sockfd ].close_conn();
                }
            }

            else if ( events[ i ].events & EPOLLOUT ) {
#if 1
                /* 根据写的结束， 决定是否关闭连接 */
                if ( !users[ sockfd ].write() ) {
                    printf("关闭连接\n");
                    users[ sockfd ].close_conn();
                }
#endif
            } else {}
        }//for
    }//while

    close( epollfd );
    close( listenfd );
    delete [] users;
    delete pool;
    std::cout << "hello ckr\n";
    return 0;
}
