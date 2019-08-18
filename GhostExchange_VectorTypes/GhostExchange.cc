#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

#include "malloc2D.h"
#include "timer.h"

#define SWAP_PTR(xnew,xold,xtmp) (xtmp=xnew, xnew=xold, xold=xtmp)
void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo, int &corners);
void Cartesian_print(double **x, int jmax, int imax, int nhalo, int nprocy, int nprocx);
void boundarycondition_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop);
void ghostcell_update(double **x, int nhalo, int corners, int jsize, int isize,
      int nleft, int nrght, int nbot, int ntop, MPI_Datatype cart_col, MPI_Datatype cart_row);
void haloupdate_test(int nhalo, int corners, int jsize, int isize, int nleft, int nrght, int nbot, int ntop,
      int jmax, int imax, int nprocy, int nprocx, MPI_Datatype cart_col, MPI_Datatype cart_row);

int main(int argc, char *argv[])
{
   MPI_Init(&argc, &argv);

   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   if (rank == 0) printf("Parallel run with no threads\n");

   int imax = 2000, jmax = 2000;
   int nprocx = 0, nprocy = 0;
   int nhalo = 2, corners = 0;

   parse_input_args(argc, argv, jmax, imax, nprocy, nprocx, nhalo, corners);
 
   struct timespec tstart_init, tstart_stencil, tstart_boundarycondition, tstart_ghostcell, tstart_total;
   double init_time, stencil_time=0.0, boundarycondition_time=0.0, ghostcell_time=0.0, total_time;
   cpu_timer_start(&tstart_total);
   cpu_timer_start(&tstart_init);

   int xcoord = rank%nprocx;
   int ycoord = rank/nprocx;

   int nleft = (xcoord > 0       ) ? rank - 1      : MPI_PROC_NULL;
   int nrght = (xcoord < nprocx-1) ? rank + 1      : MPI_PROC_NULL;
   int nbot  = (ycoord > 0       ) ? rank - nprocx : MPI_PROC_NULL;
   int ntop  = (ycoord < nprocy-1) ? rank + nprocx : MPI_PROC_NULL;

   int ibegin = imax *(xcoord  )/nprocx;
   int iend   = imax *(xcoord+1)/nprocx;
   int isize  = iend - ibegin;
   int jbegin = jmax *(ycoord  )/nprocy;
   int jend   = jmax *(ycoord+1)/nprocy;
   int jsize  = jend - jbegin;

   MPI_Datatype cart_col;
   MPI_Type_vector(jsize, nhalo, isize+2*nhalo, MPI_DOUBLE, &cart_col);
   MPI_Type_commit(&cart_col);

   MPI_Datatype cart_row;
   if (! corners){
      MPI_Type_vector(nhalo, isize, isize+2*nhalo, MPI_DOUBLE, &cart_row);
      MPI_Type_commit(&cart_row);
   }

   /* The halo update both updates the ghost cells and the boundary halo cells. To be precise with terminology,
    * the ghost cells only exist for multi-processor runs with MPI. The boundary halo cells are to set boundary
    * conditions. Halos refer to both the ghost cells and the boundary halo cells.
    */
   haloupdate_test(nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, jmax, imax, nprocy, nprocx, cart_col, cart_row);
      
   double** xtmp;
   // This offsets the array addressing so that the real part of the array is from 0,0 to jsize,isize
   double** x    = malloc2D(jsize+2*nhalo, isize+2*nhalo, nhalo, nhalo);
   double** xnew = malloc2D(jsize+2*nhalo, isize+2*nhalo, nhalo, nhalo);

   for (int j = 0; j < jsize; j++){
      for (int i = 0; i < isize; i++){
         x[j][i] = 5.0;
      }
   }

   for (int j = jmax/2 - 5; j < jmax/2 + 5; j++){
      for (int i = imax/2 - 5; i < imax/2 + 5; i++){
         if (j >= jbegin && j < jend && i >= ibegin && i < iend) {
            x[j-jbegin][i-ibegin] = 400.0;
         }
      }
   }

   boundarycondition_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);
   ghostcell_update(x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, cart_col, cart_row);

   init_time = cpu_timer_stop(tstart_init);

   for (int iter = 0; iter < 1000; iter++){
      cpu_timer_start(&tstart_stencil);

      for (int j = 0; j < jsize; j++){
         for (int i = 0; i < isize; i++){
            xnew[j][i] = ( x[j][i] + x[j][i-1] + x[j][i+1] + x[j-1][i] + x[j+1][i] )/5.0;
         }
      }

      SWAP_PTR(xnew, x, xtmp);

      stencil_time += cpu_timer_stop(tstart_stencil);

      cpu_timer_start(&tstart_boundarycondition);

      boundarycondition_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);

      boundarycondition_time += cpu_timer_stop(tstart_boundarycondition);
      cpu_timer_start(&tstart_ghostcell);

      ghostcell_update(x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, cart_col, cart_row);

      ghostcell_time += cpu_timer_stop(tstart_ghostcell);

      if (iter%100 == 0 && rank == 0) printf("Iter %d\n",iter);
   }
   total_time = cpu_timer_stop(tstart_total);

   Cartesian_print(x, jmax, imax, nhalo, nprocy, nprocx);

   if (rank == 0){
      printf("Timing is init %f stencil %f boundary condition %f ghost cell %lf total %f\n",
             init_time,stencil_time,boundarycondition_time,ghostcell_time,total_time);
   }

   MPI_Type_free(&cart_col);
   if (! corners){
      MPI_Type_free(&cart_row);
   }
   MPI_Finalize();
   exit(0);
}

