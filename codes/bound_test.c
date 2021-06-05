#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

//P = 1 / PROBABILITY_OF_IO
#define PROBABILITY_OF_IO 100000

#define DATA_SIZE 1000000

typedef struct {
	float x, y;
} Point;

typedef struct {
	Point *array;
	int size;
	int r; //r : radius
} Param;

//Occasionally, when page fault happens, it delay (sleep) longer.
void doIOProcess()
{
	int r;
	
	r = (rand() * rand()) % PROBABILITY_OF_IO;
	if (r == 1) {
		//sleep(5); //windows
		usleep(5 * 1000); //linux
		return ;
	}
	
	return ;
}

void *thread_function(void *args) {
	int i;
	int ans;
	Param *param = (Param *)args;
	
	ans = 0;
	for (i = 0; i < param->size; i++) {
		doIOProcess();
		if (sqrt(pow(param->array[i].x, 2) + pow(param->array[i].y, 2)) <= param->r) {
			ans++;
		}
	}
	
	pthread_exit((void *)ans);
}

int main()
{
	pthread_t mythread[4];
	int i;
	Param p[4];
	int ans, temp;
	Point *array;
	
	srand(time(0));
	
	array = (Point *)malloc(sizeof(Point) * DATA_SIZE);
	for (i = 0; i < DATA_SIZE; i++) {
		array[i].x = (((double)rand() / RAND_MAX));
		array[i].y = (((double)rand() / RAND_MAX));
	}
	
	for (i = 0; i < 4; i++) {
		p[i].size = DATA_SIZE / 4;
		p[i].array = array + i * p[i].size;
		p[i].r = 1;
	}
	
	for (i = 0; i < 4; i++) {
		pthread_create( &mythread[i], NULL, thread_function, &p[i]);
	}

	ans = 0;
	for (i = 0; i < 4; i++) {
		pthread_join( mythread[i], (void **)&temp );
		ans += temp;
	}
	
	printf("PI = %0.5lf\n", 4.0 * (double)ans / (double)DATA_SIZE);
	free(array);
	
	return 0;
}
