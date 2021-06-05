#include <stdio.h>
#include <stdlib.h>

void queens(int x);
void print(void);

unsigned int n, num;
char** checkerboard;
int* column;
int* right;
int* left;

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
    int i, j;
	double executionTime;
	
    printf("Please input the size of chessboard: ");
    scanf("%d", &n);
	
	start_counter();
	
    //Memory allocation
    column = malloc(sizeof(int) * n);
    right = malloc(sizeof(int) * (2 * n - 1));
    left = malloc(sizeof(int) * (2 * n - 1));
    checkerboard = malloc(sizeof(char*) * n);

    //Initialize variables
    num = 0;
    for (i = 0; i < n; i++)
    {
        column[i] = 1;

        checkerboard[i] = malloc(sizeof(char) * n);
        for (j = 0; j < n; j++)
        {
            checkerboard[i][j] = 'x';
        }
    }

    for (i = 0; i < 2 * n - 1; i++)
    {
        right[i] = left[i] = 1;
    }

    queens(0);

    printf("\nTotal %d solutions\n\n", num);

    //Release memory space
    for (i = 0; i < n; i++)
    {
        free(checkerboard[i]);
    }

    free(checkerboard);
    free(column);
    free(right);
    free(left);
	
	executionTime = get_counter();
	printf("referenced execution cycles : %lf\n", executionTime / 1000000);
	//Executing in Linux does not need this....
    system("pause");
}

void queens(int x)
{
    if (x < n)
    {
        int i;
        for (i = 0; i < n; i++)
        {
            int j = i - x + n - 1;
            int k = i + x;
			//left and right are arrays for special process
            if (column[i] && right[j] && left[k])
            {
                //Mark Queens and recursive
                column[i] = right[j] = left[k] = 0;
                checkerboard[x][i] = 'Q';

                queens(x + 1);

                column[i] = right[j] = left[k] = 1;
                checkerboard[x][i] = 'x';
            }
        }
    }
    else
    {
        //Print out hte chessboard
        //print();
        num++;
    }
}

void print(void)
{
    int i, j;

    printf("\n");
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            printf("%c", checkerboard[i][j]);
        }

        printf("\n");
    }
}

    
