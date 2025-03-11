#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 1000 // Matrix dimension (N x N)

int main() {
    int i, j;
    double **matrix;
    double sum = 0.0;
    double start_time, end_time, seq_time, parallel_time, parallel_sum_time;
    double single_end_time; // Time after single section completes
    
    // Allocate matrix on the heap
    matrix = (double **)malloc(N * sizeof(double *));
    for (i = 0; i < N; i++) {
        matrix[i] = (double *)malloc(N * sizeof(double));
    }
    
    // Check if memory allocation was successful
    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Seed the random number generator
    srand(time(NULL));
    
    printf("Matrix size: %d x %d\n", N, N);
    
    
    // Initialize matrix
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            matrix[i][j] = ((double)rand() / RAND_MAX) * 100.0;
        }
    }
    
    // Print a small sample of the matrix (e.g., top-left 3x3)
    printf("\nSequential execution - Sample of matrix (top-left 3x3):\n");
    for (i = 0; i < 3 && i < N; i++) {
        for (j = 0; j < 3 && j < N; j++) {
            printf("%8.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    // Sequential execution (no OpenMP)
    start_time = omp_get_wtime();
    // Compute sum sequentially
    sum = 0.0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            sum += matrix[i][j];
        }
    }
    
    end_time = omp_get_wtime();
    seq_time = end_time - start_time;
    
    printf("Sequential sum: %.2f\n", sum);
    printf("Sequential execution time: %.6f seconds\n\n", seq_time);
    
    // Parallel execution with OpenMP
    start_time = omp_get_wtime();
    sum = 0.0;
    
    #pragma omp parallel
    {
        // Master thread initializes the matrix
        #pragma omp master
        {
            printf("Thread %d (master) initializing the matrix...\n", omp_get_thread_num());
            for (i = 0; i < N; i++) {
                for (j = 0; j < N; j++) {
                    matrix[i][j] = ((double)rand() / RAND_MAX) * 100.0;
                }
            }
        }
        
        // Ensure all threads wait until matrix is initialized
        #pragma omp barrier
        
        // Single thread prints the matrix sample
        #pragma omp single
        {
            printf("Thread %d (single) printing matrix sample (top-left 3x3):\n", omp_get_thread_num());
            for (i = 0; i < 3 && i < N; i++) {
                for (j = 0; j < 3 && j < N; j++) {
                    printf("%8.2f ", matrix[i][j]);
                }
                printf("\n");
            }
            
            // Record time after single section completes
            single_end_time = omp_get_wtime();
        }
        
        // Start timing the parallel sum calculation
        double parallel_sum_start = omp_get_wtime();
        
        // All threads compute the sum in parallel
        double local_sum = 0.0;
        
        #pragma omp for collapse(2) private(i, j)
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                local_sum += matrix[i][j];
            }
        }
        
        // Combine all local sums
        #pragma omp critical
        {
            sum += local_sum;
        }
        
        // End timing the parallel sum calculation (per thread)
        #pragma omp single
        {
            double parallel_sum_end = omp_get_wtime();
            parallel_sum_time = parallel_sum_end - parallel_sum_start;
        }
    }
    
    end_time = omp_get_wtime();
    parallel_time = end_time - start_time;
    
    printf("Parallel sum: %.2f\n", sum);
    printf("Total parallel execution time: %.6f seconds\n", parallel_time);
    printf("Time for parallel sum computation: %.6f seconds\n\n", parallel_sum_time);
    
    // Compare performance
    printf("Performance comparison:\n");
    printf("Sequential time:           %.6f seconds\n", seq_time);
    printf("Parallel section timings:\n");
    printf("  - Initialization (master): %.6f seconds\n", single_end_time - start_time);
    printf("  - Sum computation:         %.6f seconds\n", parallel_sum_time);
    printf("  - Total parallel time:     %.6f seconds\n", parallel_time);
    printf("Speedup (total):           %.2f times\n", seq_time / parallel_time);
    printf("Speedup (sum computation): %.2f times\n", (seq_time / 3) / parallel_sum_time); // Assuming sum is ~1/3 of seq time
    
    // Free allocated memory
    for (i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
    
    return 0;
}