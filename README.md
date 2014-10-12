Matrix-Multiplication-multicore
===============================

AMS 530

Problem description
	To compute NxN matrix multiplication with MPI and compare the performance of different matrix size using different numbers of processors

Program description
	The program is written is C programming language. It applied the idea of row-wise decomposition to compute the product of two square matrixes. 
	It use a master task to distribute jobs to P-1 worker tasks. 
	
	Here P is the number of processors. The distribution is determined by three variable, averow, extra and offset.
	
	Averow: the average number of row will be send to each worker
	
	Extra: this value will be taken into account to increase the number of rows send to workers if the average value won’t be able to cover up the whole data.
	For example, if the total row number is 5, and 4 processors are used. The average number of row (which is 1) will not be covered up the date need to process. 
	Therefore, two rows will be sent to first processor instead of 1.
	
	Offset: this value is used to determined the row index.
	
	The two squares matrixes are initialized with numbers in the range of [-1,1] using rand() function.
	
	The default value of N (size of the matrix) is 1000. To create a bigger matrix, just simply change the value of rowNum and colNum.
	
	Instead of calculating the time each worker takes to complete its operation, the performance time is calculated when the program starts to sending its data to workers to process and end when the master receives all the data it needs. This is the purpose of MPI_Wtime().
	
	The random number in range [-1,1] is calculated by
      ((rand()/((RAND_MAX)+1.0)) * 2) - 1;
