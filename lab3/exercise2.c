#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define NUM_VALUES 20  // Total number of values to generate

int main() {
    int values[NUM_VALUES];
    int i, j, temp;
    double start_time, generation_time, sorting_time, total_time;
    
    // Seed the random number generator
    srand(time(NULL));
    
    // Start timing
    start_time = omp_get_wtime();
    
    // Generate random numbers in parallel and store them
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        
        // Determine which values this thread will generate
        int values_per_thread = NUM_VALUES / num_threads;
        int start_idx = thread_id * values_per_thread;
        int end_idx = (thread_id == num_threads - 1) ? NUM_VALUES : start_idx + values_per_thread;
        
        // Generate random values between 1 and 100
        for (i = start_idx; i < end_idx; i++) {
            values[i] = rand() % 100 + 1;
            
            // Print information about which thread generated which value
            printf("Thread %d generated value: %d\n", thread_id, values[i]);
        }
    }
    
    // Record time after generation
    generation_time = omp_get_wtime() - start_time;
    printf("\nAll values generated in %.6f seconds\n", generation_time);
    printf("Starting parallel sorting...\n");
    
    // Parallelize the sorting using odd-even sort algorithm
    int sorted = 0;
    int iterations = 0;
    
    while (!sorted) {
        sorted = 1;
        iterations++;
        
        // Even phase (compare and swap elements at even indices)
        #pragma omp parallel for shared(values) reduction(&:sorted)
        for (i = 0; i < NUM_VALUES - 1; i += 2) {
            if (values[i] > values[i + 1]) {
                temp = values[i];
                values[i] = values[i + 1];
                values[i + 1] = temp;
                sorted = 0;  // Not sorted yet
            }
        }
        
        // Odd phase (compare and swap elements at odd indices)
        #pragma omp parallel for shared(values) reduction(&:sorted)
        for (i = 1; i < NUM_VALUES - 1; i += 2) {
            if (values[i] > values[i + 1]) {
                temp = values[i];
                values[i] = values[i + 1];
                values[i + 1] = temp;
                sorted = 0;  // Not sorted yet
            }
        }
    }
    
    // Record sorting time
    sorting_time = omp_get_wtime() - start_time - generation_time;
    total_time = omp_get_wtime() - start_time;
    
    // Print the sorted values
    printf("\nParallel sorting completed in %.6f seconds (%d iterations)\n", 
           sorting_time, iterations);
    printf("Printing values in ascending order:\n");
    
    for (i = 0; i < NUM_VALUES; i++) {
        printf("Value #%d: %d\n", i + 1, values[i]);
    }
    
    printf("\nPerformance summary:\n");
    printf("  Generation time: %.6f seconds\n", generation_time);
    printf("  Sorting time:    %.6f seconds\n", sorting_time);
    printf("  Total time:      %.6f seconds\n", total_time);
    
    return 0;
}