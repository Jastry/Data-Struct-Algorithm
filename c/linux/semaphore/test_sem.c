#include "comm.h"

int main(int argc, char const *argv[])
{
	/* Create a new semaphore set with only one semaphore */
	int semid = creat_sem(1);
	/* Initializes the semaphore set and initializes the first semaphore to one */
	init_sem(semid, 0, 1);

	pid_t id = fork();
	if (id < 0){ 	
		perror("fork");
		return -1;
	}else if (0 == id){/*child*/
		int _semid = get_sem(0);
		int num = 10;
		while (num--) {
			printf ("child_num is %d\n", num);
			P(_semid, 0);
			printf("im am child, im watching TV...\n");
			usleep(300000);
			printf("im had go to school\n");
			usleep(100000);
			if (num == 7)
				exit(1);
			V(_semid, 0);
		}
	}else {/*father*/
		int _num = 10;
		while (_num--) {
			printf ("father num is %d\n", _num);
			P(semid, 0);
			printf("im cook for my child...\n");
			usleep(300000);
			printf("Dinner, children\n");
			usleep(100000);
			V(semid, 0);
		}
		int status = 0;
		int ret = waitpid(id, &status, 0);
		if (ret > 0){
			printf("waite success\n");
			destory_sem(semid);
		}
	}
	return 0;
}
