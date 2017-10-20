#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define BACKLOG 10

static void usage(const char* proc)
{
	printf("usage :%s [local_ip] [local_port]\n", proc);
}

static int startup(const char* ip, int port)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listen_sock){
		perror("socket");
		exit(2);
	}

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
{
	if (argc != 3){
		usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));

	int i = 0;
	struct pollfd fds[1024];
	for (; i < 1024; ++i){
		fds[i].fd = -1;
	}
	fds[0].fd = listen_sock;
	fds[0].events = POLLIN;
	int timeout = -1;
	for (;;){
		switch (poll(fds, 1024, timeout)){
			case 0:
				printf("timeout ...\n");
				break;
			case -1:
				perror("poll");
				break;
			default:
				{
					for (i = 0; i < 1024; ++i){
						if (-1 == fds[i].fd){
							continue;
						}
						if (0 == i && fds[i].revents & POLLIN){	/*listen sock read ready!*/
							struct sockaddr_in client;
							socklen_t len = sizeof (client);

							printf("get a new client [ip : %s] [port : %d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

							int new_sock = accept(fds[0].fd, (struct sockaddr*)& client, &len);
							if (-1 == new_sock){
								perror("accept");
								continue;
							}

							fds[new_sock].fd = new_sock;
							fds[new_sock].events = POLLIN;
							
						}else if (i != 0 && (fds[i].revents & POLLIN)){
							char buf[1024];
							ssize_t s = read(fds[i].fd, buf, sizeof (buf) - 1);
							if (s > 0){
								buf[s - 1] = 0;
								printf("sever echo# %s\n", buf);
							}else if (0 == s){
								//client quit
								printf("client quit\n");
								close(fds[i].fd);
								fds[i].fd = -1;
							}else{
								perror("read");
								close(fds[i].fd);
								fds[i].fd = -1;
							} 
						}else{}
					}
				}
				break;
		}
	}
	return 0;
}
