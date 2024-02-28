
# Prefix Sum

Given an integer array X of size n, return a new array S of the same size where the value stored at the i-th index of this newly formed array denotes the sum of the subarray of X that starts from the 0 index and ends at the i index.

$$
\text{{prefix\_sum}}(x_i) = \sum_{j=0}^{i} x[j] 
$$

## Serial Solution

One way to solve this problem serially would be as follows (there is more than 1 solution):

```
S[0] = X[0];
for i in range (1 to n-1):
    S[i] = S[i-1] + X[i];
```

**Computation Time:** O(n)

## Parallel Prefix Sum 

A high level abstraction of the parallelized algorithm would be:

1. Calculate the prefix sum of the local n/p elements -> O(n / p)
2. Perform parallel prefix algorithm using hypercubic permutations with the last local result on each processor -> O(log p)
3. One processor displays the result

This gives the following runtime:
**Computation Time:** O((n / p) log p)
**Communication time:** ((𝜏 + 𝜇) log p) 
where 𝜏 is the setup time for each message passing and 𝜇 is the transmission time for each message.

Lets look at this in more detail now...

The set of processors exchanging messages in each round is decided by bit fixing. If you want to know more, refer a previous workshop on **hypercubic permutations** for details. Its on our github: [Hypercubic Permutations](https://github.com/suco-gt/parallel-tutorials/blob/main/hpc-techniques/hypercubic-permulations/hypercubic-permutations.ipynb).

So in each iteration a different set of processors exchange their total sums in order to communicate the total sum without any collision of message paths.

```
For j in (0 to d-1):
	partner_rank = my_rank XOR (2^j)
	Exchange total_sum with partner_rank
	total_sum += received_total_sum
	if exchange is done with a processor of smaller id, then 
		prefix_sum += received_total_sum
```

This pseudo code is implemented in cpp as:

```cpp
int dim = log2(size);
for (int d = 0; d < dim; d++)
{
	int mask = 1 << d;
	int partner = rank ^ mask;

	MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
	MPI_Recv(&temp, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);
	sum += temp;
	
	if (partner < rank)
		prefix_sum += temp;
	
	MPI_Barrier(MPI_COMM_WORLD);
}
```

Here, `MPI_Barrier(MPI_COMM_WORLD)` is a synchronization point that ensures that all processes in the specified communicator reach the same point in the program before any of them proceeds further. Keep in mind that the use of barriers can affect the performance of parallel programs, and unnecessary or poorly-placed barriers may lead to increased idle time for some processes. However, in certain situations, barriers are necessary to ensure correct program execution.

The line `MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);` sends the value of the variable `sum` from the current MPI process to another process with rank `partner` using MPI communication. Similarly, the line `MPI_Recv(&temp, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);` receives an integer value from the MPI process with rank `partner` and stores it in the variable `temp`, using MPI communication, with `status` information recorded in the status variable.

### Compile and Run the Code

To compile, issue the command `make`. This uses the Makefile, which is a way of automating the compilation process. The commands that are actually run are shown as output. Then use `srun` to allocate cores and run:

```
$ make
$ srun -n 256 ./prefix
```

Remember that the current makefile compiles the solution code, so if you want to compile and run your code in `prefix_sum.cpp` then you need to change the makefile or paste this in the terminal:

```
$ mpicxx -o prefix prefix_sum.cpp
$ srun -n 256 ./prefix
```

The program output should look something like the extract below. The numbers may be different because we use rand(). The code is correct if `Calculated Sum` and `Correct Sum` are the same.

```
Prefix Sum of Rank 0: 2082836
Calculated Sum: 535968646
Correct Sum: 535968646
```
