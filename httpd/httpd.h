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


#define MAX_EVENT_NUMBER 1024
#define BACKLOG 10
#define BUFF_SIZE 1024


#endif //__HTTP_H__
