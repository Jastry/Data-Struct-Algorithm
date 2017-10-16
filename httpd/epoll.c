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

static void usage(const char* proc){
	printf("usage \n\t %s [local_ip] [local_port]\n\n", proc);
}

static int startup(const char* ip, int port)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(listen_sock);

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if (-1 == bind(listen_sock, (struct sockaddr*)& local, sizeof (local))){
		perror("bind");
		exit(3);
	}

	if (-1 == listen(listen_sock, BACKLOG)){
		perror("listen");
		exit(4);
	}
	return listen_sock;
}

static int set_noblocking(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	if (-1 == fl) {
		perror("fcntl : F_GETFL");
	}
	if (-1 == fcntl(fd, F_SETFL, fl | O_NONBLOCK)){
		perror("fcntl : F_SETFL");
	}

	return (fl | O_NONBLOCK);
}

static void add_fd(int efds, int fd, int enable_et)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (enable_et){
		ev.events |= EPOLLET;
	}
	if (-1 == epoll_ctl(efds, EPOLL_CTL_ADD, fd, &ev)){
		perror("epoll_ctl : add_fd");
	}
	set_noblocking(fd);
}

static void lt(struct epoll_event* revs, int num, int efds, int listenfd)
{
	const char* msg = "hello world";
	char buf[BUFF_SIZE];
	for (int i = 0; i < num; ++i){
		int sockfd = revs[i].data.fd;
		if (sockfd == listenfd){
			struct sockaddr_in client;
			socklen_t len = sizeof (client);

			int connfd = accept(listenfd, (struct sockaddr*)& client, &len);
			if (connfd == -1){
				perror("accept : lt");
				return;
			}
			add_fd(efds, connfd, 0);/*lt 模式*/
		}else if (revs[i].events & EPOLLIN){/*读就绪*/
			printf("event trigger once\n");
			bzero(buf, sizeof (buf));
			int ret = recv(sockfd, buf, sizeof (buf) - 1, 0);
			if (ret <= 0){
				printf("client quit\n");
				close(sockfd);
				continue;
			}
			printf("get %d bytes msg of connect say : %s\n",sockfd, buf);
 		}else if (revs[i].events & EPOLLOUT){
			write(sockfd, msg, strlen(msg));
		}else {
			printf("Something else happened\n");
		}
	}
}

static void et(struct epoll_event* revs, int num, int efds, int listenfd)
{
	char buf[BUFF_SIZE];
	for (int i = 0; i < num; ++i){
		int sockfd = revs[i].data.fd;
		if (sockfd == listenfd){
			struct sockaddr_in client;
			socklen_t len = sizeof (client);

			int connfd = accept(listenfd, (struct sockaddr*)& client, &len);
			if (-1 == connfd){
				perror("accept : et");
				return;
			}else {
				add_fd(efds, connfd, 1);
			}
		}else if (revs[i].events & EPOLLIN){
			printf("event tirgger once\n");
			for (;;){
				bzero(buf, sizeof (buf));
				int ret = recv(sockfd, buf, sizeof (buf), 0);
				if (ret < 0){
					/*对于非阻塞I/O如果下面条件成立，则表示数据已经全部读取完毕，此后epoll就能再次触发 EPOLLIN 事件，以驱动下一次读操作*/
					if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){
						printf("read later\n");
						break;
					}
					close(sockfd);
					break;
				}else if (ret == 0){
					printf("client quit\n");
					close(sockfd);
				}else{
					printf("connecfd is %d msg is %s\n", sockfd, buf);
				}
			}
		}else {
			printf("Something else happened\n");
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 2){
		usage(argv[0]);
		return 1;
	}
	
	int listenfd = startup(argv[1], atoi(argv[2]));

	struct epoll_event revs[MAX_EVENT_NUMBER];
	int efds = epoll_create(5);
	if (-1 == efds){
		perror("epoll_create");
		return 6;
	}
	add_fd(efds, listenfd, 1);

	int timeout = -1;
	for (;;){
		int ret = epoll_wait(efds, revs, MAX_EVENT_NUMBER, timeout);
		if (ret < 0){
			perror("epoll_wait");
			break;
		}

		//lt(revs, ret, efds, listenfd);
		et(revs, ret, efds, listenfd);
	
	}
	close(listenfd);
	return 0;
}

