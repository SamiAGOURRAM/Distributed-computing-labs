#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define N 10000000 // Size of the array

int main() {
    int i;
    double *array = (double*)malloc(N * sizeof(double));
    double sum = 0.0, max_val = 0.0, std_dev = 0.0, mean = 0.0, variance = 0.0;
    double start_time, end_time;
    
    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize array with random values between 0 and 100
    printf("Initializing array with %d random values...\n", N);
    for (i = 0; i < N; i++) {
        array[i] = ((double)rand() / RAND_MAX) * 100.0;
    }
    
    // Record start time
    start_time = omp_get_wtime();
    
    // Parallel sections to compute different statistics
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // Section 1: Compute the sum of all elements
            double local_sum = 0.0;
            for (i = 0; i < N; i++) {
                local_sum += array[i];
            }
            sum = local_sum;
            mean = sum / N;
            printf("Thread %d computed sum: %.2f (mean: %.2f)\n", 
                   omp_get_thread_num(), sum, mean);
        }
        
        #pragma omp section
        {
            // Section 2: Compute the maximum value
            double local_max = array[0];
            for (i = 1; i < N; i++) {
                if (array[i] > local_max) {
                    local_max = array[i];
                }
            }
            max_val = local_max;
            printf("Thread %d found maximum value: %.2f\n", 
                   omp_get_thread_num(), max_val);
        }
        
        #pragma omp section
        {
            // Section 3: Compute the standard deviation
            // First we need the mean, but we can calculate it independently
            double local_mean = 0.0;
            for (i = 0; i < N; i++) {
                local_mean += array[i];
            }
            local_mean /= N;
            
            // Now calculate variance
            double local_variance = 0.0;
            for (i = 0; i < N; i++) {
                local_variance += (array[i] - local_mean) * (array[i] - local_mean);
            }
            local_variance /= N;
            
            // Standard deviation is the square root of variance
            std_dev = sqrt(local_variance);
            
            printf("Thread %d computed standard deviation: %.2f\n", 
                   omp_get_thread_num(), std_dev);
        }
    }
    
    // Record end time
    end_time = omp_get_wtime();
    
    // Print results
    printf("\nResults:\n");
    printf("Sum: %.2f\n", sum);
    printf("Mean: %.2f\n", mean);
    printf("Maximum value: %.2f\n", max_val);
    printf("Standard deviation: %.2f\n", std_dev);
    printf("Computation time: %.6f seconds\n", end_time - start_time);
    
    // Free allocated memory
    free(array);
    
    return 0;
}