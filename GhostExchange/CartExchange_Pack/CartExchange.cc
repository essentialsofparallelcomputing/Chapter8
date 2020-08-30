#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

#include "malloc2D.h"
#include "timer.h"

#define SWAP_PTR(xnew,xold,xtmp) (xtmp=xnew, xnew=xold, xold=xtmp)
void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo, int &corners, int &do_timing);
void Cartesian_print(MPI_Comm cart_comm, double **x, int jmax, int imax, int nhalo);
void boundarycondition_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop);
void ghostcell_update(MPI_Comm cart_comm, double **x, int nhalo, int corners, int jsize, int isize,
      int nleft, int nrght, int nbot, int ntop, int do_timing);
void haloupdate_test(MPI_Comm cart_comm, int nhalo, int corners, int jsize, int isize, int nleft, int nrght, int nbot, int ntop,
      int jmax, int imax, int nprocy, int nprocx, int do_timing);

double boundarycondition_time=0.0, ghostcell_time=0.0;

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
   int do_timing = 0;

   parse_input_args(argc, argv, jmax, imax, nprocy, nprocx, nhalo, corners, do_timing);
 
   struct timespec tstart_stencil, tstart_total;
   double stencil_time=0.0, total_time;
   cpu_timer_start(&tstart_total);

   int dims[2] = {nprocy, nprocx}; // needs to be initialized
   int periods[2]={0,0};
   int coords[2];
   MPI_Dims_create(nprocs, 2, dims);
   MPI_Comm cart_comm;
   MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
   MPI_Cart_coords(cart_comm, rank, 2, coords);

   int nleft, nrght, nbot, ntop;
   MPI_Cart_shift(cart_comm, 1, 1, &nleft, &nrght);
   MPI_Cart_shift(cart_comm, 0, 1, &nbot,  &ntop);

   int ibegin = imax *(coords[1]  )/dims[1];
   int iend   = imax *(coords[1]+1)/dims[1];
   int isize  = iend - ibegin;
   int jbegin = jmax *(coords[0]  )/dims[0];
   int jend   = jmax *(coords[0]+1)/dims[0];
   int jsize  = jend - jbegin;

   /* The halo update both updates the ghost cells and the boundary halo cells. To be precise with terminology,
    * the ghost cells only exist for multi-processor runs with MPI. The boundary halo cells are to set boundary
    * conditions. Halos refer to both the ghost cells and the boundary halo cells.
    */
   haloupdate_test(cart_comm, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, jmax, imax, nprocy, nprocx, do_timing);

   double** xtmp;
   // This offsets the array addressing so that the real part of the array is from 0,0 to jsize,isize
   double** x    = malloc2D(jsize+2*nhalo, isize+2*nhalo, nhalo, nhalo);
   double** xnew = malloc2D(jsize+2*nhalo, isize+2*nhalo, nhalo, nhalo);

   if (! corners) { // MPI_Ineighbor_alltoallv does not handle corners easily
      if (rank == 0) printf("Error -- MPI_Ineighbor_alltoallv\n");
      MPI_Finalize();
      exit(0);
   }

   for (int j = 0; j < jsize; j++){
      for (int i = 0; i < isize; i++){
         x[j][i] = 5.0;
      }
   }

   int ispan=5, jspan=5;
   if (ispan > imax/2) ispan = imax/2;
   if (jspan > jmax/2) jspan = jmax/2;
   for (int j = jmax/2 - jspan; j < jmax/2 + jspan; j++){
      for (int i = imax/2 - ispan; i < imax/2 + ispan; i++){
         if (j >= jbegin && j < jend && i >= ibegin && i < iend) {
            x[j-jbegin][i-ibegin] = 400.0;
         }
      }
   }

   boundarycondition_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);
   ghostcell_update(cart_comm, x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, do_timing);

   for (int iter = 0; iter < 1000; iter++){
      cpu_timer_start(&tstart_stencil);

      for (int j = 0; j < jsize; j++){
         for (int i = 0; i < isize; i++){
            xnew[j][i] = ( x[j][i] + x[j][i-1] + x[j][i+1] + x[j-1][i] + x[j+1][i] )/5.0;
         }
      }

      SWAP_PTR(xnew, x, xtmp);

      stencil_time += cpu_timer_stop(tstart_stencil);

      boundarycondition_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);
      ghostcell_update(cart_comm, x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, do_timing);

      if (iter%100 == 0 && rank == 0) printf("Iter %d\n",iter);
   }
   total_time = cpu_timer_stop(tstart_total);

   Cartesian_print(cart_comm, x, jmax, imax, nhalo);

   if (rank == 0){
      printf("CartExchange_Pack Timing is stencil %f boundary condition %f ghost cell %lf total %f\n",
             stencil_time,boundarycondition_time,ghostcell_time,total_time);
   }

   free(x);
   free(xnew);

   MPI_Finalize();
   exit(0);
}

