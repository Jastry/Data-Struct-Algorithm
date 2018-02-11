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
#define SERVER_STRING "Server: httpd/0.1.0\r\n"
#define STDIN   0
#define STDOUT  1
#define STDERR  

void usage(const char* proc);
static int get_line(int sockfd, char* line, int size);
int startup(const char* ip, int port);
static void drop_header(int sockfd);
void not_found(int client);
void headers(int client);
void bad_request(int client);
void cannot_execute(int client);
void error_die(const char *sc);
void* header_request(void* arg);
static void echo_www(int fd, const char* path, ssize_t size);
static void  exec_cgi(int sockfd, const char* method, const char* query_string, const char* path);



#endif //__HTTP_H__
