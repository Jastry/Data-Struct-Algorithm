#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/stat.h>
#include "processpool.h"
#include <iostream>
#include <string>

enum Method_t {
    GET = 1,
    POST,
    OTHER
};
typedef enum Method_t Method;

class cgi_conn {
public:
    cgi_conn(){}
    ~cgi_conn(){}

    /* 初始化客户端连接，清空读缓冲区 */
    void init( int epollfd, int sockfd, const struct sockaddr_in& client )
    {
        m_epollfd = epollfd;
        m_sockfd = sockfd;
        m_address = client;
        memset(m_buf, '\0', sizeof(m_buf));
        m_read_idx = 0;
    }

    void process()
    {
        int idx = 0;
        int ret = -1;
        /* 循环读取和分析客户数据 */
        while (true) {
            idx = m_read_idx;
            ret = recv(m_sockfd, m_buf + idx, sizeof(m_buf), 0);
            if (ret < 0) {
                if (errno != EAGAIN) {
                    removefd(m_epollfd, m_sockfd);
                    printf("threr is 43, at test.cpp\n");
                }
                break;
            }
            /* 如果对方关闭连接，则服务器也关闭连接 */
            else if (ret == 0) {
               removefd( m_epollfd, m_sockfd );
               printf("threr is 49, at test.cpp\n");
               break;
            } else {
                m_read_idx += ret;
                printf("user content is %s\n", m_buf);
                /* 如果遇到字符 "\r\n",则开始处理客户请求 */
                for (; idx < m_read_idx; ++idx) {
                    if ( (idx >= 1) && (m_buf[idx - 1] == '\r') && (m_buf[idx] == '\n') ) {
                        break;
                    }
                }
                /* 防止发生粘包问题，如果没有读到 "\r\n" 表示需要读取更多的客户数据 */
                if ( idx == m_read_idx ) {
                    continue;
                }
                m_buf[idx - 1] = '\0';
                
                std::string file_name = m_buf;
                
                /* 通过第一行获取 method & 文件 */
                get_method(file_name);

                printf("file is %s\n", m_file.c_str());
                switch (m_method) {
                    case (POST):
                        std::cout << "post method" << std::endl;
                        break;
                    case (GET):
                        std::cout << "get method" << std::endl;
                        break;
                    case (OTHER):
                        std::cout << "other method" << std::endl;
                        break;
                }
                /* 判断客户要运行的程序是否存在 */
                if ( access( file_name.c_str(), F_OK ) == -1 ) {
                    removefd( m_epollfd, m_sockfd );
                    printf("threr is 71, at test.cpp\n");
                    break;
                }

                /* 创建子进程来执行 cgi 程序 */
                ret = fork();
                if (ret == -1) {
                    removefd( m_epollfd, m_sockfd );
                    printf("threr is 79, at test.cpp\n");
                    break;
                } else if (ret > 0) {
                    //father
                    removefd( m_epollfd, m_sockfd );
                    printf("threr is 84, at test.cpp\n");
                    break;
                } else {
                    //child 将标准输出重定向到 m_sockfd，并执行 cgi 服务
                    close(STDOUT_FILENO);
                    dup( m_sockfd );
                    execl( m_buf, m_buf, NULL );
                    exit(0);
                }
            }
        }
    }
private:
    void get_method(std::string& str)
    {
        if ((str.find( "GET" ) != std::string::npos) || (str.find( "get" ) != std::string::npos))
            m_method = GET;
        else if ((str.find( "POST" ) != std::string::npos) || (str.find("post") != std::string::npos))
            m_method = POST;
        else 
            m_method = OTHER;
        int file_pos = str.find("/");
        for (int i = file_pos+1; i < str.size() && (!isspace(str[i])); ++i) {
            m_file += "str[i]";
        }
    }
private:
    /* 读缓冲区大小 */
    Method m_method;
    std::string m_file;
    static const int BUFFER_SIZE = 1024;
    int m_epollfd;
    int m_sockfd;
    sockaddr_in m_address;
    char m_buf[BUFFER_SIZE];
    
    /* 标记读缓冲中已经读入客户数据的最后一个字节的下一个位置 */
    int m_read_idx;
};

void usage(const char * arg)
{
    printf("usage : \r\n%s [ip] [port]\n", arg);
}

int start_up(const char * ip, int port)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons( port );
    inet_pton( AF_INET, ip, &address.sin_addr );

    ret = bind( listenfd, (struct sockaddr*)&address, sizeof(address) );
    assert( ret != -1 );
    return listenfd;
}

int main( int argc, char * argv[] )
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    const char * ip = argv[1];
    int port = atoi( argv[2] );

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons( port );
    inet_pton( AF_INET, ip, &address.sin_addr );

    ret = bind( listenfd, (struct sockaddr*)&address, sizeof(address) );
    assert( ret != -1 );
    ret = listen( listenfd, 5 );
    assert( ret != -1 );

    processpool< cgi_conn >* pool = processpool< cgi_conn >::create( listenfd );
    if (pool) {
        pool->run();
        delete pool;
    }
    close( listenfd );
    return 0;
}

# if 0
locker lock;

/*
 * 子线程运行的函数，他首先获得互斥锁，然后暂停 5s，再释放该互斥锁 
 */
void *anther(void *arg) 
{
    printf("in child thread, lock the mutex\n");
    lock.lock();
    sleep(5);
    lock.unlock();
}

void prepare()
{
    lock.lock();
}

void parent()
{
    lock.unlock();
}

void child()
{
    lock.unlock();
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, anther, NULL);
   
    /*
     * 父进程中的主线成现暂停 1s 确保在执行 fork 之前， 
     * 子线程已经开始运行并且拿到了互斥锁 
     */

    sleep(1);
    pthread_atfork(prepare, parent, child);
    int pid = fork();
    if ( pid < 0 ) {
        pthread_join( pid, NULL );
        return 1;
    } else if ( 0 == pid ) {
        printf(" I'm child, want to get the lock\n");
        /*
         * 子进程从父进程中继承了互斥锁的状态，
         * 该互斥锁处于锁住状态，这是由父进程中的子线程引起的，
         * 因此子进程想要获取锁就会一直阻塞，尽管从逻辑上他不应该被阻塞
         */
        sleep(1);
        lock.lock();
        printf("I am child  get lock \n");
        sleep(2);
        lock.unlock();
        std::cout << "I'm child I relese the lock\n";
        exit(0);
    } else {
        std::cout << "I'm father I want get lock\n";
        lock.lock();
        std::cout << "I'm father I get lock\n";

        sleep(3);
        std::cout << "I'm father I relese the lock\n";
        lock.unlock();
        wait(NULL);
    }

    pthread_join(tid, NULL);
    return 0;
}
#endif
