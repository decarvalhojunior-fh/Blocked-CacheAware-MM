#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <omp.h>
#include <string.h>

int N = 0;
int block_size[3] = {25, 50, 200}; 
int blocked = -1;

int P=1;

void fill(int n, double* m);
void mm_blocked(int level, int size, double* a, double* b, double* c, int a_x, int a_y, int b_x, int b_y);
void mm_simple(int size, double* a, double* b, double* c);

void main(int argc, char* argv[])
{
   char *p;
   N = strtol(argv[1], &p, 10);
   if (argc > 2)
      P = strtol(argv[2], &p, 10);
   if (argc > 3)
      blocked = !strcmp(argv[3],"blocked") || strcmp(argv[3],"simple");

   printf("start ... N=%d blocked=%d P=%d\n", N, blocked, P); fflush(stdout);
 
   double* a = malloc(N*N*sizeof(double));
   double* b = malloc(N*N*sizeof(double));
   double* c = malloc(N*N*sizeof(double));

   printf("malloc ...\n"); fflush(stdout);

   fill(N,a);
   fill(N,b);
   fill(N,c);

   printf("filled ...\n"); fflush(stdout);
 
   clock_t begin = clock();

   if (blocked)
      mm_blocked(3, N, a, b, c, 0, 0, 0, 0);
   else
      mm_simple(N, a, b, c);

   clock_t end = clock();
   double time_spent = (double)(end - begin) / (CLOCKS_PER_SEC * P);

   printf("elapsed time = %f\n", time_spent); fflush(stdout);
}

void mm_blocked(int level, int size, double* a, double* b, double* c, int a_x, int a_y, int b_x, int b_y)
{
   if (level == 0)
   {
      int limit_i   = a_x + size;
      int limit_j   = b_y + size;
      int limit_k_a = a_y + size;

      for (int i=a_x; i<limit_i; i++)
          for (int j=b_y; j<limit_j; j++)
               for (int k_a=a_y, k_b = b_x; k_a<limit_k_a; k_a++, k_b++)
                   *(c + i*N + j) += *(a + i*N + k_a) + *(b + k_b*N + j); 
   }
   else
   {
      int bs = block_size[level-1];  // block size.
      int nb = size/bs;              // number of blocks.

      #pragma omp parallel for num_threads(P) if (level==3)
      for (int i=0; i<nb; i++)
          for (int j=0; j<nb; j++)
               for (int k=0; k<nb; k++)
                  mm_blocked(level-1, bs, a, b, c, a_x + i*bs, a_y + k*bs, b_x + k*bs, b_y + j*bs);
   }


}

void mm_simple(int size, double* a, double* b, double* c)
{
      #pragma omp parallel for num_threads(P)
      for (int i=0; i<size; i++)
          for (int j=0; j<size; j++)
               for (int k=0; k<size; k++)
                   *(c + i*N + j) += *(a + i*N + k) + *(b + k*N + j); 
}


void fill(int n, double* m)
{
    for (int i=0; i<n; i++)
        for (int j=0; j<n; j++)
	    *(m + i*N + j) = (double) rand();
}
