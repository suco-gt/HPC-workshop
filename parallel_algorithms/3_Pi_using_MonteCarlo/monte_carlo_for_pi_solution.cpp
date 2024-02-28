#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For time()
#include <mpi.h>

int main(int argc, char* argv[]) 
{
	int rank, size;
	long int n;
	double start_time, stop_time;
	
	// Initialize MPI environment
	MPI_Init(&argc, &argv);

	// Communicator that includes all processes in the MPI program 
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	if(rank == 0)
	{
		start_time = MPI_Wtime();
		// Initalize data size n input through command line
		n = std::atol(argv[1]);
	}
	
	// Broadcast to all processes in comm
	MPI_Bcast(&n, 1, MPI_LONG, 0, comm);
	srand(time(NULL) + rank);
	long int local_num_points = n / size;
	int global_extra_num_points = n % size;
	
	// Some processors get one extra data point to compute
	if(rank < global_extra_num_points)
		local_num_points++;

	long int local_num_points_in_circle = 0;
	long int global_num_points_in_circle = 0;
	
	while(local_num_points--)
	{
		double x = ((double)std::rand()) / RAND_MAX; 
		double y = ((double)std::rand()) / RAND_MAX;
		if(((x * x) + (y * y)) <= 1)
		{
			local_num_points_in_circle++;
		}
	}
	
	/* TODO: Use the appropriate MPI function to perform a reduction operation on the variable local_num_points_in_circle across all MPI processes. 
	 * Sum up the values and store the result in the variable global_num_points_in_circle. Ensure that the result is stored on the process with rank 0 in the communicator comm.
	*/
	MPI_Reduce(&local_num_points_in_circle, &global_num_points_in_circle, 1, MPI_LONG, MPI_SUM, 0, comm);
	
	if(rank == 0)
	{
		double pi = ((double)global_num_points_in_circle) * 4 / n;
		stop_time = MPI_Wtime();
		printf("%.12f, %f", pi, stop_time - start_time);
	}
	
	return MPI_Finalize();
}	