void boundarycondition_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop)
{
   struct timespec tstart_boundarycondition;
   cpu_timer_start(&tstart_boundarycondition);

// Boundary conditions -- constant
   if (nleft == MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         for (int ll=-nhalo; ll<0; ll++){
            x[j][ll] = x[j][0];
         }
      }
   }

   if (nrght == MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         for (int ll=0; ll<nhalo; ll++){
            x[j][isize+ll] = x[j][isize-1];
         }
      }
   }

   if (nbot == MPI_PROC_NULL){
      for (int ll=-nhalo; ll<0; ll++){
         for (int i = -nhalo; i < isize+nhalo; i++){
            x[ll][i] = x[0][i];
         }
      }
   }
      
   if (ntop == MPI_PROC_NULL){
      for (int ll=0; ll<nhalo; ll++){
         for (int i = -nhalo; i < isize+nhalo; i++){
            x[jsize+ll][i] = x[jsize-1][i];
         }
      }
   }

   boundarycondition_time += cpu_timer_stop(tstart_boundarycondition);
}

void ghostcell_update(MPI_Comm cart_comm, double **x, int nhalo, int corners, int jsize, int isize,
      int nleft, int nrght, int nbot, int ntop, int do_timing)
{
   if (do_timing) MPI_Barrier(MPI_COMM_WORLD);

   struct timespec tstart_ghostcell;
   cpu_timer_start(&tstart_ghostcell);

   MPI_Request request;
   MPI_Status status;

   int nhorizontal_count = nhalo*jsize;
   int nvertical_count   = nhalo*isize; // nhalo*(isize+2*nhalo);
   int buff_count = 2*nhorizontal_count + 2*nvertical_count;
   int buff_size = buff_count*sizeof(double);

   double *sbuf = (double *)malloc(buff_count*sizeof(double));
   double *rbuf = (double *)malloc(buff_count*sizeof(double));

   int counts[4] = {nvertical_count, nvertical_count, nhorizontal_count, nhorizontal_count};
   int displs[4] = {0, nvertical_count, 2*nvertical_count, 2*nvertical_count+nhorizontal_count};

   int position = 0;
   if (nbot != MPI_PROC_NULL){
      for (int k = 0; k < nhalo; k++){
         MPI_Pack(&x[k][0], isize, MPI_DOUBLE, sbuf, buff_count*sizeof(double), &position,  MPI_COMM_WORLD);
      }
   } else {
      position += isize*nhalo*sizeof(double);
   }

   if (ntop != MPI_PROC_NULL){
      for (int k = 0; k < nhalo; k++){
         MPI_Pack(&x[jsize-nhalo+k][0], isize, MPI_DOUBLE, sbuf, buff_count*sizeof(double), &position, MPI_COMM_WORLD);
      }
   } else {
      position += isize*nhalo*sizeof(double);
   }

   if (nleft != MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         MPI_Pack(&x[j][0], nhalo, MPI_DOUBLE, sbuf, buff_count*sizeof(double), &position,  MPI_COMM_WORLD);
      }
   } else {
      position += jsize*nhalo*sizeof(double);
   }

   if (nrght != MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         MPI_Pack(&x[j][isize-nhalo], nhalo, MPI_DOUBLE, sbuf, buff_count*sizeof(double), &position, MPI_COMM_WORLD);
      }
   } else {
      position += jsize*nhalo*sizeof(double);
   }

   MPI_Ineighbor_alltoallv(sbuf, counts, displs, MPI_DOUBLE, rbuf, counts, displs, MPI_DOUBLE, cart_comm, &request);
   MPI_Waitall(1, &request, &status);

   position = 0;
   if (nbot != MPI_PROC_NULL){
      for (int k = 0; k < nhalo; k++){
         MPI_Unpack(rbuf, buff_size, &position, &x[-nhalo+k][0], isize, MPI_DOUBLE, MPI_COMM_WORLD);
      }
   } else {
      position += isize*nhalo*sizeof(double);
   }

   if (ntop != MPI_PROC_NULL){
      for (int k = 0; k < nhalo; k++){
         MPI_Unpack(rbuf, buff_size, &position, &x[jsize+k][0], isize, MPI_DOUBLE, MPI_COMM_WORLD);
      }
   } else {
      position += isize*nhalo*sizeof(double);
   }

   if (nleft != MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         MPI_Unpack(rbuf, buff_size, &position, &x[j][-nhalo], nhalo, MPI_DOUBLE, MPI_COMM_WORLD);
      }
   } else {
      position += jsize*nhalo*sizeof(double);
   }

   if (nrght != MPI_PROC_NULL){
      for (int j = 0; j < jsize; j++){
         MPI_Unpack(rbuf, buff_size, &position, &x[j][isize], nhalo, MPI_DOUBLE, MPI_COMM_WORLD);
      }
   } else {
      position += jsize*nhalo*sizeof(double);
   }

   if (do_timing) MPI_Barrier(MPI_COMM_WORLD);

   ghostcell_time += cpu_timer_stop(tstart_ghostcell);
}

