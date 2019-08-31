#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
   double start_time, main_time, min_time, max_time, avg_time;

   MPI_Init(&argc, &argv);
   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   // barrier synchronizes all the processes so they start at about the same time
   MPI_Barrier(MPI_COMM_WORLD);
   start_time = MPI_Wtime();

   sleep(30); // represents work being done

   // get the timer value and subtract off the starting value to get the elapsed time
   main_time = MPI_Wtime() - start_time;
   // Use reduction calls to compute the max, min, and average time
   MPI_Reduce(&main_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
   MPI_Reduce(&main_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0,MPI_COMM_WORLD);
   MPI_Reduce(&main_time, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);
   if (rank == 0) printf("Time for work is Min: %lf  Max: %lf  Avg:  %lf seconds\n",
                         min_time, max_time, avg_time/nprocs);

   MPI_Finalize();
   return 0;
}
