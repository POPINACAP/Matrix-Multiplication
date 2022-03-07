// Compile with gcc -pthread -lm <source.c> -o <executable>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
   int x; /* row */
   int y; /* column */
}instruction;

int size; /* added to be used globally */

void *cell_multiply(void *param);
void read_matrix(FILE *f, int[10][10], int *, int *);
void print_matrix(char *, int[10][10], int, int);

int matrix_a[10][10];
int matrix_b[10][10];
int matrix_c[10][10];

int main(int argc, char *argv[]) {
   FILE *f = NULL;
   int row_a, col_a;
   int row_b, col_b;

   if (argc != 2) {
      fprintf(stderr,"usage: %s <file>\n", argv[0]);
      return 1;
   }

   f = fopen(argv[1], "r");
   if (f == NULL) {
      fprintf(stderr, "Can't open file %s\n", argv[1]);
      return -1;
   }

   printf("Multi-threaded Matrix Multiplication\n");
   printf("\n");

   read_matrix(f, matrix_a, &row_a, &col_a);
   read_matrix(f, matrix_b, &row_b, &col_b);

   print_matrix("A", matrix_a, row_a, col_a);
   print_matrix("B", matrix_b, row_b, col_b);

   size = row_b;/* gets size of row_b so that it can be used in cell_multiply */

   if (col_a != row_b) {
      fprintf(stderr, "Matrix multiplication not possible. Columns in A (%d) must equal rows in B (%d)\n", col_a, row_b);
      return 2;
   }

   int num_threads = row_a*col_b; /* gets number of threads needed */
   instruction inst[row_a*col_b];

   pthread_t tids[num_threads]; /* the thread identifier */
   pthread_attr_t attr; /* set of attributes for the thread */
   pthread_attr_init(&attr); /* get the default attributes */

   /* creating threads */
   for(int x = 0; x < row_a; x++){
      for(int y = 0; y < col_b; y++) {
         inst[num_threads].x = x;
         inst[num_threads].y = y;
         pthread_create(&tids[num_threads], &attr, cell_multiply, (void *) &inst[num_threads]);/* create the thread*/
         num_threads++;
      }        
   }
    /* joining threads */
    for(int i = 0; i < (row_a*col_b); i++){
         pthread_join(tids[i], NULL);/* wait for threads to complete*/
    }
   print_matrix("A x B = C", matrix_c, row_a, col_b);
}
/* performs the multiplication */
void *cell_multiply(void *param) {
   instruction *inst = param;
   int sum = 0;

   for(int n = 0; n < size; n++){
       matrix_c[inst->x][inst->y] += matrix_a[inst->x][n] * matrix_b[n][inst->y];
   }
   pthread_exit(0);
}
/* print the matrices */
void print_matrix(char *title, int m[10][10], int row_cnt, int col_cnt) {
   printf("%s [%dx%d]:\n", title, row_cnt, col_cnt);
   int max = 0;
   for(int r = 0; r < row_cnt; r++) {
      for (int c = 0; c < col_cnt; c++) {
        max = max < m[r][c] ? m[r][c] : max; 
      };
   }

   int size = (int)(log10((double)max)+1);
   char format[10];
   sprintf(format, "%%%dd ", size);

   for(int r = 0; r < row_cnt; r++) {
      printf("   ");
      for (int c = 0; c < col_cnt; c++) {
         printf(format, m[r][c]);
      };
      printf("\n");
   }
   printf("\n");
}
/* read from .txt file */
void read_matrix(FILE *f, int m[10][10], int *row_cnt, int *col_cnt) {
   fscanf(f, "%d %d", row_cnt, col_cnt);
   for (int r = 0; r < *row_cnt; r++) {
      for (int c = 0; c < *col_cnt; c++) {
         fscanf(f, "%d", &(m[r][c]));
      }
   }
}
