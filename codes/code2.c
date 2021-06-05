#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

pthread_mutex_t m_lock, m_lock_2;
unsigned int count;

int calculateNextValue(int value)
{
	return (value * 2 + 2) / 2;
}

void *thread_function(void *arg) {
	int i, j;
	
	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 1000; j++) {
			pthread_mutex_lock(&m_lock_2);
			pthread_mutex_lock(&m_lock);
		
			count = calculateNextValue(count);

			pthread_mutex_unlock(&m_lock);
			pthread_mutex_unlock(&m_lock_2);
		}
	}
	
	return NULL;
}

void *thread_function2(void *arg) {
	int i, j;
	
	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 1000; j++) {
			pthread_mutex_lock(&m_lock);
			pthread_mutex_lock(&m_lock_2);
		
			count = calculateNextValue(count);
		
			pthread_mutex_unlock(&m_lock_2);
			pthread_mutex_unlock(&m_lock);
		}
	}
	
	return NULL;
}

int main()
{
	pthread_t mythread, mythread2;
	
	count = 0;
	pthread_mutex_init(&m_lock, NULL);
	pthread_mutex_init(&m_lock_2, NULL);

	pthread_create( &mythread, NULL, thread_function, NULL);
	pthread_create( &mythread2, NULL, thread_function2, NULL);
		
	pthread_join ( mythread, NULL );
	pthread_join ( mythread2, NULL );
	pthread_mutex_destroy(&m_lock_2);
	pthread_mutex_destroy(&m_lock);
	
	printf("%d\n", count);
	
	return 0;
}
