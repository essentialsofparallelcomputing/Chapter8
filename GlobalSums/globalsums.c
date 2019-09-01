#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
#include "timer.h"

#define ORDERS_OF_MAGNITUDE 1.0e9;

double *init_energy(long ncells, int *nsize_out, double *accurate_sum);

struct esum_type{
   double sum;
   double correction;
};

MPI_Datatype MPI_TWO_DOUBLES;
MPI_Op KAHAN_SUM;

void kahan_sum( struct esum_type * in, struct esum_type * inout, int *len,
    MPI_Datatype *MPI_TWO_DOUBLES)
{
   double corrected_next_term, new_sum;
   corrected_next_term = in->sum + (in->correction + inout->correction);
   new_sum = inout->sum + corrected_next_term;
   inout->correction = corrected_next_term - (new_sum - inout->sum);
   inout->sum = new_sum;
}

void init_kahan_sum(void){
   MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_TWO_DOUBLES);
   MPI_Type_commit(&MPI_TWO_DOUBLES);

   int commutative = 1;
   MPI_Op_create((MPI_User_function *)kahan_sum, commutative, &KAHAN_SUM);
}

double global_kahan_sum(int nsize, double *local_energy){
   struct esum_type local, global;
   local.sum = 0.0;
   local.correction = 0.0;

   for (long i = 0; i < nsize; i++) {
      double corrected_next_term = local_energy[i] + local.correction;
      double new_sum             = local.sum + local.correction;
      local.correction   = corrected_next_term - (new_sum - local.sum);
      local.sum          = new_sum;
   }

   MPI_Allreduce(&local, &global, 1, MPI_TWO_DOUBLES, KAHAN_SUM, MPI_COMM_WORLD);

   return global.sum;
}

int main(int argc, char *argv[])
{
   MPI_Init(&argc, &argv);
   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   init_kahan_sum();

   if (rank == 0) printf("MPI Kahan tests\n");

   for (int pow_of_two = 8; pow_of_two < 31 ; pow_of_two++){
      long ncells = (long)pow((double)2,(double)pow_of_two);

      int nsize;
      double accurate_sum;
      double *local_energy = init_energy(ncells, &nsize, &accurate_sum);

      struct timespec cpu_timer;
      cpu_timer_start(&cpu_timer);

      double test_sum = global_kahan_sum(nsize, local_energy);

      double cpu_time = cpu_timer_stop(cpu_timer);
   
      if (rank == 0){
         double sum_diff = test_sum-accurate_sum;
         printf("  ncells %ld log %d accurate sum %-17.16lg sum %-17.16lg ",
                ncells,(int)log2((double)ncells),accurate_sum,test_sum);
         printf("diff %10.4lg relative diff %10.4lg runtime %lf\n",
                sum_diff,sum_diff/accurate_sum, cpu_time);
      }

      free(local_energy);
   }

   MPI_Type_free(&MPI_TWO_DOUBLES);
   MPI_Op_free(&KAHAN_SUM);
   MPI_Finalize();
   return 0;
}

double *init_energy(long ncells, int *nsize_out, double *accurate_sum){
   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   long ncellsdiv2 = ncells/2;

   long ibegin = ncells *(rank  )/nprocs;
   long iend   = ncells *(rank+1)/nprocs;
   long nsize  = iend-ibegin;

   double high_value = 1.0e-1;
   double low_value  = 1.0e-1/ORDERS_OF_MAGNITUDE;
   *accurate_sum = (double)ncellsdiv2 * high_value +
                   (double)ncellsdiv2 * low_value;

   double *local_energy = (double *)malloc(nsize*sizeof(double));

   int sizes[nprocs];
   MPI_Allgather(&nsize, 1, MPI_INT, sizes, 1, MPI_INT, MPI_COMM_WORLD);

   long offsets[nprocs];
   offsets[0] = 0;
   for (int i = 1; i<nprocs; i++){
      offsets[i] = offsets[i-1] + (long)sizes[i-1];
   }

   // Initialize with high values first
   for (long i = 0; i < nsize; i++){
      local_energy[i] = (i + offsets[rank] < ncellsdiv2) ? high_value : low_value;
   }

   *nsize_out = nsize;

   return(local_energy);
}

