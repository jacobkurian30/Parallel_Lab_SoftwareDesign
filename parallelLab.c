//Jacob Kurian
// 05/02/2017
//Parallel Lab

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define min(x, y) ((x)<(y)?(x):(y))



int main(int argc, char* argv[])
{
  int nrows, ncols;
  double *aa;

  double *b;

  double *c;
  double *buffer, ans;
  double *times;
  double total_times;
  int run_index;
  int nruns;
  int myid, master, numprocs;
  double starttime, endtime;
  MPI_Status status;
  int rownum, row;
  int i, j, numsent, sender;
  srand(time(0));
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);


  if (argc > 1 ) {

        if(numprocs <= atoi(argv[1])) {

    nrows = atoi(argv[1]);
    ncols = nrows;
  aa = (double*)malloc(sizeof(double) * nrows * ncols);


    b = (double*)malloc(sizeof(double) * ncols*nrows);

    c = (double*)malloc(sizeof(double) * nrows*ncols);
        //      a*b = c
    buffer = (double*)malloc(sizeof(double) * ncols);
    master = 0;
    int i, n;
    time_t t;
    n = atoi(argv[1]);


   /* Intializes random number generator */
   srand((unsigned) time(&t));

        // creating randon numbers for matrix
    if (myid == master) {
        printf(" *************Generated Matrix*****************\n");
        printf("*********************************************\n");
        for (i = 0; i < nrows; i++) {
            for (j = 0; j < ncols; j++) {
              n = rand() % 25;
              aa[i*ncols + j] = n;
              printf("  %.2f" ,  aa[i*ncols + j] );
              b[i*ncols + j] = aa[i*nrows + j];

            }
        printf("\n");
        }
        printf("******************************************\n\n");

        starttime = MPI_Wtime();
        numsent = 0;
  MPI_Bcast(b, ncols*nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);

        for (i = 0; i < min(numprocs-1, nrows*ncols); i++) {
            for (j = 0; j < ncols; j++) {
                buffer[j] = aa[i/ncols + j];
            }
                //send to a slave (i+1)
            MPI_Send(buffer, ncols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
            numsent++;
        }

        for (i = 0; i < nrows*ncols; i++) {
                // getting back entry
            MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            sender = status.MPI_SOURCE; // getting source
            rownum = status.MPI_TAG; // what entry

            c[rownum-1] = ans;


        if (numsent < nrows*ncols) {
          for (j = 0; j < ncols; j++) {

            buffer[j] = aa[((numsent/nrows))*nrows + j];
          }
          MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD); // send
          numsent++;
        }

        else {

            MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
            }

        }
  endtime = MPI_Wtime();
        int matrixLength = atoi(argv[1]);
        //printing the result  to the screen
        printf("\n**********************************************************\n");
        printf("Total Time : %f seconds :",(endtime - starttime));
        printf("\n*************************************************************\n");



        printf("\n*******************Product of the Matrix*************************\n");
        printf("--------------------------------------------------------------------\n");
         for(int i=0; i<nrows*ncols; i++){

                if(i != 0 && i % matrixLength == 0){
                        printf("\n");
                }
            printf("  %.2f " ,  c[i]);

         }

        printf("\n");
    }

    else {

      MPI_Bcast(b, ncols*nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);

        if (myid <= nrows) {


            while(1) {

              MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG,MPI_COMM_WORLD, &status);

              if (status.MPI_TAG == 0){
                break;
                }
              row = status.MPI_TAG;
              ans = 0.0;

              for (j = 0; j < ncols; j++) {

                ans += buffer[j] * b[j*nrows + ((row-1)%ncols)];
              }

              MPI_Send(&ans, 1, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
            }
        }
    }
}
        else{
                printf("\n****************************************************************************************\n");
                printf("The processors shouldn't be greater than matrix size\n");
                printf("Plases Re-eneter a  number for processor that is less than or equal to the matrix size\n");
                printf("*****************************************************************************************\n");
        }
  }
  else {
    fprintf(stderr, "Usage matrix_times_vector <size>\n");
  }
  MPI_Finalize();
  return 0;
}




