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

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);

	if (-1 == bind(sock, (struct sockaddr*)& local, sizeof (local))){
		perror("bind");
		return 3;
	}

	char buf[1024];
	bzero(buf, sizeof (buf));

	for (;;){
		struct sockaddr_in peer;
		socklen_t len = sizeof (peer);

		ssize_t s = recvfrom(sock, buf, sizeof (buf) - 1, 0, (struct sockaddr*)& peer, &len);
		if (-1 == s){
			perror("recvfrom");
			return 4;
		}else {
			buf[s] = 0;
			printf("[%s : %d]# %s \n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port), buf);

			if (-1 == sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)& peer, sizeof (peer))){
				perror ("sendto");
				return 5;
			}
		}
	}
	close(sock);
	return 0;
}
