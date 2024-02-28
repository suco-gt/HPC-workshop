# Shortest Path Problem

This exercise implements the Bellman-Ford algorithm for single-source shortest paths in graphs. The main advantage of the Bellman-Ford algorithm over Dijkstra’s algorithm is that it can be parallelized. Here, you will do exactly that! Your task is to write a program in C, including OpenMP directives, to calculate shortest paths in a directed graph using a parallel version of the Bellman-Ford algorithm. In this algorithm, the 
shortest-path estimates are updated each iteration by considering whether each edge can improve the estimates, with different threads handling different edges. Note that the number of iterations required may vary across different runs because of the asynchronous nature of the threads.

Details of the implementation:
* Input and command-line arguments: Your program should take 2 required command-line arguments and 1 optional command-line argument, in the following order.
	* **First command-line argument (required):** Filename that specifies the graph information. The first line of the file will include two integers, the number of vertices (numvertices) in the graph followed by the number of edges. The remaining lines correspond to the edges, with each line including three values associated with an edge: the “from” vertex (specified by an integer from 0 to numvertices-1), the “to” vertex (similarly specified), and a (positive) double value for the weight of the edge.Some sample input files corresponding to graphs of various sizes have been provided.
	* **Second command-line argument (required):** The number of threads to use in the parallel section(s) of the code. 
	* **Third command-line argument (optional):** If provided, this argument represents the index of a “destination” vertex for a shortest path beginning at vertex 0 for which output should be provided. The output will include the length of the shortest path from vertex 0 to this vertex, as well as the vertices of the path in the proper order (originating at the source vertex 0). If not provided, the same information should be provided, but for the shortest paths to all vertices (a total of numvertices, including the source vertex) instead of just to a single destination vertex. 
* An edge struct is used to dynamically allocate an array of edges to loop over as part of the Bellman-Ford algorithm.


## Algorithm

In the parallel version of the Bellman-Ford algorithm, edges may not be evaluated in the order in which they are listed because synchrony is not enforced across the threads, which is acceptable. Thus no barriers are added to force a certain order. 

The overview of the algorithm's parallelization: 

* The number of threads is set using the function `omp_set_num_threads()` 
* Parallelization is done across the edges. That is, the work that is distributed over the threads is that of considering whether a shorter distance estimate from the source vertex (0) to the `to` vertex of an edge can be found by going to the `from` vertex of that edge and then including that edge in the path.
* The `change_flag` in the algorithm is set to indicate whether anything changed, and thus whether further iterations are needed. This is a prime candidate for a reduction and reduces several unnecessary computation.
* Use `omp_get_wtime()` to get the time spent in the parallel section. 


We will use parallelization for initializing and relaxing the edges in the function that implements the bellman ford algorithm. This is computationally the heaviest part of the program which benefits from parallelization:

```cpp
void bellmanFord(Edge* edges, int num_threads, int num_edges, int num_vertices, int destination_vertex);
```

The distances are intialixed in parallel, which means that the number of assignments here and distributed across `num_threads`:
```cpp
omp_set_num_threads(num_threads);

// Initialize distances for each vertex
#pragma omp parallel for 
for (int i = 0; i < num_vertices; i++) 
{
	distances[i] = (i == 0) ? 0 : INF;
}
```

The edge relaxation step updates the distances to vertices (distances) based on the current estimates and the weights of the edges. For each edge `(u, v)` with weight `w`, it checks whether the distance to `v` can be improved by going through `u`. If so, it updates the distance to `v` and sets the predecessor of `v` to `u`.

This is the main logic which performs parallel relaxation of edges, updating distance estimates and predecessors for each vertex. It repeatedly relaxes the edges in parallel until no further improvement is possible, as indicated by the `change_flag`.
```cpp
// Relax edges in parallel
for (int j = 1; j <= num_vertices - 1; j++) 
{
	change_flag = 0;  // Reset the flag before each iteration
	
	#pragma omp parallel for reduction(|:change_flag)
	for (int i = 0; i < num_edges; i++) 
	{
		int u = edges[i].from;
		int v = edges[i].to;
		double w = edges[i].weight;

		if (distances[u] != INF && distances[u] + w < distances[v]) 
		{
			distances[v] = distances[u] + w;
			predecessors[v] = u;
			change_flag = 1;  
		}
	}
	
	if (!change_flag)  // still 0, so terminate early 
	{
		break;
	}
}
```


The `reduction(|:change_flag)` clause is used with the pragma to handle the `change_flag` variable. The | operator performs a bitwise OR reduction operation on the `change_flag` variable across all threads. This reduction ensures that if any thread modifies the `change_flag` to 1, the shared `change_flag` variable will be updated accordingly.

After the parallel relaxation step, it checks the `change_flag`. If `change_flag` is still 0 after all iterations, it means no updates were made in the last iteration, indicating that the algorithm has converged, and further iterations are unnecessary. In such a case, the algorithm terminates early.



Displaying results is done in 2 steps:
* `displayResults()` – It does the formatting of the output and prints the vertex, final distance, path. Since the path is returned as an array with the trace backwards (from destination to source), the printing is done from the end of the array
* `findPathInverted()` – this only deals with finding the path for the shortest distance from a node to the source 0

Finally, the makefile uses optimization flags to improve performance: 

```
gcc -o bellmanford bf.o -lm -fopenmp -O3
```


## Execution

This can be run on the cluster or on your local machine. Try to observe the speedup when the `num_threads` is 1 versus when it is high like 100. There are 4 graphs of varying sizes given in the folder. For compilation and run:
```
$ make
$ ./bellmanford [file_name] [num_threads] [optional_dest]
```
Example of one run:
```
./bellmanford graph500-100K.txt 100 4
```

Corresponding output:

```
4: 0.43770; 0 466 53 299 414 4
0.01194
```
