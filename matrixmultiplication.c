/********************************************
* 
* AMS530 Project 1
*
* NxN Matrix Multiplication
*
********************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define rowNum 1000	/* Number of rows */
#define colNum 1000	/* Number of columns */

#define MASTER 0

/* Variables to identify the source of messages */
#define FROM_MASTER 1
#define FROM_WORKER 2

MPI_Status status;

double a[rowNum][colNum], b[rowNum][colNum], c[rowNum][colNum];	    	/* Matrixes A, B and the result matrix C */

main(int argc, char **argv)
{
int numtasks, taskid, numworkers, source, dest, i, j, k, count,
    mtype,		       	/* message type */
    rows,                      	/* number of rows (matrix A) will be sent to each worker */
    averow, extra, offset;      /* variables used to determine the numnber of rows needed to sent to each worker */

/* seed for random number generating function */
int seed = 10000;
srand(seed);

/* variable to hold the time for the whole multiplication process */
double time;

int check;

check = MPI_Init(&argc, &argv);

if (check != MPI_SUCCESS) {
   printf("Error starting MPI program. Terminating... \n");
   MPI_Abort(MPI_COMM_WORLD, check);
}

MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
numworkers = numtasks-1;

/* Master */
if (taskid == MASTER) {
  printf("Number of worker tasks = %d\n", numworkers);

// Initialize matrix A and B. Number range: [-1,1] */
  for (i=0; i<rowNum; i++)
    for (j=0; j<colNum; j++)
	a[i][j]=((rand()/((RAND_MAX)+1.0)) * 2) - 1;
  for (i=0; i<rowNum; i++)
    for (j=0; j<colNum; j++)
      b[i][j]=((rand()/((RAND_MAX)+1.0)) * 2) - 1;

time = MPI_Wtime();	/* Start timer counter */

  /* send matrix data to the worker tasks */
  averow = rowNum / numworkers;
  extra = rowNum % numworkers;
  offset = 0;
  mtype = FROM_MASTER;

  for (dest = 1; dest <= numworkers; dest++) {
    // Determine the number of rows to send to the worker
    rows = (dest <= extra) ? averow+1 : averow;

    // Send the offset value to the worker
    MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);

    // Send the 'rows' value to the worker
    MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
    count = rows*colNum;
    MPI_Send(&a[offset][0], count, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
    count = colNum*colNum;
    MPI_Send(&b, count, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);

    // Update the offset value
    offset = offset + rows;
    }

  /* Receive results from all workers and update matrix C*/
  mtype = FROM_WORKER;
  for (i=1; i<=numworkers; i++)	{
    source = i;

    // Receive the offset value
    MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);

    // Receive the number of 'rows'
    MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
    count = rows*colNum;
    MPI_Recv(&c[offset][0], count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD,
               &status);

    }

  /* Print out the result matrix C. (Take out #ifdef #endif to print). This will greatly increase the performance time however */
  #ifdef PRINT
  printf("The result matrix C\n");
  for (i=0; i<rowNum; i++) {
    printf("\n");
    for (j=0; j<colNum; j++)
      printf("%6.2f   ", c[i][j]);
    }
  printf ("\n");
  #endif

  time=MPI_Wtime() - time;	/* Stop timer counter */

  printf("Total time: %f\n\n", time);

  }

/* Worker */
if (taskid > MASTER) {
  mtype = FROM_MASTER;
  source = MASTER;

  /* Receive data from Master */
  MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
  MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
  count = rows*colNum;
  MPI_Recv(&a, count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
  count = colNum*colNum;
  MPI_Recv(&b, count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);

  /* Peform the calculattion */
  for (k=0; k<colNum; k++)
    for (i=0; i<rows; i++) {
      c[i][k] = 0.0;
      for (j=0; j<colNum; j++)
        c[i][k] = c[i][k] + a[i][j] * b[j][k];
      }

  /* Sending back data to Master */
  MPI_Send(&offset, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
  MPI_Send(&rows, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
  MPI_Send(&c, rows*colNum, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
