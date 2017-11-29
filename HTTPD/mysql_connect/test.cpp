#include "mysql_signals.h"


int main()
{
	info* msg = new info();
	unsigned char ch = 0;
	
	//father dup2(0, output(0)); stdin father -> child post information
	if (getenv("METHOD")){
		strcpy(msg->_method, getenv("METHOD"));
		//printf("%s\n", msg->_method);
		if (strcasecmp("POST", msg->_method) == 0){
			if (getenv("CONTENT_LEN_ENV")){
				msg->_len = atoi(getenv("CONTENT_LEN_ENV"));
				int i = 0;
				for (; i < msg->_len; ++i){
					//if (i = 1023) -> process data & bzero _msg
					read(0, &ch, sizeof (char));
					msg->_msg[i] = ch;
				}
				//printf("22 : child recv from parent msg is %s\n", msg->_msg);
			}else {
				printf("content len env is NULL\n");
				delete msg;
				return 1;
			}
		}else if (strcasecmp("GET", msg->_method) == 0){
			if (getenv("QUERY_STRING_ENV")){
				strcpy(msg->_msg, getenv("QUERY_STRING_ENV"));
				printf("get : %s\n", msg->_msg);
			}else {
				printf("query string  env is NULL\n");
				delete msg;
				return 2;
			}
		}
	}else {
		printf("method env is NULL\n");
		delete msg;
		return 3;
	}
	process(msg);
	delete msg;
	return 0;
}
