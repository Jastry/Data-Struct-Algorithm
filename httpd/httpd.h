#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

#define MAX_EVENT_NUMBER 1024
#define BACKLOG 2 
#define BUFF_SIZE 1024
enum MSG{
	SUCCESS,
	NOTICE,
	WARNING,
	ERROR,
	FATAL
};


static void  exec_cgi(int sockfd, const char* method, const char* query_string, const char* path);
void add_fd(int efds, int sockfd);
void et( struct epoll_event* revs, int nums, int efds, int listenfd );
int setnoblocking(int fd);
void del_fd( int efds, int sockfd );
static void ctl_fd(int efds, int sockfd);
static void echo_www(int fd, const char* path, ssize_t size);
void usage(const char* proc);
void* header_request(void* arg);
int startup(const char* ip, int port);
void printf_log(int sock, const char* msg);
void* request(void* arg);
#endif //__HTTP_H__
