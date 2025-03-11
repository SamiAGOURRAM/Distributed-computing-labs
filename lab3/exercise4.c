#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define DATA_SIZE 10000000 // Size of the input data array

int main() {
    int *data = NULL;  // Input data array
    int *result = NULL; // Result array
    double start_time, end_time;
    int i;
    int sum = 0;
    
    // Allocate memory for data arrays
    data = (int*)malloc(DATA_SIZE * sizeof(int));
    result = (int*)malloc(DATA_SIZE * sizeof(int));
    
    if (data == NULL || result == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Start timing
    start_time = omp_get_wtime();
    
    // Begin parallel region with all threads
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        
        /************ STAGE 1: Read input data ************/
        #pragma omp single
        {
            printf("Stage 1: Thread %d is reading input data...\n", thread_id);
            
            // Simulate reading data by initializing the array
            for (i = 0; i < DATA_SIZE; i++) {
                data[i] = rand() % 100;  // Random values between 0 and 99
            }
            
            printf("Stage 1 complete: Input data has been read\n");
        }
        // Implicit barrier at the end of single ensures all threads 
        // wait until the data is ready
        
        /************ STAGE 2: Process data in parallel ************/
        printf("Stage 2: Thread %d starting data processing\n", thread_id);
        
        // Each thread processes a portion of the data
        #pragma omp for
        for (i = 0; i < DATA_SIZE; i++) {
            // Example processing: square each value
            result[i] = data[i] * data[i];
        }
        
        // Explicit barrier to ensure all processing is complete
        #pragma omp barrier
        
        printf("Thread %d has completed its portion of Stage 2\n", thread_id);
        
        /************ STAGE 3: Write final result ************/
        #pragma omp single
        {
            printf("Stage 3: Thread %d is finalizing results...\n", thread_id);
            
            // Compute some final statistic from the results
            sum = 0;
            for (i = 0; i < DATA_SIZE; i++) {
                sum += result[i];
            }
            
            printf("Stage 3 complete: Final sum of squared values = %d\n", sum);
        }
    }
    
    // End timing
    end_time = omp_get_wtime();
    
    printf("Program completed in %.6f seconds\n", end_time - start_time);
    
    // Verify result with sequential calculation
    int verify_sum = 0;
    for (i = 0; i < DATA_SIZE; i++) {
        verify_sum += data[i] * data[i];
    }
    
    printf("Verification: %s (parallel: %d, sequential: %d)\n", 
           (sum == verify_sum) ? "PASSED" : "FAILED", sum, verify_sum);
    
    // Free allocated memory
    free(data);
    free(result);
    
    return 0;
}