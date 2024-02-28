#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define ARRAY_SIZE 1048576

int main(int argc, char *argv[])
{
    int rank, size, new_size;
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    // Calculate the new size as the nearest power of 2 greater than or equal to the number of processors
    new_size = pow(2, ceil(log2(size)));
	
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

    // Use MPI_Scatterv instead of MPI_Scatter
    MPI_Scatterv(array, sendcounts, displs, MPI_INT, chunk, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    int sum = 0;
    int temp = 0;

    // Finding sum of all local data points before combining with other processors
    for (i = 0; i < sendcounts[rank]; i++)
    {
        sum += chunk[i];
    }
	
    // Read about hypercubic permutations from our github. Communication uses that topology
    int dim = log2(new_size);
    
	for (int d = 0; d < dim; d++)
	{
		int mask = 1 << d;
		int partner = rank ^ mask;
		
		if (partner < size)
		{
			MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
			MPI_Recv(&temp, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);
			sum += temp;
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

    if (rank == 0)
    {
        fprintf(stderr, "Actual number of processors: %d\n", size);
        fprintf(stderr, "Assumed number of processors: %d\n", new_size);
        fprintf(stderr, "Calculated Sum: %d\n", sum);
        fprintf(stderr, "Correct Sum: %d\n", total_sum);
    }

    free(array);
    free(chunk);
    MPI_Finalize();
    return 0;
}
