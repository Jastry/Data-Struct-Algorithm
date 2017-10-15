#include "httpd.h"

const char* msg = "HTTP/1.0 200 OK\r\n\r\n\r\n";
const char* path = "wwwroot/index.html";
void add_fd(int efds, int sockfd)
{
	struct epoll_event ev;
	ev.data.fd = sockfd;
	ev.events = EPOLLOUT | EPOLLIN | EPOLLET;

	if ( -1 == epoll_ctl(efds, EPOLL_CTL_ADD, sockfd, &ev) ){
		perror("epoll_ctl : add");
		close(sockfd);
	}
	setnoblocking(sockfd);
}

int setnoblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void del_fd( int efds, int sockfd )
{
	if ( -1 == epoll_ctl( efds, EPOLL_CTL_DEL, sockfd, NULL) ){
		perror( "epoll_ctl" );
	}
	close(sockfd);
}
void et( struct epoll_event* revs, int nums, int efds, int listenfd ) 
{
	char buff[BUFF_SIZE];
	bzero(buff, sizeof (buff));
	for ( int i = 0; i < nums; ++i ) {
		int sockfd = revs[i].data.fd;
		if ( sockfd == listenfd ) {
			struct sockaddr_in client;
			socklen_t len = sizeof (client);
			int connfd = accept( sockfd, (struct sockaddr*)& client, &len );
			if ( connfd < 0 ){
				perror( "accept" );
			}
			add_fd( efds, connfd );
			printf( "get a new client, client ip : %s, client port : %d\n",\
				inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		} else if ( revs[i].events & EPOLLIN ) {
			for (;;){
				int ret = recv(sockfd, buff, sizeof (buff - 1), 0);
				if (ret < 0){
					if (errno == EAGAIN || errno == EWOULDBLOCK){
						printf("read later");
						break;
					}
					close(sockfd);
					break;
				} else if (ret == 0){
					printf("client quit\n");
					close(sockfd);
				} else {
					char line[BUFF_SIZE];
					/* HEAD REQUEST */
					printf("%s", buff);
					ctl_fd(efds, sockfd);
				}
			}
		} else if ( revs[i].events & EPOLLOUT) {	
			char status[BUFF_SIZE];
			send(sockfd, msg, strlen(msg), 0);
			
			int fd = open(path, O_RDONLY);
			if (fd < 0){ 
				perror("open");
			}
			struct stat stat_buf;
			fstat(fd, &stat_buf);
			
			sendfile(sockfd, fd, NULL, stat_buf.st_size);

			close(fd);
			del_fd(efds, sockfd);
			//get_line()
		}
	}
}

static void ctl_fd(int efds, int sockfd)
{
	struct epoll_event ev;
	ev.data.fd = sockfd;
	ev.events = EPOLLOUT | EPOLLET;

	if ( -1 == epoll_ctl(efds, EPOLL_CTL_MOD, sockfd, &ev)){
		perror("epoll_ctl : ctl_fd");
	}
}
void printf_log(const char* msg, enum MSG agent)
{
	const char* const level[] = {
		"SUCCESS",
		"NOTICE",
		"WARNING",
		"ERROR",
		"FATAL"
	};
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
	assert(line);
	int  len = 0;
	int  r = 0;
	char ch = '\0';

	while (ch != '\n' && len < size -1){
		r = recv(sockfd, &ch, 1, 0);
		if (r > 0) {
			if (ch == '\r'){
				recv(sockfd, &ch, 1, MSG_PEEK);
				if (ch == '\n')
					recv(sockfd, &ch, 1, 0);
				ch = '\n';
			}
			line[len++] = ch;
		} else {
				ch = '\n';
		}
	}
	line[len] = '\0';
	return len;
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

static void drop_header(int sockfd)
{
	int ret = -1;
	char buf[BUFF_SIZE];
	do{
		ret = get_line(sockfd, buf, sizeof (buf));
	} while (ret > 0 && strcmp(buf, "\n") != 0);
}

void* header_request(void* arg)
{
	int  connfd = (int)arg;
	char line[BUFF_SIZE];
	char method[BUFF_SIZE/10];
	char path[BUFF_SIZE/10];
	char url[BUFF_SIZE/10];

	bzero(line, sizeof (line));
	bzero(method, sizeof (line));
	bzero(path, sizeof (line));
	bzero(url, sizeof (line));
	int cgi = 0;
	/* 从请求行中提取方法和路径 */
	int size = get_line( connfd, line, sizeof (line) );
	if ( size < 0 ){
		//perror
		goto end;
	}

	printf ("%s\n", line);
	int i = 0;
	while (!isspace( line[i] ) && i < size){ /*提取方法 GET or POST or ...*/
		method[i] = line[i];
		i++;
	}
	method[i] = 0;

	while (isspace(line[i])) /* 筛除方法和路径之间的空格 */
		i++;
	
	int j = 0;
	while (!isspace(line[i])){	/*提取url*/
		url[j] = line[i];
		j++; i++;
	}
	url[j] = 0;

	printf("method is %s, url is %s\n",method, url);
	
	char* query_string = NULL;
	if ( strcasecmp("GET", method) == 0 ) {	
		for (i = 0; i < j; ++i){
			if (url[i] == '?'){	/*GET 请求，参数与路径用 '?' 号隔开 */
				url[i] = 0;
				query_string = &url[++i];
				break;
			}
		}
		sprintf(path, "%s", url+1);
		if (path[strlen(path - 1)] == '/'){
			strcat(path, "index.html");
		}else 
		printf("path is %s\n", path);
	}
	if (query_string)	/* 有参数，用cgi */
		cgi = 1;
	/* 得到 path, 得到 query_string*/
	struct stat st;
	if (-1 == stat(path, &st)){
		//404

		return (void*) 3;
	}
	if ( S_ISDIR(st.st_mode) ) {	/* 该文件是目录，返回静态网页 */
		strcat(path, "index.html");
		printf("%s\n", path);
	}else if (st.st_mode & S_IXUSR ||st.st_mode & S_IXGRP ||st.st_mode & S_IXOTH){
		cgi = 1;
	} else {}	/*other file type*/

	if (cgi){
		//exec_cgi();
	}else {
		drop_header(connfd);
		echo_www(connfd, path, st.st_size);
	}
end:	
	close(connfd);
}

static void echo_www(int sock, const char* path, ssize_t size)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0){
		perror("open");
		return;
	}

	char status[20];
	sprintf(status, "HTTP1.0 200 OK\r\n\r\n");

	if (-1 == send(sock, status, strlen(status), 0)){
		perror("send");
		return;
	}

	if (-1 == sendfile(sock, fd, NULL, size)){
		perror("sendfile");
		return;
	}
	close(fd);
}
