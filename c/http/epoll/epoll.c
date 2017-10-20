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


#define SIZE 64
#define BACKLOG 1024


const char* msg = "HTTP/1.0 200 OK\r\n\r\n<html><h1>hello epoll! </h1></html>\r\n";

static void usage(const char* proc){
	printf("usage \n\t %s [local_ip] [local_port]\n\n", proc);
}

static int startup(const char* ip, int port)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listen_sock){
		perror("socket");
		exit(2);
	}
	

#define REUSEADDR 1
#ifdef REUSEADDR
	printf("set SO_REUSEADDR\n");
	int on = 1;
	if (-1 == setsockopt(listen_sock, SOL_SOCKET,SO_REUSEADDR, &on, sizeof (on))){
		perror("setsockopt : reuse failed");
		exit(5);
	}
#endif //RUSEADDR

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

int main(int argc, char* argv[])
//int main()
{
	if (argc != 3){
		usage(argv[0]);
		return 1;
	}
	//char* argv[3];
	//argv[1] = "127.0.0.1";
	//argv[2] = "8080";

	int listen_sock = startup(argv[1], atoi(argv[2]));

	struct epoll_event ev, revs[256];
	int efds = epoll_create(SIZE);
	if (-1 == efds){
		perror("epoll_create");
		return 5;
	}
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;

	if (-1 == epoll_ctl(efds, EPOLL_CTL_ADD, listen_sock, &ev)){
		perror("epoll_ctl : listen_sock");
		return 6;
	}
	
	int nums = -1;
	int timeout = 1000;
	for (;;){
		switch((nums = epoll_wait(efds, revs, SIZE, -1/*timeout*/))){
			case 0:
				printf("timeout ...\n");
				break;
			case -1:
				perror("epoll_wait");
				break;
			default:
				{
					//at least one fd ready!
					int i = 0;
					for (; i < nums; ++i){
						int fd = revs[i].data.fd;
						/*the listen_sock is ready to read*/
						if (fd == listen_sock && (revs[i].events & EPOLLIN)){
							struct sockaddr_in client;
							socklen_t len = sizeof (client);

							int connec_fd = -1;
							if (-1 == (connec_fd = accept(listen_sock, (struct sockaddr*)& client, &len))){
								perror("accept");
								continue;
							}
							//get a new client
							printf("get a new client [ip : %s] [port : %d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
							/*insert client to rfds*/
							ev.events = EPOLLIN;
							ev.data.fd = connec_fd;
							if (-1 == (epoll_ctl(efds, EPOLL_CTL_ADD, connec_fd, &ev))){
								perror("epoll_ctl : connec_fd");
								close(connec_fd);
								continue;
							}
						}else if (fd != listen_sock){
							//normal fd read & write already!
							char buf[1024];
							bzero(buf, sizeof (buf));
							if (EPOLLIN & revs[i].events){
								//read ready!
								ssize_t s = read(fd, buf, sizeof (buf) - 1);
								if (s > 0){
									buf[s - 1] = 0;
									printf("client: %s\n", buf);
									//change revs[i].events to write
									ev.events = EPOLLOUT;
									ev.data.fd = fd;
									if (-1 == epoll_ctl(efds, EPOLL_CTL_MOD, fd, &ev)){
										perror("epoll_ctl");
									}
								}else if (0 == s){
									printf("client quit\n");
									if (-1 == epoll_ctl(efds, EPOLL_CTL_DEL, fd, NULL)){
										perror("epoll_ctl delete");
									}
									close(fd);
								}else {
									perror("read");
									if (-1 == epoll_ctl(efds, EPOLL_CTL_DEL, fd, NULL)){
										perror("epoll_ctl delete");
									}
									close(fd);
								}
							}else if (EPOLLOUT & revs[i].events){
								//write ready!
								if (-1 == write(fd, msg, strlen (msg))){
									perror("write");
									if (-1 == epoll_ctl(efds, EPOLL_CTL_DEL, fd, NULL)){
										perror("epoll_ctl delete");
									}
									close(fd);
								}
								if (-1 == epoll_ctl(efds, EPOLL_CTL_DEL, fd, NULL)){
									perror("epoll_ctl delete");
								}
								close(fd);
							}else {
								/* other option */
							}
						}else{
							/* impossible */
						}
					}
				}
				break;
		}
		
	}
	close(listen_sock);
	return 0;
}
