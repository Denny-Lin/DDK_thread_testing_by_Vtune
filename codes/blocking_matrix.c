#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 1000
#define BLOCK_WIDTH 100
//#define TEST_CORRECTNESS
//#define PRINT_OUT_RESULT

//Timing function
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

void access_counter(unsigned *hi, unsigned *lo);
void start_counter();
double get_counter();
//End of Timing function

//Matrix function
void multiply(int **A, int **B, int **F, int size);
void multiply_blocking(int **A, int **B, int **F, int size);
void printMatrix(int **array, int size);
int **createMatrix(int size);
int **createZeroMatrix(int size);
void freeMatrix(int **array, int size);
int compareMatrix(int **src, int **trgt, int size);
//End of Matrix function

int main()
{
	int **A = createMatrix(DATA_SIZE);
	int **B = createMatrix(DATA_SIZE);
	int **F = createZeroMatrix(DATA_SIZE);
	
	//make array random values and keep it the same everytime it runs.
	srand(100);
	start_counter();
	multiply_blocking(A, B, F, DATA_SIZE);
	printf("execution time = %lf Mega cycles\n", get_counter() / 1000000.0);
	#ifdef PRINT_OUT_RESULT
		printMatrix(A, DATA_SIZE);
		printMatrix(B, DATA_SIZE);
		printMatrix(F, DATA_SIZE);
	#endif
	
	#ifdef TEST_CORRECTNESS
		int **F2 = createZeroMatrix(DATA_SIZE);
		multiply(A, B, F2, DATA_SIZE);
		#ifdef PRINT_OUT_RESULT
			printMatrix(F2, DATA_SIZE);
		#endif
		if (compareMatrix(F, F2, DATA_SIZE)) {
			printf("Correct!\n");
		}
		else {
			printf("Wrong!\n");
		}
		freeMatrix(F2, DATA_SIZE);
	#endif
	
	freeMatrix(A, DATA_SIZE);
	freeMatrix(B, DATA_SIZE);
	freeMatrix(F, DATA_SIZE);
	system("pause");
	return 0;
}


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

void multiply(int **A, int **B, int **F, int size)
{
	int i, j, k;
	
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			for (k = 0; k < size; k++) {
				F[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	
	return ;
}

void multiply_blocking(int **A, int **B, int **F, int size)
{
	int i, j, k;
	int i2, j2;
	int i3, j3;
	
	if (DATA_SIZE % BLOCK_WIDTH != 0) {
		printf("error: wrong ratio of DATA_SIZE to BLOCK_WIDTH\n");
		return ;
	}
	
	i2 = 0;
	for (i = 0; i < size / BLOCK_WIDTH; i++) {
		j2 = 0;
		for (j = 0; j < size / BLOCK_WIDTH; j++) {
			for (k = 0; k < size; k++) {
				for (i3 = i2; i3 < i2 + BLOCK_WIDTH; i3++) {
					for (j3 = j2; j3 < j2 + BLOCK_WIDTH; j3++) {
						F[i3][j3] += A[i3][k] * B[j3][k];
					}	
				}
			}
			j2 += BLOCK_WIDTH;
		}
		i2 += BLOCK_WIDTH;
	}
	
	return ;
}

void printMatrix(int **array, int size)
{
	int i, j;
	
	printf("\n");
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			printf("%d, ", array[i][j]);
		}
		printf("\n");
	}
	
	return ;
}

int **createMatrix(int size)
{
	int i, j;
	int **array;
	
	array = (int **)malloc(sizeof(int *) * size);
	for (i = 0; i < size; i++) {
		array[i] = (int *)malloc(sizeof(int) * size);
		for (j = 0; j < size; j++) {
			array[i][j] = rand() % 10;
		}
	}
	
	return array;
}

int **createZeroMatrix(int size)
{
	int i, j;
	int **array;
	
	array = (int **)malloc(sizeof(int *) * size);
	for (i = 0; i < size; i++) {
		array[i] = (int *)malloc(sizeof(int) * size);
		for (j = 0; j < size; j++) {
			array[i][j] = 0;
		}
	}
	
	return array;
}

void freeMatrix(int **array, int size)
{
	int i;
	
	for (i = 0; i < size; i++) {
		free(array[i]);
	}
	free(array);
	
	return ;
}

int compareMatrix(int **src, int **trgt, int size)
{
	int i, j;
	
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (src[i][j] != trgt[i][j])
				return 0;
		}
	}
	
	return 1;
}
