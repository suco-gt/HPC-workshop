#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[]){

  int num_threads;
  int thread_id;

  #pragma omp parallel default(none) shared(num_threads) private(thread_id)
  {
    num_threads = omp_get_num_threads();
    thread_id = omp_get_thread_num();

    printf("Hello from OpenMP thread %03d of %03d\n", thread_id, num_threads);
  }

  return 0;
}
