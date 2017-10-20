#include "comm.h"


static int comm_semget(int nsems, int flags){
	key_t _key = ftok(PATHNAME, PROJ_ID);
	if (_key == -1){
		perror("ftok");
		return -1;
	}

	int semid = semget(_key, nsems, flags);
	if (semid < 0){
		perror("semget");
		return -2;
	}
	return semid;
}

int init_sem(int semid, int which, int initval){
	union semun sem_un;
	sem_un.val = initval;

	int ret = semctl(semid, which, SETVAL, sem_un);
	if (ret < 0){
		perror("semctl");
		return -1;
	}
	return 0;
}

int creat_sem(int nsems){
	return comm_semget(nsems, IPC_CREAT | IPC_EXCL | 0666);
}

int get_sem(int nsems){
	return comm_semget(nsems, IPC_EXCL | 0666);
}

void destory_sem(int semid){
	semctl(semid, IPC_RMID, 0);
}

static int sem_PV(int semid, int which, int option){
	struct sembuf sem_buf;
	memset(&sem_buf, 0, sizeof (sem_buf));

	sem_buf.sem_num	 = which;
	sem_buf.sem_op	 = option;
	sem_buf.sem_flg  = SEM_UNDO;/*0*/

	int ret = semop(semid, &sem_buf, 1); /*number of semophore*/
	if (-1 == ret){
		perror("semop");
		return -1;
	}
	return 0;
}

int P(int semid, int which){
	return sem_PV(semid, which, -1);
}
int V(int semid, int which){
	return sem_PV(semid, which, 1);
}
