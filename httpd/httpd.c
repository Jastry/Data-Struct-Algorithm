#include "httpd.h"

void printf_log(const char* msg, enum MSG agent)
{
	const char* const level[] = {
		"SUCCESS",
		"NOTICE",
		"WARNING",
		"ERROR",
		"FATAL"
	};
//#ifndef _STDOUT_
//#define _STDOUT_
#ifdef _STDOUT_
	printf("[%s] [%s]\n", msg, level[agent]);
	
#endif //_STDOUT
}


void usage(const char* proc)
{
	printf("usage : \n\t%s [local_ip] [local_port]\n\n", proc);
}

//ret > 1,line != '\0' 表示读的是数据;ret == 1 && line == '\n';表示空行;ret <= 0 && line == '\0'读完了
static int get_line(int sockfd, char* line, int size)
{
	/* '\r' -> '\n'; '\r\n' -> '\n' */
	/*assert(line);
	char ch = '\0';
	int len = 0;
	while (ch != '\n' && len < size -1){
		int r = recv(sockfd, &ch, 1, 0);
		if (r > 0) {
			if (ch == '\r'){
				recv(sockfd, &ch, 1, MSG_PEEK);
				if (ch == '\n'){
					recv(sockfd, &ch, 1, 0);
				} else {
					ch = '\n';
				}
			}
			line[len++] = ch;
		} else {
			ch = '\n';
		}
	}

	line[len] = '\0';
	return len;*/
	assert(line);
	read(sockfd, line, sizeof (line));
}

int startup(const char* ip, int port)
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd){
		perror("socket");
		exit(2);
	}
	
	int on = 1;
	if (-1 == setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on))){
		/* 如果服务器崩了，可以立即重启，立即退出TIME_WAIT状态 */
		perror("setsockopt");
		exit(3);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if (-1 == bind(listenfd, (struct sockaddr*)& local, sizeof (local))){
		perror("bind");
		exit(4);
	}

	if (-1 == listen(listenfd, BACKLOG)){
		perror("listen");
		exit(5);
	}
	return listenfd;
}

void* header_request(void* arg)
{
	int connfd = (int)arg;
	char line[1024];
	bzero(line, sizeof (line));
	get_line(connfd, line, sizeof (line));
	printf("%s",line);
	close(connfd);
}