void haloupdate_test(MPI_Comm cart_comm, int nhalo, int corners, int jsize, int isize, int nleft, int nrght, int nbot, int ntop,
      int jmax, int imax, int nprocy, int nprocx, int do_timing)
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
   ghostcell_update(cart_comm, x, nhalo, corners, jsize, isize, nleft, nrght, nbot, ntop, do_timing);

   Cartesian_print(cart_comm, x, jmax, imax, nhalo);

   malloc2D_free(x, nhalo);
}

void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo, int &corners, int &do_timing)
{
   int c;
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   while ((c = getopt(argc, argv, "ch:i:j:tx:y:")) != -1){
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
         case 't':
            do_timing = 1;
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

   if (nprocx != 0 && nprocy != 0) {
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
}

void Cartesian_print(MPI_Comm cart_comm, double **x, int jmax, int imax, int nhalo)
{
   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   int dims[2];
   int periods[2];
   int coords[2];
   MPI_Cart_get(cart_comm, 2, dims, periods, coords);

   int isize_total=0;
   int *isizes = (int *)malloc(dims[1]*sizeof(int));
   for (int ii = 0; ii < dims[1]; ii++){
      int coords[2];
      MPI_Cart_coords(cart_comm, ii, 2, coords);
      int ibegin = imax *(coords[1]  )/dims[1];
      int iend   = imax *(coords[1]+1)/dims[1];
      isizes[ii] = iend-ibegin;
      isize_total += isizes[ii] + 2*nhalo;
   }

   if (isize_total > 40) return;

   if (rank == 0) {
      printf("     ");
      for (int ii = 0; ii < dims[1]; ii++){
         for (int i = -nhalo; i < isizes[ii]+nhalo; i++){
            printf("%6d   ",i);
         }
         printf("   ");
      }
      printf("\n");
   }

   int ibegin = imax *(coords[1]  )/dims[1];
   int iend   = imax *(coords[1]+1)/dims[1];
   int isize  = iend - ibegin;
   int jbegin = jmax *(coords[0]  )/dims[0];
   int jend   = jmax *(coords[0]+1)/dims[0];
   int jsize  = jend - jbegin;

   double *xrow = (double *)malloc(isize_total*sizeof(double));
   for (int jj=dims[0]-1; jj >= 0; jj--){
      int ilen = 0;
      int jlen = 0;
      int jlen_max;
      int *ilen_global = (int *)malloc(nprocs*sizeof(int));
      int *ilen_displ = (int *)malloc(nprocs*sizeof(int));
      int coords[2];
      MPI_Cart_coords(cart_comm, rank, 2, coords);
      if (coords[0] == jj) {
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
            for (int ii = 0; ii < dims[1]; ii++){
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
