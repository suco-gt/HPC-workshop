
# Prefix Sum

This problem is a continuation of the previous one `4_Prefix_Sum`. You can refer that README file for details of the problem statement and the implementation.

In this exercise, the goal is to extend the solution allowing the hypercubic permutations to run when the number of processors is not a power of 2.


## Solution

The overview of the solution is as follows:

1. Find `new_size` such that it is power of 2 and `(new_size / 2) < actual_size < new_size`
2. The code runs same as before, only with `new_size` processors
3. Ignore communications to/from non-existing processors, that is `rank >= actual_size`


The computation and communication costs remain the same as before.
**Computation Time:** O((n / p) log p)
**Communication time:** ((𝜏 + 𝜇) log p) 
where 𝜏 is the setup time for each message passing and 𝜇 is the transmission time for each message.


## Implementation

There are only a few changes in the original code. The `new_size` is calculated as follows:

```
new_size = pow(2, ceil(log2(size)));
```

Inside the main for loop for hypercubic permutations, a condition `if (partner < size)` is added to send and receive messages of only the existent processors:
```
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
```

> Refer a previous workshop on **hypercubic permutations** for details. Its on our github: [Hypercubic Permutations](https://github.com/suco-gt/parallel-tutorials/blob/main/hpc-techniques/hypercubic-permulations/hypercubic-permutations.ipynb).


### Compile and Run the Code

To compile, issue the command `make`. This uses the Makefile, which is a way of automating the compilation process. The commands that are actually run are shown as output. Then use `srun` to allocate cores and run:

```
$ make clean
$ make
$ srun -n 200 ./prefix
```

Remember that the current makefile compiles the solution code, so if you want to compile and run your code in `prefix_sum.cpp` then you need to change the makefile or paste this in the terminal:

```
$ mpicxx -o prefix prefix_sum.cpp
$ srun -n 200 ./prefix
```

The program output should look something like the extract below. The numbers may be different because we use rand(). The code is correct if `Calculated Sum` and `Correct Sum` are the same.

```
Actual number of processors: 200
Assumed number of processors: 256
Calculated Sum: 535968646
Correct Sum: 535968646
```
