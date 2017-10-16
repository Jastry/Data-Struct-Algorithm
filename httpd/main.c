#include "httpd.h"

#if 1
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
		
		printf("get a client ip : %s, port is %d\n",inet_ntoa( client.sin_addr), ntohs(client.sin_port));

		pthread_t tid;
		pthread_create(&tid, NULL, header_request, (void*) connfd);
		pthread_detach(tid);
	}
	close(listenfd);
	return 0;
}

#endif

#if 0
int main(int argc, char** argv)
{
	if (argc <= 2){
		usage(argv[0]);
		return 1;
	}

	int listenfd = startup(argv[1], atoi(argv[2]));
	struct epoll_event revs[MAX_EVENT_NUMBER];
	int efds = epoll_create(6);
	if (-1 == efds){
		perror("epoll_create");
		return 6;
	}

	add_fd(efds, listenfd);
	int timeout = -1;
	for (;;){
		int ret = epoll_wait(efds, revs, MAX_EVENT_NUMBER, timeout);
		if (ret < 0){
			perror("epoll_wait");
			break;
		}
		et(revs, ret, efds, listenfd);
	}
	close(listenfd);
	return 0;
}
#endif
