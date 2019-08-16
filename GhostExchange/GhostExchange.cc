#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

#include "malloc2D.h"
#include "timer.h"

#define SWAP_PTR(xnew,xold,xtmp) (xtmp=xnew, xnew=xold, xold=xtmp)
void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo);
void Cartesian_print(double **x, int jmax, int imax, int nhalo, int nprocy, int nprocx);
void halo_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop);
void haloupdate_test(int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop, int jmax, int imax, int nprocy, int nprocx);

int main(int argc, char *argv[])
{
   MPI_Init(&argc, &argv);

   int rank, nprocs;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   if (rank == 0) printf("Parallel run with no threads\n");

   int imax = 2000, jmax = 2000;
   int nprocx = 0, nprocy = 0;
   int nhalo = 2;

   parse_input_args(argc, argv, jmax, imax, nprocy, nprocx, nhalo);
 
   struct timespec tstart_init, tstart_stencil, tstart_haloupdate, tstart_total;
   double init_time, stencil_time=0.0, haloupdate_time=0.0, total_time;
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

   haloupdate_test(nhalo, jsize, isize, nleft, nrght, nbot, ntop, jmax, imax, nprocy, nprocx);
      
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

   halo_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);

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

      cpu_timer_start(&tstart_haloupdate);

      halo_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);

      haloupdate_time += cpu_timer_stop(tstart_haloupdate);

      if (iter%100 == 0 && rank == 0) printf("Iter %d\n",iter);
   }
   total_time = cpu_timer_stop(tstart_total);

   Cartesian_print(x, jmax, imax, nhalo, nprocy, nprocx);

   if (rank == 0){
      printf("Timing is init %f stencil %f haloupdate %f total %f\n",
             init_time,stencil_time,haloupdate_time,total_time);
   }

   MPI_Finalize();
   exit(0);
}

void halo_update(double **x, int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop)
{
   /* The halo update both updates the ghost cells and the boundary halo cells. To be precise with terminology,
    * the ghost cells only exist for multi-processor runs with MPI. The boundary halo cells are to set boundary
    * conditions. Halos refer to both the ghost cells and the boundary halo cells.
    */
   MPI_Request request[4];
   MPI_Status status[4];
   double xbuf_left_send[nhalo*jsize];
   double xbuf_right_send[nhalo*jsize];
   double xbuf_right_recv[nhalo*jsize];
   double xbuf_left_recv[nhalo*jsize];
   int position_left = 0;
   int position_right = 0;
   for (int j = 0; j < jsize; j++){
      MPI_Pack(&x[j][0],           nhalo, MPI_DOUBLE, xbuf_left_send,  jsize*nhalo*sizeof(double), &position_left,  MPI_COMM_WORLD);
      MPI_Pack(&x[j][isize-nhalo], nhalo, MPI_DOUBLE, xbuf_right_send, jsize*nhalo*sizeof(double), &position_right, MPI_COMM_WORLD);
   }

   int rank_left_source, rank_right_source, rank_right_dest, rank_left_dest;
   int rank_bottom_source, rank_top_source, rank_bottom_dest, rank_top_dest;

   MPI_Irecv(&xbuf_right_recv, position_left,  MPI_PACKED, nrght,  1001, MPI_COMM_WORLD, &request[0]);
   MPI_Isend(&xbuf_left_send,  position_left,  MPI_PACKED, nleft, 1001, MPI_COMM_WORLD, &request[1]);

   MPI_Irecv(&xbuf_left_recv,  position_right, MPI_PACKED, nleft, 1002, MPI_COMM_WORLD, &request[2]);
   MPI_Isend(&xbuf_right_send, position_right, MPI_PACKED, nrght, 1002, MPI_COMM_WORLD, &request[3]);
   MPI_Waitall(4, request, status);

   position_left = 0;
   position_right = 0;
   for (int j = 0; j < jsize; j++){
      MPI_Unpack(xbuf_right_recv, jsize*nhalo*sizeof(double), &position_right,  &x[j][isize],  nhalo, MPI_DOUBLE, MPI_COMM_WORLD);
      MPI_Unpack(xbuf_left_recv,  jsize*nhalo*sizeof(double), &position_left,   &x[j][-nhalo], nhalo, MPI_DOUBLE, MPI_COMM_WORLD);
   }

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

   MPI_Barrier(MPI_COMM_WORLD);

   MPI_Irecv(&x[jsize][-nhalo],   nhalo*(isize+2*nhalo), MPI_DOUBLE, ntop, 1001, MPI_COMM_WORLD, &request[0]);
   MPI_Isend(&x[0    ][-nhalo],   nhalo*(isize+2*nhalo), MPI_DOUBLE, nbot, 1001, MPI_COMM_WORLD, &request[1]);

   MPI_Irecv(&x[     -nhalo][-nhalo], nhalo*(isize+2*nhalo), MPI_DOUBLE, nbot, 1002, MPI_COMM_WORLD, &request[2]);
   MPI_Isend(&x[jsize-nhalo][-nhalo], nhalo*(isize+2*nhalo), MPI_DOUBLE, ntop, 1002, MPI_COMM_WORLD, &request[3]);
   MPI_Waitall(4, request, status);

// Boundary conditions -- constant
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

void haloupdate_test(int nhalo, int jsize, int isize, int nleft, int nrght, int nbot, int ntop, int jmax, int imax, int nprocy, int nprocx)
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

   halo_update(x, nhalo, jsize, isize, nleft, nrght, nbot, ntop);

   Cartesian_print(x, jmax, imax, nhalo, nprocy, nprocx);

   malloc2D_free(x, nhalo);
}

void parse_input_args(int argc, char **argv, int &jmax, int &imax, int &nprocy, int &nprocx, int &nhalo)
{
   int c;
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   while ((c = getopt(argc, argv, "h:i:j:x:y:")) != -1){
      switch(c){
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

   int dims[2]={nprocs/nprocx,nprocx};

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
