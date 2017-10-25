#include "httpd.h"

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
	const char* msg = "HTTP/1.0 200 OK\r\n\r\n\r\n";
	const char* path = "wwwroot/index.html";
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
void printf_log(int sock, const char* msg)
{
	send(sock, msg, strlen(msg), 0);
	close(sock);
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
	if ( size <= 0 ){
		printf_log(connfd, "HTTP/1.0 400 notfound_1\r\n\r\n\r\n");
		goto end;
	}

	//printf ("%s\n", line);
	int i = 0;
	while (!isspace( line[i] ) && i < size){
		// get method 
		method[i] = line[i];
		i++;
	}
	method[i] = 0;

	while (isspace(line[i])) // clear space 
		i++;
	
	int j = 0;
	while (!isspace(line[i])){	
		// get url 
		url[j] = line[i];
		j++; i++;
	}
	url[j] = 0;

	char* query_string = NULL;
	//get
	if ( strcasecmp("GET", method) == 0 ) {	//get query string
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
			printf("this is 237 : path is %s\n", path);
	}else {	
		//post 
		sprintf(path,"%s", url+1);
	}

	if (query_string)
		cgi = 1;
	
	struct stat st;
	if (-1 == stat(path, &st)){
		//404
		printf_log( connfd, "HTTP/1.0 400 notfounds_2\r\n\r\n\r\n");
		printf("path is %s",path);
		return (void*) 3;
	}
	if ( S_ISDIR(st.st_mode) ) {	/* 该文件是目录，返回静态网页 */
		strcat(path, "/index.html");
	}else if (st.st_mode & S_IXUSR ||st.st_mode & S_IXGRP ||st.st_mode & S_IXOTH){
		printf("path is %s\n", path);
		cgi = 1;
	} else {}	/*other file type*/

	if (cgi){
		exec_cgi(connfd, method, query_string, path);
	}else {
		drop_header(connfd);
		echo_www(connfd, path, st.st_size);
	}
end:	
	close(connfd);
}

static void echo_www(int sock, const char* path, ssize_t size)
{
	const char* msg = "HTTP/1.0 200 OK\r\n";

	const char* type = "Content-Type: text/html;charset=ISO-8859-1\r\n\r\n";	
	
	int fd = open(path, O_RDONLY);
	if (fd < 0){
		perror("open");
		
		printf_log(sock, "HTTP/1.0 400 notfound_3\r\n\r\n\r\n");
		return;
	}

	char status[20];

	if (-1 == send(sock, msg, strlen(msg), 0) || -1 == send(sock, type, strlen(type), 0)){
		perror("send");
		printf_log(sock, "HTTP/1.0 400 notfound_4\r\n\r\n\r\n");
		return;
	}

	if (-1 == sendfile(sock, fd, NULL, size) ){
		perror("sendfile");
		printf_log(sock, "HTTP/1.0 400 notfound_5\r\n\r\n\r\n");
		return;
	}
	close(fd);
}

static void  exec_cgi(int sockfd, const char* method, const char* query_string, const char* path)
{
	char method_env[BUFF_SIZE/10];
	char query_string_env[BUFF_SIZE/10];
	char content_len_env[BUFF_SIZE/10];
	bzero(method_env, sizeof (method_env));
	bzero(query_string_env, sizeof (query_string_env));
	bzero(content_len_env, sizeof (content_len_env));
	
	int input[2];
	int output[2];
	int content_len = -1;

	if ( !strcasecmp(method, "GET") ){	//clear request socket, get query string

		drop_header(sockfd);
	}
	
	char buf[BUFF_SIZE/10];
	if ( !strcasecmp(method, "POST") ){	//get Content-Length send msg for child
		int ret = -1;
		do{
			bzero(buf, sizeof (buf));
			ret = get_line(sockfd, buf, sizeof (buf));
			if ( !strncasecmp(buf, "Content-Length: ", 16) ){
				content_len = atoi(&buf[16]);
			}
		}while ( ret > 0 && strcmp(buf, "\n") );
		if (-1 == content_len){
			printf_log(sockfd, "HTTP/1.0 400 notfound_6\r\n\r\n\r\n");
			return;
		}
	}

	if (-1 == pipe(input) || -1 == pipe(output)){
		perror("pipe");
		printf_log(sockfd, "HTTP/1.0 400 notfound_7\r\n\r\n\r\n");
		return;
	}

	pid_t id = fork();
	if (id < 0){
		printf_log(sockfd, "HTTP/1.0 400 notfound_8\r\n\r\n\r\n");
		perror("fork");
		return;
	}else if (id == 0){//child
		close(input[1]);
		close(output[0]);
		/* 程序替换后子进程通过往 fd 0 给父进程发送消息，从 fd 1 读取父进程发送过来的消息 */
		
		dup2(input[0], 0);
		dup2(output[1],1);
		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);
		if ( !strcasecmp( "get", method) ){
			if (!query_string){
				//echo_errno
				printf("query_string is NULL\n");
				printf_log(sockfd, "HTTP/1.0 400 notfound_9\r\n\r\n\r\n");
				return;
			}
			
			sprintf(query_string_env, "QUERY_STRING_ENV=%s", query_string);
			putenv(query_string_env);
			execl(path, path, NULL);
			printf_log(sockfd, "HTTP/1.0 400 notfound_10\r\n\r\n\r\n");
			printf("exit, path is %s",path);
			exit(1);
		}else if ( !strcasecmp("POST", method) ){ //post method
			if (content_len < 0){
				printf_log(sockfd, "HTTP/1.0 400 notfound_11\r\n\r\n\r\n");
				return;
			}
			//putenv to child to read 0(output[0])
			sprintf(content_len_env, "CONTENT_LEN_ENV=%d", content_len);
			putenv(content_len_env);
			execl(path, path, NULL);
			exit(1);
		}else {
			//other method
			printf("other method is %s\n", method);
		}	
	}else{
		//father
		close(input[0]);
		close(output[1]);
		/* response to server */	
		const char* msg = "HTTP/1.0 200 OK\r\n";
		send(sockfd, msg, strlen(msg), 0);
		//Content-type -> html
		const char* type = "Content-Type: text/html;charset=ISO-8859-1\r\n\r\n";	
		send(sockfd, type, strlen(type), 0);

		char c = '\0';
		int ret = -1;
		//char tmp[1024];
		if ( !strcasecmp("POST", method)){	//method is post,send msg to child
			int i = 0;
			for (; i < content_len; ++i){
				recv(sockfd, &c, 1, 0);
				write(input[1], &c, sizeof (char));
				//tmp[i] = c;
			}
		}
		//printf("post -> server : %s\n", tmp);

		while ( read(output[0], &c, 1) > 0 ){	//send child data to server
			send(sockfd, &c, 1, 0);
		}
		//tmp[i] = 0;
		waitpid(id, NULL, 0);
		close(input[1]);
		close(output[0]);
	}
}
