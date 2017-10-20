#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <pthread.h>

#define LISTEN_BACKLOG 50
void* request(void* arg)
{
	int client_sock = (int)arg;
	char buf[1024];
	bzero(buf, sizeof (buf));
	for (;;){
		ssize_t s = read(client_sock, buf, sizeof (buf));
		if (-1 == s){
			perror("read");
			close(client_sock);
			break;
			//return 6;
		}else if (0 == s){
			printf("client quit\n");
			break;
		}else{
			buf[s] = 0;
			printf("client say: %s\n", buf);
			ssize_t _s = write(client_sock, buf, strlen(buf));	
			if (-1 == _s){
				perror("write");
				break;
				//return 7;
			}
		}
	}
	close(client_sock);
	return 0;
}

void usage(const char* proc)
{
	printf("%s [local_ip] [port] \n", proc);
}

int main(int argc, char* argv[])
{
	if (argc != 3){
		usage(argv[0]);
		return 1;
	}

	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0){
		perror("socket");
		return 2;
	}

	struct sockaddr_in local;
	bzero(&local, sizeof (local));
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);	/*192.168.43.1*/

	if ((bind(listen_sock, (struct sockaddr*)&local, sizeof (local))) == -1){
		perror("bind");
		return 3;
	}

	if (listen(listen_sock, LISTEN_BACKLOG) == -1){
		perror("listen");
		return 4;
	}




	for (;;){
		struct sockaddr_in new_sock; 
		socklen_t len;

		int client_sock = accept(listen_sock, (struct sockaddr*)&new_sock, &len);
		if (client_sock == -1){
			perror("accept");
			//return 5;
			continue;
		}

		printf("get new client: [%s] [%d]\n", inet_ntoa(new_sock.sin_addr), ntohs(new_sock.sin_port));

		/*version 1.1*/
		pthread_t tid;
		pthread_create(&tid, NULL, request,(void*) client_sock);
		
		pthread_detach(tid);

		/*version 1.1*/
/*
		pid_t id = fork();
		if (-1 == id){
			perror("fork");
			close(client_sock);
			return 5;
		}else if (0 == id){// child 
			//read & write
			close(listen_sock);
			if (fork() > 0){
				exit(0);
			}
			char buf[1024];
			bzero(buf, sizeof (buf));
			for (;;){
				int s = read(client_sock, buf, sizeof (buf));
				if (-1 == s){
					perror("read");
					close(client_sock);
					return 6;
				}else if (0 == s){
					printf("client quit\n");
					break;
				}else{
					buf[s] = 0;
					printf("client say: %s\n", buf);
					ssize_t _s = write(client_sock, buf, strlen(buf));	
					if (-1 == _s){
						perror("write");
						return 7;
					}
				}
			}
			close(client_sock);
			exit(0);

		}else{// father
			close(client_sock);
			waitpid(id, NULL, 0);
		}
	
*/

/*	
 *	version 1.0
		char buf[1024];
		bzero(buf, sizeof (buf));
		int flag = 0;
		for (;;){
			ssize_t size = read(client_sock, buf, sizeof (buf) -1);
			if (size > 0){
				buf[size] = 0;
				printf("client# %s\n", buf);
				
				if (0 == strcmp(buf, "exit")){
					flag = 1;
					break;
				}

				int s = write(client_sock, buf, strlen(buf));
				if (s < 0){
					perror("write");
					return 5;
				}else {
					if (write(client_sock, buf, sizeof (buf)) < 0 ){
						perror("write");
						return 6;
					}
				}	
			
			}else if (0 == size){
				printf("client quit\n");
				break;
			}else {
				perror("read");
				return 5;
			}
		}
		close(client_sock);
		if (1 == flag)
			break;
*/
 	}		
	close(listen_sock);
	return 0;
}
