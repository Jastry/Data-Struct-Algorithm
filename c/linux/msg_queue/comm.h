#ifndef __COMM_H__
#define __COMM_H__

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PATHNAME "."
#define PROJ_ID 06666

#define SERVER 1
#define CLIENT 2

struct msgbuf
{
	long mtype;
	char mtext[1024];
};


void destory_msg_queue(int msgid);

int send_msg(int msgid, char msg[], long flgType);

int rcv_msg(int msgid, char msg[], long rcvType);

int creat_msgid();

int get_msgid();



#endif //__COMM_H__
