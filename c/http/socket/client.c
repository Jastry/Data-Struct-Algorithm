#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

void usage(const char* proc)
{
	printf("%s [local_ip] [port]\n", proc);
}

int main(int argc, char* argv[])
{
	if (3 != argc){
		usage(argv[0]);
		return 1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		perror("socket");
		return 2;
	}

	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(atoi(argv[2]));
	peer.sin_addr.s_addr = inet_addr(argv[1]);

	if (- 1 == connect(sock, (struct sockaddr*)& peer, sizeof (peer))){
		perror("connect");
		return 3;
	}

	char buf[1024];
	bzero(buf, sizeof (buf));
	
	for (;;){
		printf("please enter: ");
		fflush(stdout);
		ssize_t s = read(0, buf, sizeof (buf));
		
		if (s < 0){
			perror("read");
			return 4;
		}else{
			buf[s - 1] = 0;
			if (0 == strcmp(buf, "exit")){	break;	}
			ssize_t _s = write(sock, buf, strlen(buf));
			
			if (s < 0){	perror("write");	return 5;	}
			buf[_s] = 0;
			int ss = read(sock, buf, sizeof (buf));
			if (-1 == ss){	perror("read");	return 4;	}
			printf("server echo$ : %s\n", buf);
		
		}
	}
	close(sock);
	return 0;
}
