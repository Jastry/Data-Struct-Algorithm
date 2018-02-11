#include "httpd.h"

void usage(const char* proc)
{
	printf("usage : \n\t%s [local_ip] [local_port]\n\n", proc);
}

static int get_line(int sockfd, char* line, int size)
{
	/*
     * ret > 1,line != '\0' 表示读的是数据;
     * ret == 1 && line == '\n'表示空行;
     * ret <= 0 && line == '\0'读完了;
     * '\r' -> '\n'; '\r\n' -> '\n' 
     */

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

void not_found(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

//void headers(int client/*, const char *filename*/)
void headers(int client)
{
    char buf[1024];
    //(void)filename;  /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}

void* header_request(void* arg)
{
	int  connfd = (intptr_t)arg;
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
        cannot_execute(connfd);
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
        not_found(connfd);
		printf("path is %s",path);
		return (void*) 3;
	}
	if ( S_ISDIR(st.st_mode) ) {	/* 该文件是目录，返回静态网页 */
		strcat(path, "/index.html");
	}else if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) {
		//printf("there is 224 ,path is %s\n", path);
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

static void echo_www(int sockfd, const char* path, ssize_t size)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
        not_found(sockfd);
		return;
	}
    headers(sockfd);
	if ( -1 == sendfile(sockfd, fd, NULL, size) ) {
        cannot_execute(sockfd);
		return;
	}
	close(fd);
}

static void  exec_cgi(int sockfd, const char* method, const char* query_string, const char* path)
{
	char method_env[255];
	char query_string_env[255];
	char content_len_env[255];
	bzero(method_env, sizeof (method_env));
	bzero(query_string_env, sizeof (query_string_env));
	bzero(content_len_env, sizeof (content_len_env));
	
	int input[2];
	int output[2];
	int content_len = -1;

	if (strcasecmp(method, "GET") == 0){	//clear request socket, get query string

		drop_header(sockfd);
	}
	
	char buf[BUFF_SIZE/10];
	if (strcasecmp(method, "POST") == 0){	//get Content-Length send msg for child
		int ret = -1;
		do{
			bzero(buf, sizeof (buf));
			ret = get_line(sockfd, buf, sizeof (buf));
			if ( !strncasecmp(buf, "Content-Length: ", 16) ){
				content_len = atoi(&buf[16]);
			}
		}while ( ret > 0 && strcmp(buf, "\n") );
		if (-1 == content_len){
			bad_request(sockfd);
            return;
		}
	}

	if (-1 == pipe(input) || -1 == pipe(output)){
        cannot_execute(sockfd);
        return;
	}

	pid_t id = fork();
	if (id < 0){
        cannot_execute(sockfd);
		return;
	}

    /* response to server */	
    const char* msg = "HTTP/1.0 200 OK\r\n";
    send(sockfd, msg, strlen(msg), 0);
    
    if (id == 0){//child
	
        close(input[1]);
		close(output[0]);
		/* 程序替换后子进程通过往 fd 0 给父进程发送消息，从 fd 1 读取父进程发送过来的消息 */
		
		dup2(input[0], 0);
		dup2(output[1],1);
		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);
		if ( strcasecmp( "get", method) ==  0) {
			sprintf(query_string_env, "QUERY_STRING_ENV=%s", query_string);
			putenv(query_string_env);
		} else if ( strcasecmp("POST", method) == 0) { //post method
			sprintf(content_len_env, "CONTENT_LEN_ENV=%d", content_len);
			putenv(content_len_env);
		} else {
			//other method
			printf("other method is %s\n", method);
		}
        execl(path, path, NULL);
        exit(0);
	} else {
		//father
		close(input[0]);
		close(output[1]);

		char c = '\0';
		int ret = -1;
		if ( strcasecmp("POST", method) == 0){	//method is post,send msg to child
			int i = 0;
			for (; i < content_len; ++i){
				recv(sockfd, &c, 1, 0);
				write(input[1], &c, sizeof (char));
			}
		}
		while ( read(output[0], &c, 1) > 0 ){	//send child data to server
			send(sockfd, &c, 1, 0);
		}
		waitpid(id, NULL, 0);
		close(input[1]);
		close(output[0]);
	}
}

