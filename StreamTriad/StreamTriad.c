#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "timer.h"

#define NTIMES 16
// large enough to force into main memory
#define STREAM_ARRAY_SIZE 80000000

int main(int argc, char *argv[]){

   MPI_Init(&argc, &argv);

   int nprocs, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   int ibegin = STREAM_ARRAY_SIZE *(rank  )/nprocs;
   int iend   = STREAM_ARRAY_SIZE *(rank+1)/nprocs;
   int nsize = iend-ibegin;
   double *a = malloc(nsize * sizeof(double));
   double *b = malloc(nsize * sizeof(double));
   double *c = malloc(nsize * sizeof(double));

   struct timespec tstart;
   // initializing data and arrays
   double scalar = 3.0, time_sum = 0.0;
   for (int i=0; i<nsize; i++) {
      a[i] = 1.0;
      b[i] = 2.0;
   }

   for (int k=0; k<NTIMES; k++){
      cpu_timer_start(&tstart);
      // stream triad loop
      for (int i=0; i<nsize; i++){
         c[i] = a[i] + scalar*b[i];
      }
      time_sum += cpu_timer_stop(tstart);
      // to keep the compiler from optimizing out the loop
      c[1]=c[2];
   }

   free(a);
   free(b);
   free(c);

   if (rank == 0) printf("Average runtime is %lf msecs\n", time_sum/NTIMES);
   MPI_Finalize();
   return(0);
}

