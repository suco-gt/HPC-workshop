# Estimating π using Monte Carlo
Monte Carlo methods are a broad class of computational algorithms that rely on repeated random sampling to obtain numerical results. In this example, we use it to estimate the value of Pi. 

## Estimation of Pi 
The idea is to simulate random (x, y) points in a 2-D plane with domain as a square of side 1 unit centered on (0,0). Imagine a circle inside the same domain with same radius r and inscribed into the square. We then calculate the ratio of number points that lied inside the circle and total number of generated points. 

The random points less than the side of the square 1 are generated using:
```
srand(time(NULL) + rank);
double x = ((double)std::rand()) / RAND_MAX;
double y = ((double)std::rand()) / RAND_MAX;
```

The check for if the point is inside or outside the circle is done using this if condition:
```
if (((x * x) + (y * y)) <= 1)
{
	local_num_points_in_circle++;
}
```

A global sum reduction operation on the variable local_num_points_in_circle is done across all processes in the communicator. The result is then stored in the variable global_num_points_in_circle on the process with rank 0:
```
MPI_Reduce(&local_num_points_in_circle,&global_num_points_in_circle, 1, MPI_LONG, MPI_SUM, 0, comm);
```

The process with rank 0 then continues to calculate the estimate of pi, calculate the running time and display these outputs. Pi is calculated as:
```
double pi = ((double)global_num_points_in_circle)*4/n;
```

Compile:
mpicxx -o calc_pi monte_carlo_for_pi.cpp

Run:
srun -n 100 ./calc_pi 1000000
