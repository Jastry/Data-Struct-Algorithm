#include <stdio.h>
#include <pthread.h>

int gCount = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* Add_count(void* arg)
{
	int i = 0;
	
	pthread_mutex_lock(&lock);
	for (i = 0; i < 50000000; ++i){
		++gCount;
		pthread_mutex_unlock(&lock);
	}
}

int main()
{
	pthread_t tid_1;
	pthread_t tid_2;

	pthread_create(&tid_1, NULL, Add_count, NULL);
	pthread_create(&tid_2, NULL, Add_count, NULL);

	pthread_mutex_destroy(&lock);

	pthread_join(tid_1, NULL);
	pthread_join(tid_2, NULL);
	printf("gCount is %d\n", gCount);
	return 0;
}
