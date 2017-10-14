#include "httpd.h"


int main(int argc, char** argv)
{
	if (argc <= 2){
		usage(argv[0]);
		return 1;
	}
	int listenfd = startup(argv[1], atoi(argv[2]));

	for (;;){
		struct sockaddr_in client;
		int len = sizeof (client);
		int connfd = accept(listenfd, (struct sockaddr*)& client, &len);
		if (-1 == connfd){
			perror("accept");
			close(connfd);
		}

		pthread_t tid;
		pthread_create(&tid, NULL, header_request, (void*) connfd);
		pthread_detach(tid);
	}
	close(listenfd);
	return 0;
}
