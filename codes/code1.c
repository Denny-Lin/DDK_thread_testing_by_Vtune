#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

pthread_mutex_t m_lock;
unsigned int count, flag;

int calculateNextValue(int value)
{
	return (value * 5 + 4) / 4;
}

void *thread_function(void *arg) {
	int i;
	
	for (i = 0; i < 1000; i++) {
		pthread_mutex_lock(&m_lock);
		count = calculateNextValue(count);
		pthread_mutex_unlock(&m_lock);
	}
	flag = flag + 3;

	return NULL;
}

void *thread_function2(void *arg) {
	int i;
	
	for (i = 0; i < 1000; i++) {
		pthread_mutex_lock(&m_lock);
		count = calculateNextValue(count);
		pthread_mutex_unlock(&m_lock);
	}
	flag = flag + 3;

	return NULL;
}

int main()
{
	pthread_t mythread, mythread2;
	
	count = 0;
	flag = 0;
	pthread_mutex_init(&m_lock, NULL);

	pthread_create( &mythread, NULL, thread_function, NULL);
	pthread_create( &mythread2, NULL, thread_function2, NULL);

	while (flag != 6);
	pthread_join ( mythread, NULL );
	pthread_join ( mythread2, NULL );
	pthread_mutex_destroy(&m_lock);
	
	printf("%d\n", count);
	
	return 0;
}
