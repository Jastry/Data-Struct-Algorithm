#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

int main(int argc, char const *argv[])
{
	char 	buf[MAXLINE];
	pid_t 	pid;
	int 	status;

	printf("%% ");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		if ((pid = fork()) < 0){
			perror("fork");
			//err_sys("fork error");
		} else if (0 == pid) {	/*child*/
			execlp(buf, buf, (char*)0);
			//err_ret
			printf("couldn't execute : %s\n", buf);
			exit(127);
		}
		/*parent*/
		if ((pid = waitpid(pid, &status, 0)) < 0)
			//err_sys("waitpid error");
			perror("waitpid error");
		printf("%% ");
	}
	return 0;
}
