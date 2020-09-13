#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char *argv[])
{
   int rank, input_size;
   char *input_string, *line;
   FILE *fin;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (rank == 0){
      fin = fopen("../file.in", "r");
      fseek(fin, 0, SEEK_END);
      input_size = ftell(fin);
      fseek(fin, 0, SEEK_SET);
      input_string = (char *)malloc((input_size+1)*sizeof(char));
      fread(input_string, 1, input_size, fin);
      input_string[input_size] = '\0';
   }

   MPI_Bcast(&input_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
   if (rank != 0) input_string = (char *)malloc((input_size+1)*sizeof(char));
   MPI_Bcast(input_string, input_size, MPI_CHAR, 0, MPI_COMM_WORLD);

   if (rank == 0) fclose(fin);

   line = strtok(input_string,"\n");
   while (line != NULL){
      printf("%d:input string is %s\n",rank,line);
      line = strtok(NULL,"\n");
   }
   free(input_string);

   MPI_Finalize();
   return 0;
}
