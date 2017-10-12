#ifndef __COMM_H__
#define __COMM_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>

union semun{
	int              val;    /* Value for SETVAL */
   	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO(
								Linux-specific) */
};

#define PATHNAME "."
#define PROJ_ID 88

int init_sem(int semid, int which, int initval);
int creat_sem(int nsems);
int get_sem(int nsems);
int P(int semid, int which);
int V(int semid, int which);
void destory_sem(int semid);

#endif //__COMM_H__
