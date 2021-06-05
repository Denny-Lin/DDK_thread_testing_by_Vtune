#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LEVEL 1

unsigned int num = 0, flag = 0;

pthread_mutex_t num_lock = PTHREAD_MUTEX_INITIALIZER,
				malloc_lock = PTHREAD_MUTEX_INITIALIZER;
                
typedef struct _arg{
	int rowIndex;
	int sizeN;
    char** checkerboard;
}ArgType;

void queens(int x);
void print(char** checkerboard, int size);
void *queen_thread(void *arg);
char** createNewCheckBoard(int n);
void freeCheckBoard(char** checkerboard, int size);
int checkCorrectness(ArgType *param, int index);
ArgType *makeCopyOfArgType(ArgType *param);
void freeArgType(ArgType *param);

//Timing function!!!
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

void access_counter(unsigned *hi, unsigned *lo)
{
  /* Get cycle counter */
  asm("rdtsc; movl %%edx,%0; movl %%eax,%1"
      : "=r" (*hi), "=r" (*lo)
      : /* No input */
      : "%edx", "%eax");
}

void start_counter()
{
  /* Get current value of cycle counter */
  access_counter(&cyc_hi, &cyc_lo);
}


double get_counter()
{
  unsigned ncyc_hi, ncyc_lo;
  unsigned hi, lo, borrow;
  /* Get cycle counter */
  access_counter(&ncyc_hi, &ncyc_lo);
  /* Do double precision subtraction */
  lo = ncyc_lo - cyc_lo;
  borrow = lo > ncyc_lo;
  hi = ncyc_hi - cyc_hi - borrow;
  return (double) hi * (1 << 30) * 4 + lo;
}

int main(void)
{
    int i, j, rowIndex;
	ArgType arg;
	ArgType *param;
	int n;
	double executionTime;
	
    printf("Please input the size of chessboard: ");
    scanf("%d", &n);
    start_counter();
	//Initialize global variables
    num = 0;
        
	arg.rowIndex = 0;
	arg.sizeN = n;
	arg.checkerboard = createNewCheckBoard(n);
	//This is just a demo, so we make a clone copy of arg and pass it to queen_thread.
	param = makeCopyOfArgType(&arg);

	pthread_mutex_init(&num_lock, NULL);
	pthread_mutex_init(&malloc_lock, NULL);
	
	queen_thread((void *)param);
	
	pthread_mutex_destroy(&malloc_lock);
	pthread_mutex_destroy(&num_lock);
	
	//free all variables
	freeCheckBoard(arg.checkerboard, n);
	
    printf("\nTotal number of solutions : %d \n\n", num);

	executionTime = get_counter();
	printf("referenced execution cycles : %lf\n", executionTime / 1000000);
	//Executing in Linux does not need this....
    system("pause");
	return 0;
}

void *queen_thread(void *arg) {
	ArgType *param = arg;
	int rowIndex = param->rowIndex;
	int n = param->sizeN;
	char **checkerboard = param->checkerboard;
	int i;
	ArgType *param_temp = NULL;
	pthread_t *mythread = NULL;
	char *mythread_used = NULL;
	
	if (rowIndex < LEVEL) {
		mythread = (pthread_t *)malloc(sizeof(pthread_t) * n);
		mythread_used = (char *)malloc(sizeof(char) * n);
		for (i = 0; i < n; i++) {
			mythread_used[i] = 0;
		}
	}
	
	if (rowIndex < n)
    {
        for (i = 0; i < n; i++)
        {
			if (checkCorrectness(param, i)) {
				checkerboard[rowIndex][i] = 'Q';
				
                if (rowIndex < LEVEL) {
					//while is used for preventing fail allocation of parameter
					while (param_temp == NULL) {
						param_temp = makeCopyOfArgType(param);
					}
					param_temp->rowIndex++;
					
					//Debuging
					//printf("%d(%x):%d, %d\n", rowIndex, pthread_self(), param_temp->rowIndex, param_temp->sizeN);
					
					//while is used for preventing fail creation of thread
					while (pthread_create(&mythread[i], NULL, queen_thread, (void *)param_temp));
					mythread_used[i] = 1;
					param_temp = NULL;
				}
				else {
					param->rowIndex++;
                	queen_thread(param);
					param->rowIndex--;
				}
				
				checkerboard[rowIndex][i] = 'x';
			}
        }
    }
    else
    {
        //Output the chessboard
        pthread_mutex_lock(&num_lock);
        //print(checkerboard, n);
        num++;
        pthread_mutex_unlock(&num_lock);
    }
    
    //Wait and Clean all threads
	if (rowIndex < LEVEL) {
		flag = 0;
		for (i = 0; i < n; i++) {
			if (mythread_used[i])
				pthread_join(mythread[i], NULL);
		}
		free(mythread);
	}
	//Clean parameter for this function
	if (rowIndex < LEVEL + 1) {
		freeArgType(param);
	}
	
	return ;
}

void print(char** checkerboard, int size)
{
    int i, j;

    printf("\n");
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            printf("%c", checkerboard[i][j]);
        }

        printf("\n");
    }
	return ;
}


char** createNewCheckBoard(int n)
{
    char** checkerboard;
    int i, j;
    
    checkerboard = malloc(sizeof(char*) * n);
    
    for (i = 0; i < n; i++)
    {
        checkerboard[i] = malloc(sizeof(char) * n);
        for (j = 0; j < n; j++)
        {
            checkerboard[i][j] = 'x';
        }
    }
    
    return checkerboard;
}

void freeCheckBoard(char** checkerboard, int size)
{
	int i;
	
    for (i = 0; i < size; i++)
    {
        free(checkerboard[i]);
    }

    free(checkerboard);
}

int checkCorrectness(ArgType *param, int index)
{
	int rowIndex = param->rowIndex;
	int n = param->sizeN;
	char **checkerboard = param->checkerboard;
	int i, left, right;
	
	left = index - 1;
	right = index + 1;
	for (i = rowIndex - 1; i >= 0; i--) {
		//Check above
		if (checkerboard[i][index] == 'Q')
			return 0;
		//Check left above
		if (left >= 0 && (checkerboard[i][left] == 'Q'))
			return 0;
		//Check right above
		if (right <= n - 1 && (checkerboard[i][right] == 'Q'))
			return 0;
		
		left--;
		right++;
	}
	
	return 1;
}
    
ArgType *makeCopyOfArgType(ArgType *param)
{
	ArgType *new_arg = (ArgType *)malloc(sizeof(ArgType));
	int i, j;
	
	new_arg->rowIndex = param->rowIndex;
	new_arg->sizeN = param->sizeN;
	new_arg->checkerboard = createNewCheckBoard(param->sizeN);
	
	for (i = 0; i < new_arg->sizeN; i++)
    {
        for (j = 0; j < new_arg->sizeN; j++)
        {
            new_arg->checkerboard[i][j] = param->checkerboard[i][j];
        }
    }
    
	return new_arg;
}

void freeArgType(ArgType *param)
{
	freeCheckBoard(param->checkerboard, param->sizeN);
	free(param);
	
	return ;
}
