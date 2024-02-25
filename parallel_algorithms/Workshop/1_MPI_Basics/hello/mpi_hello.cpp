#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    // Initialize MPI (Put the MPI initialization functions here)
     MPI_Init(&argc, &argv);  

    // TODO: Get the number of ranks (size) from the Communicator (Put the MPI size function here):
    //
    
    // TODO: Get the rank for each process from the communicator. (Put the MPI rank function here):
    //
    
    // Printing hello from each rank
    printf("Hello from rank %d of %d total \n", rank, size);
    
    // Clean up and finalize the MPI environment (Put the MPI finalization function here):
    //
    
    return 0;
}
