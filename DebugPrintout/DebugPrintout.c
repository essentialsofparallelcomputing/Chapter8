#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include "timer.h"
int main(int argc, char *argv[])
{
   int rank, nprocs;
   double total_time;
   struct timespec tstart_time;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   cpu_timer_start(&tstart_time);
   sleep(30); // represents work being done
   total_time += cpu_timer_stop(tstart_time);

   double times[nprocs]; 
   MPI_Gather(&total_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   if (rank == 0) {
      for (int i=0; i<nprocs; i++){
         printf("%d:Time for work on rank %d is %lf seconds\n", i, i, times[i]);
      }
   }

   MPI_Finalize();
   return 0;
}
