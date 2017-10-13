#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void usage(const char* proc)
{
	printf("%s [ip_addr] [port]\n",proc);
}

int main(int argc, char* argv[])
{
	if (argc != 3){
		usage(argv[0]);
		return 1;
	}

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sock){
		perror("socket");
		return 2;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);

	char buf[1024];
	bzero(buf, sizeof (buf));

	
	for (;;){
		
		printf("please enter# ");
		fflush(stdout);
		
		ssize_t s = read(0, buf, sizeof (buf));
		if (s > 0){
			buf[s - 1] = 0;
			if (-1 == sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)& server, sizeof (server))){
				perror ("sendto");
				return 5;
			}
		}

		s = recvfrom(sock, buf, sizeof (buf) - 1, 0, NULL, NULL);
		if (-1 == s){
			perror("recvfrom");
			return 4;
		}else {
			buf[s = 0];
			printf("server echo$ %s\n", buf);
		}
	}
	close(sock);
	return 0;
}
