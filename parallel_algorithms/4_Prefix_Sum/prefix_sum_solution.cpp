#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define ARRAY_SIZE 1048576

int main(int argc, char *argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	if (ARRAY_SIZE % size != 0 && rank == 0)
	{
		fprintf(stderr, "Array size must be multiple of mpi job size.\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	MPI_Status status;
	int *array = (int *)malloc(sizeof(int) * ARRAY_SIZE);
	int *chunk;
	int *sendcounts = (int *)malloc(sizeof(int) * size);
	int *displs = (int *)malloc(sizeof(int) * size);
	int i = 0;

	int total_sum = 0;
	for (i = 0; i < ARRAY_SIZE; i++)
	{
		array[i] = rand() % 1024;
		total_sum += array[i];
	}

	// Calculate send counts and displacements for MPI_Scatterv
	int local_size = ARRAY_SIZE / size;
	for (i = 0; i < size; i++)
	{
		sendcounts[i] = local_size;
		displs[i] = i * local_size;
	}

	// Adjust send counts and displacements for the last process
	sendcounts[size - 1] += ARRAY_SIZE % size;

	// Allocate memory for the local chunk
	chunk = (int *)malloc(sizeof(int) * sendcounts[rank]);

	MPI_Barrier(MPI_COMM_WORLD);

	/* TODO:
	Utilize an MPI function to scatter portions of the array to different processes based on sendcounts and displs. Consider the roles of chunk, sendcounts[rank], and MPI_COMM_WORLD.
	*/
	MPI_Scatterv(array, sendcounts, displs, MPI_INT, chunk, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

	int sum = 0;
	int prefix_sum = 0;
	int temp = 0;
	int key = 1;

	// Finding sum of all local data points before combining with other processors
	for (i = 0; i < sendcounts[rank]; i++)
	{
		sum += chunk[i];
	}
	prefix_sum = sum;

	// Read about hypercubic permutations from our github. Communication uses that topology
	int dim = log2(size);
	for (int d = 0; d < dim; d++)
	{
		int mask = 1 << d;
		int partner = rank ^ mask;

		/* TODO:
		Utilize the provided MPI functions to communicate the value of sum between processes. 
		Be mindful of the parameters used in MPI_Send and MPI_Recv and consider the roles of partner and MPI_COMM_WORLD. 
		*/ 
		MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
		MPI_Recv(&temp, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);
		sum += temp;
		
		if (partner < rank)
			prefix_sum += temp;
		
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if (rank == size - 1)
	{
		fprintf(stderr, "Calculated Sum: %d\n", sum);
		fprintf(stderr, "Correct Sum: %d\n", total_sum);
	}
	
	//If you want to print the prefix sum a particular process uncomment this and set the rank
	if (rank == size - 1) // (rank == 0)
	{
		fprintf(stderr, "Prefix Sum of Rank %d: %d\n", rank, prefix_sum);
	}

	free(array);
	free(chunk);
	MPI_Finalize();
	return 0;
}
