#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BACKLOG 10
#define SIZE sizeof(fd_set)*8

int fds[SIZE];

static void usage(const char* proc)
{
	printf("usage : %s [local_ip] [local_port]\n", proc);
}

static int startup(const char* ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock){
		perror("socket");
		exit(2);
	}

	struct sockaddr_in local_sock;
	local_sock.sin_family = AF_INET;
	local_sock.sin_port = htons(port);
	local_sock.sin_addr.s_addr = inet_addr(ip);

	if (-1 == bind(sock, (struct sockaddr*)&local_sock, sizeof (local_sock))){
		perror("bind");
		exit(3);
	}

	if (-1 == listen(sock, BACKLOG)){
		perror("listen");
		exit(4);
	}

	return sock;
}

int main(int argc, char* argv[])
{
	if (argc != 3){
		usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(argv[1], atoi(argv[2]));

	ssize_t nums = sizeof (fds) / sizeof (fds[0]);
	int i = 0;
	for (; i < nums; ++i){
		fds[i] = -1;
	}
	fds[0] = listen_sock;
	for (;;){
		int max_fd = -1;
		fd_set rfds;
		FD_ZERO(&rfds);
		for (i = 0; i < nums; ++i){
			if (fds[i] > 0){
				FD_SET(fds[i], &rfds);
				if (max_fd < fds[i]){
					max_fd = fds[i];
				}
			}
		}
		
		struct timeval timeout = {1,0};

		switch (select(max_fd + 1, &rfds, NULL, NULL, NULL/*&timeout*/)){
			case 0:
				printf("timeout ...\n");
				break;
			case -1:
				perror("select");
				break;
			default:
				{
					//at least one fd prapared!
					for (i = 0; i < nums; ++i){
						if (-1 == fds[i]){
							continue;
						}
						if (0 == i && FD_ISSET(listen_sock, &rfds)){
							//listen_sock
							struct sockaddr_in client;
							socklen_t len = sizeof (client);
							int new_sock = accept(listen_sock, (struct sockaddr*)& client, &len);
							if (-1 == new_sock){
								perror("accept");
								continue;
							}
							int j = 1;	/*listen_sock is 1*/
							for (; j < nums; ++j){	/*find new pos push new_sock*/
								if (-1 == fds[j])
									break;
							}
							if (j == nums){
								printf("client is full\n");
								close(new_sock);
								continue;
							}else {
								fds[j] = new_sock;
								printf("get a new client [%s : %d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
							}
							
						}else if (0 != i && FD_ISSET(fds[i], &rfds)){
							//normal fd ready!
							char buf[1024];
							ssize_t s = read(fds[i], buf, sizeof (buf) - 1);
							if (s > 0){
								buf[s - 1] = 0;
								printf("client say# %s\n", buf);
							}else if (0 == s){
								//client quit!
								printf("client quit\n");
								close(fds[i]);
								fds[i] = -1;
							}else{
								perror("read");
								close(fds[i]);
								fds[i] = -1;
							}
						}else {}
					}//for(nums)
					
				}//default
				break;
		}//switch
	}//for(;;)
	return 0;
}
