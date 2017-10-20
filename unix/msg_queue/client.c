#include "comm.h"

int main()
{

	//creat message queue
	int msgid = get_msgid();
	char buf[1024];

	for(;;){
		
		buf[0] = 0;

		printf ("please enter:");
		fflush(stdout);

		ssize_t size = read(0, (void*)buf, sizeof (buf));
		buf[size-1] = 0;

		send_msg(msgid, buf, CLIENT);
		if (0 == strcmp(buf, "exit")){
			break;
		}

		rcv_msg(msgid, buf, SERVER);
		printf("server say:%s\n", buf);
		if (0 == strcmp(buf, "exit")){
			break;
		}
	}	
	
	destory_msg_queue(msgid);
	
	return 0;
}