void boundarycondition_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop)
{
// Boundary conditions -- constant
   if (nleft == MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         for (int k=-nhalo; k<0; k++){
            x[j][k] = x[j][0];
         }
      }
   }

   if (nrght == MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         for (int k=0; k<nhalo; k++){
            x[j][isize+k] = x[j][isize-1];
         }
      }
   }

   if (nbot == MPI_PROC_NULL){
      for (int i = -nhalo; i < isize+nhalo; i++){
         for (int k=-nhalo; k<0; k++){
            x[k][i] = x[0][i];
         }
      }
   }
      
   if (ntop == MPI_PROC_NULL){
      for (int i = -nhalo; i < isize+nhalo; i++){
         for (int k=0; k<nhalo; k++){
            x[jsize+k][i] = x[jsize-1][i];
         }
      }
   }
}

void ghostcell_update(double **x, int nhalo, int corners, int jsize, int isize,
      int nleft, int nrght, int nbot, int ntop, MPI_Datatype cart_col, MPI_Datatype cart_row)
{
   MPI_Request request[8];
   MPI_Status status[8];

   MPI_Irecv(&x[0][isize], 1,  cart_col, nrght, 1001, MPI_COMM_WORLD, &request[0]);
   MPI_Isend(&x[0][0],     1,  cart_col, nleft, 1001, MPI_COMM_WORLD, &request[1]);

   MPI_Irecv(&x[0][-nhalo],      1, cart_col, nleft, 1002, MPI_COMM_WORLD, &request[2]);
   MPI_Isend(&x[0][isize-nhalo], 1, cart_col, nrght, 1002, MPI_COMM_WORLD, &request[3]);

   int waitcount;

   if (corners) {
      MPI_Waitall(4, request, status);

      MPI_Barrier(MPI_COMM_WORLD);

      MPI_Irecv(&x[jsize][-nhalo],   nhalo*(isize+2*nhalo), MPI_DOUBLE, ntop, 1001, MPI_COMM_WORLD, &request[0]);
      MPI_Isend(&x[0    ][-nhalo],   nhalo*(isize+2*nhalo), MPI_DOUBLE, nbot, 1001, MPI_COMM_WORLD, &request[1]);

      MPI_Irecv(&x[     -nhalo][-nhalo], nhalo*(isize+2*nhalo), MPI_DOUBLE, nbot, 1002, MPI_COMM_WORLD, &request[2]);
      MPI_Isend(&x[jsize-nhalo][-nhalo], nhalo*(isize+2*nhalo), MPI_DOUBLE, ntop, 1002, MPI_COMM_WORLD, &request[3]);
      waitcount = 4;
   } else {
      MPI_Irecv(&x[jsize][0],   1, cart_row, ntop, 1001, MPI_COMM_WORLD, &request[4]);
      MPI_Isend(&x[0    ][0],   1, cart_row, nbot, 1001, MPI_COMM_WORLD, &request[5]);

      MPI_Irecv(&x[     -nhalo][0], 1, cart_row, nbot, 1002, MPI_COMM_WORLD, &request[6]);
      MPI_Isend(&x[jsize-nhalo][0], 1, cart_row, ntop, 1002, MPI_COMM_WORLD, &request[7]);
      waitcount = 8;
   }

   MPI_Waitall(waitcount, request, status);
}

void haloupdate_test(int nhalo, int corners, int jsize, int isize, int nleft, int nrght, int nbot, int ntop,
      int jmax, int imax, int nprocy, int nprocx, MPI_Datatype cart_col, MPI_Datatype cart_row)
{
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   double** x = malloc2D(jsize+2*nhalo, isize+2*nhalo, nhalo, nhalo);

   if (jsize > 12 || isize > 12) return;

   for (int j = -nhalo; j < jsize+nhalo; j++){
      for (int i = -nhalo; i < isize+nhalo; i++){
         x[j][i] = 0.0;
      }
   }

   for (int j = 0; j < jsize; j++){
      for (int i = 0; i < isize; i++){
         x[j][i] = rank * 1000 + j*10 + i;
      }
   }

   boundarycondition_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);
   ghostcell_update(x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, cart_col, cart_row);

   Cartesian_print(x, jmax, imax, nhalo, nprocy, nprocx);

   malloc2D_free(x, nhalo);
}

