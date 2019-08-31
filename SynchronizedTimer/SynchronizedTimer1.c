#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
   double start_time, main_time;

   MPI_Init(&argc, &argv);
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // barrier synchronizes all the processes so they start at about the same time
   MPI_Barrier(MPI_COMM_WORLD);
   // get the starting value of the timer using the MPI_Wtime routine
   start_time = MPI_Wtime();

   sleep(30); // represents work being done

   // synchronize the processes, which has the effect that we get the longest time taken
   MPI_Barrier(MPI_COMM_WORLD);
   // get the timer value and subtract off the starting value to get the elapsed time
   main_time = MPI_Wtime() - start_time;
   if (rank == 0) printf("Time for work is %lf seconds\n", main_time);

   MPI_Finalize();
   return 0;
}
