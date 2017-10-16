#include <stdio.h>
#include <stdlib.h>

int main()
{
	const char* msg = getenv("QUERY_STRING_ENV");
	if (msg){
		printf("query string is %s\n", msg);
	}
	else 
		printf("SHELL is %s\n", getenv("SHELL"));
	return 0;
}