void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo, int &corners)
{
   int c;
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   while ((c = getopt(argc, argv, "ch:i:j:x:y:")) != -1){
      switch(c){
         case 'c':
            corners = 1;
            break;
         case 'h':
            nhalo = atoi(optarg);
            break;
         case 'i':
            imax = atoi(optarg);
            break;
         case 'j':
            jmax = atoi(optarg);
            break;
         case 'x':
            nprocx = atoi(optarg);
            break;
         case 'y':
            nprocy = atoi(optarg);
            break;
         case '?':
            if (optopt == 'h' || optopt == 'i' || optopt == 'j' || optopt == 'x' || optopt == 'y'){
               if (rank == 0) fprintf (stderr, "Option -%c requires an argument.\n", optopt);
               MPI_Finalize();
               exit(1);
            }
            break;
         default:
            if (rank == 0) fprintf(stderr,"Unknown option %c\n",c);
            MPI_Finalize();
            exit(1);
      }
   }

   int xcoord = rank%nprocx;
   int ycoord = rank/nprocx;

   int ibegin = imax *(xcoord  )/nprocx;
   int iend   = imax *(xcoord+1)/nprocx;
   int isize  = iend - ibegin;
   int jbegin = jmax *(ycoord  )/nprocy;
   int jend   = jmax *(ycoord+1)/nprocy;
   int jsize  = jend - jbegin;

   int ierr = 0, ierr_global;
   if (isize < nhalo || jsize < nhalo) {
      if (rank == 0) printf("Error -- local size of grid is less than the halo size\n");
      ierr = 1;
   }
   MPI_Allreduce(&ierr, &ierr_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
   if (ierr_global > 0) {
      MPI_Finalize();
      exit(0);
   }
}

void Cartesian_print(double **x, int jmax, int imax, int nhalo, int nprocy, int nprocx)
{
   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   int isize_total=0;
   int isizes[nprocx];
   for (int ii = 0; ii < nprocx; ii++){
      int xcoord = ii%nprocx;
      int ycoord = ii/nprocx;
      int ibegin = imax *(xcoord  )/nprocx;
      int iend   = imax *(xcoord+1)/nprocx;
      isizes[ii] = iend-ibegin;
      isize_total += isizes[ii] + 2*nhalo;
   }

   if (isize_total > 40) return;

   if (rank == 0) {
      printf("     ");
      for (int ii = 0; ii < nprocx; ii++){
         for (int i = -nhalo; i < isizes[ii]+nhalo; i++){
            printf("%6d   ",i);
         }
         printf("   ");
      }
      printf("\n");
   }

   int xcoord = rank%nprocx;
   int ycoord = rank/nprocx;

   int ibegin = imax *(xcoord  )/nprocx;
   int iend   = imax *(xcoord+1)/nprocx;
   int isize = iend-ibegin;
   int jbegin = jmax *(ycoord  )/nprocy;
   int jend   = jmax *(ycoord+1)/nprocy;
   int jsize = jend-jbegin;

   double *xrow = (double *)malloc(isize_total*sizeof(double));
   for (int jj=nprocy-1; jj >= 0; jj--){
      int ilen = 0;
      int jlen = 0;
      int jlen_max;
      int *ilen_global = (int *)malloc(nprocs*sizeof(int));
      int *ilen_displ = (int *)malloc(nprocs*sizeof(int));
      if (ycoord == jj) {
         ilen = isize + 2*nhalo;
         jlen = jsize;
      }
      MPI_Allgather(&ilen,1,MPI_INT,ilen_global,1,MPI_INT,MPI_COMM_WORLD);
      MPI_Allreduce(&jlen,&jlen_max,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      ilen_displ[0] = 0;
      for (int i=1; i<nprocs; i++){
         ilen_displ[i] = ilen_displ[i-1] + ilen_global[i-1];
      }
      for (int j=jlen_max+nhalo-1; j>=-nhalo; j--){
         MPI_Gatherv(&x[j][-nhalo],ilen,MPI_DOUBLE,xrow,ilen_global,ilen_displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
         if (rank == 0) {
            printf("%3d:",j);
            for (int ii = 0; ii < nprocx; ii++){
               for (int i = 0; i< isizes[ii]+2*nhalo; i++){
                  printf("%8.1lf ",xrow[i+ii*(isizes[ii]+2*nhalo)]);
               }
               printf("   ");
            }
            printf("\n");
         }
      }
      free(ilen_global);
      free(ilen_displ);
      if (rank == 0) printf("\n");
   }
   free(xrow);
}
