#include "comm.h"

int main()
{

	//creat message queue
	int msgid = creat_msgid();
	char buf[1024];

	for(;;){

		rcv_msg(msgid, buf, CLIENT);
		printf("client say:%s\n", buf);
		if (0 == strcmp(buf, "exit")){
			break;
		}
		
		buf[0] = 0;
		printf("please enter: ");
		fflush(stdout);
		ssize_t size = read(0, (void*)buf, sizeof (buf));
		buf[size - 1] = 0;
		
		send_msg(msgid, buf, SERVER);
		if (0 == strcmp(buf, "exit")){
			break;
		}
	}	
	
	destory_msg_queue(msgid);
	
	return 0;
}
