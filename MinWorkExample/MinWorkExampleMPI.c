#include <mpi.h>
#include <stdio.h>
int main(int argc, char **argv)
{
   MPI_Init(&argc, &argv);

   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   printf("Rank %d of %d\n", rank, nprocs);

   MPI_Finalize();
   return 0;
}
