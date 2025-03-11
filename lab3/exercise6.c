#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 8
#define NUM_ITERATIONS 10000000

void increment_counter_critical(long long *counter, int num_iterations) {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for
        for (int i = 0; i < num_iterations; i++) {
            #pragma omp critical
            {
                (*counter)++;
            }
        }
    }
}

void increment_counter_atomic(long long *counter, int num_iterations) {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for
        for (int i = 0; i < num_iterations; i++) {
            #pragma omp atomic
            (*counter)++;
        }
    }
}

int main() {
    long long counter_critical = 0;
    long long counter_atomic = 0;
    double start_time, end_time;
    
    printf("Testing counter incrementation with %d threads and %d iterations\n", 
           NUM_THREADS, NUM_ITERATIONS);
    
    start_time = omp_get_wtime();
    increment_counter_critical(&counter_critical, NUM_ITERATIONS);
    end_time = omp_get_wtime();
    
    printf("Critical section:\n");
    printf("  Final counter value: %lld\n", counter_critical);
    printf("  Execution time: %.6f seconds\n", end_time - start_time);
    
    start_time = omp_get_wtime();
    increment_counter_atomic(&counter_atomic, NUM_ITERATIONS);
    end_time = omp_get_wtime();
    
    printf("Atomic directive:\n");
    printf("  Final counter value: %lld\n", counter_atomic);
    printf("  Execution time: %.6f seconds\n", end_time - start_time);
    
    double speedup = (end_time - start_time > 0) ? 
                     (counter_critical / (end_time - start_time)) / 
                     (counter_atomic / (end_time - start_time)) : 0;
                     
    printf("\nPerformance comparison:\n");
    if (end_time - start_time > 0) {
        printf("  Atomic operations are typically %.2f times faster than critical sections\n", 
               speedup);
    }
    
    return 0;
}