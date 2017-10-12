#include "comm.h"

static int creat_msg_queue(int msgflg){

	key_t _key = ftok(PATHNAME, PROJ_ID);
	if (_key == -1){
		perror("ftok");
		return -1;
	}

	int msgid = msgget(_key, msgflg);
	if (msgid < 0){
		perror("msgget");
		return -2;
	}
	return msgid;
}

void destory_msg_queue(int msgid){

	msgctl(msgid, IPC_RMID, NULL);
}

int send_msg(int msgid, char msg[], long flgType){
	assert(msg != NULL);

	struct msgbuf buf;
	buf.mtype = flgType;
	strcpy(buf.mtext, msg);
	
	if (msgsnd(msgid, (void*)&buf,sizeof (buf.mtext), 0) < 0){
		perror("msgsnd");
		return -1;
	}
	return 0;
}	

int rcv_msg(int msgid, char msg[], long rcvType){

	struct msgbuf buf;
	ssize_t size = msgrcv(msgid, &buf, sizeof (buf.mtext), rcvType, 0);
	if (size < 0){
		perror("msgrcv");
		return -1;
	}
	
	strcpy(msg, buf.mtext);
	return 0;
}

int creat_msgid(){
	return creat_msg_queue(IPC_CREAT | IPC_EXCL | 0666);
}

int get_msgid(){
	return creat_msg_queue(IPC_EXCL | 0666);
}
