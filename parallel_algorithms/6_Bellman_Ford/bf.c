#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include <time.h>

#define INF 99999

// Structure for edge
typedef struct edge
{
	int from, to;
	double weight;
} Edge;

// Function definitions
void bellmanFord(Edge* edges, int num_threads, int num_edges, int num_vertices, int destination_vertex);
void displayResults(double distances[], int predecessors[], int num_vertices, int destination_vertex);
int* findPathInverted(int vertex, int num_vertices, int predecessors[], int* index);


int main(int argc, char *argv[]) 
{
	char *filename = argv[1];
	int num_threads = atoi(argv[2]);
	int num_edges = 0, num_vertices = 0;
	int destination_vertex = -1;
	
	if (argc > 3) // checking if it is entered
	{
		destination_vertex = atoi(argv[3]);
	}
	
	FILE *file = fopen(filename, "r");
	
	if (file == NULL)  // file name is incorrect
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	fscanf(file, "%d %d", &num_vertices, &num_edges);
	
	Edge* edges = (Edge*)malloc(num_edges * sizeof(Edge));
	for (int i = 0; i < num_edges; i++)
	{
		fscanf(file, "%d %d %lf", &edges[i].from, &edges[i].to, &edges[i].weight);
	}

	fclose(file);

	double start_time = omp_get_wtime();
	bellmanFord(edges, num_threads, num_edges, num_vertices, destination_vertex);
	double end_time = omp_get_wtime();

	printf("%.5f\n", end_time - start_time);  // elapsed time
	
	// Memory release
	free(edges);

	return 0;
}


void bellmanFord(Edge* edges, int num_threads, int num_edges, int num_vertices, int destination_vertex)
{
	double distances[num_vertices];
	int predecessors[num_vertices];
	int change_flag = 0;			// used to note if loop can be terminated early
	
	omp_set_num_threads(num_threads);
	
	// Initialize distances for each vertex
	#pragma omp parallel for 
	for (int i = 0; i < num_vertices; i++) 
	{
		distances[i] = (i == 0) ? 0 : INF;
	}
	
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
	
	displayResults(distances, predecessors, num_vertices, destination_vertex);
}


void displayResults(double distances[], int predecessors[], int num_vertices, int destination_vertex)
{
	/* Print shortest paths or abbreviated output based on the command-line argument
	** destination_vertex is -1 if there was no vertex specified in the command line
	*/
	
	if (destination_vertex >= 0 && destination_vertex < num_vertices) 
	{
		if (distances[destination_vertex] == INF)
		{
			// Format for infinite distance
			printf("%d: INFTY; %d\n", destination_vertex, destination_vertex);
		}
		else
		{
			// Printing as "vertex: distance; path"
			printf("%d: %.5f; ", destination_vertex, distances[destination_vertex]);
			int index = 0;
			int* path = findPathInverted(destination_vertex, num_vertices, predecessors, &index);
			
			for (int i = index-1; i >=0; i--) 
			{
				printf("%d ", path[i]);
			}
			printf("\n");
			free(path);
		}
	} 
	else 
	{
		for (int i = 0; i < num_vertices; i++) 
		{
			if (distances[i] == INF)
			{
				// Format for infinite distance
				printf("%d: INFTY; %d\n", i, i);
			}
			else
			{
				// Printing as "vertex: distance; path"
				printf("%d: %.5f; ", i, distances[i]);
				int index = 0;
				int* path = findPathInverted(i, num_vertices, predecessors, &index);
				
				// Printing as source to destination
				for (int i = index-1; i >=0; i--) 
				{
					printf("%d ", path[i]);
				}
				printf("\n");
				free(path);
			}
		}
	}
}


int* findPathInverted(int vertex, int num_vertices, int predecessors[], int* index)
{
	/* This index pointer is used in calling function to know size of the array utilized
	** The path array is created here to hold the vertices in the path from destination to source 
	*/
	
	int current = vertex;  // destination
	int* path = (int*)malloc(num_vertices * sizeof(int));
	*index = 0; 
	
	// Travesing from destination to source (inverted path)
	while (current != 0)
	{
		path[*index] = current;	 // add to path
		(*index)++;
		current = predecessors[current];
	}
	path[*index] = 0;  // adding source
	(*index)++;
	
	return path;
}