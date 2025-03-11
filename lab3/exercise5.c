#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Adjust these parameters to simulate different workloads
#define LIGHT_WORK_SIZE 10000000    // Light computation
#define MODERATE_WORK_SIZE 50000000  // Moderate computation
#define HEAVY_WORK_SIZE 100000000   // Heavy computation

// Function prototypes
void taskA(void);  // Light computation
void taskB(void);  // Moderate computation
void taskC(void);  // Heavy computation
double do_work(long size);  // Function to simulate computational work

int main() {
    double start_time, end_time, naive_time, optimized_time, better_optimized_time;
    
    printf("Load Balancing with Parallel Sections\n");
    printf("-------------------------------------\n\n");
    
    // PART 1: Naive implementation with one task per section
    printf("Naive Implementation (one task per section):\n");
    start_time = omp_get_wtime();
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Thread %d executing Task A (light)...\n", omp_get_thread_num());
            taskA();
        }
        
        #pragma omp section
        {
            printf("Thread %d executing Task B (moderate)...\n", omp_get_thread_num());
            taskB();
        }
        
        #pragma omp section
        {
            printf("Thread %d executing Task C (heavy)...\n", omp_get_thread_num());
            taskC();
        }
    }
    
    end_time = omp_get_wtime();
    naive_time = end_time - start_time;
    printf("Naive implementation completed in %.6f seconds\n\n", naive_time);
    
    // PART 2: First optimized implementation
    printf("First Optimized Implementation:\n");
    start_time = omp_get_wtime();
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Thread %d executing Task C (heavy)...\n", omp_get_thread_num());
            taskC();
        }
        
        #pragma omp section
        {
            printf("Thread %d executing Task B (moderate) + Task A (light)...\n", omp_get_thread_num());
            taskB();
            taskA();
        }
    }
    
    end_time = omp_get_wtime();
    optimized_time = end_time - start_time;
    printf("First optimized implementation completed in %.6f seconds\n\n", optimized_time);
    
    // PART 3: Better optimized implementation with task chunking
    printf("Better Optimized Implementation (dividing heavy task):\n");
    start_time = omp_get_wtime();
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Thread %d executing 60%% of Task C (heavy part 1)...\n", omp_get_thread_num());
            double result = do_work(HEAVY_WORK_SIZE * 0.6);
            printf("Heavy task part 1 completed with result: %.2f\n", result);
        }
        
        #pragma omp section
        {
            printf("Thread %d executing 40%% of Task C (heavy part 2) + Task A (light)...\n", omp_get_thread_num());
            double result1 = do_work(HEAVY_WORK_SIZE * 0.4);
            printf("Heavy task part 2 completed with result: %.2f\n", result1);
            
            taskA();
        }
        
        #pragma omp section
        {
            printf("Thread %d executing Task B (moderate)...\n", omp_get_thread_num());
            taskB();
        }
    }
    
    end_time = omp_get_wtime();
    better_optimized_time = end_time - start_time;
    printf("Better optimized implementation completed in %.6f seconds\n\n", better_optimized_time);
    
    // Compare performance
    printf("Performance Comparison:\n");
    printf("  Naive implementation:          %.6f seconds\n", naive_time);
    printf("  First optimized implementation: %.6f seconds\n", optimized_time);
    printf("  Better optimized implementation: %.6f seconds\n", better_optimized_time);
    
    double improvement1 = (naive_time - optimized_time) / naive_time * 100.0;
    double improvement2 = (naive_time - better_optimized_time) / naive_time * 100.0;
    
    printf("  First optimization improvement: %.2f%%\n", improvement1);
    printf("  Better optimization improvement: %.2f%%\n", improvement2);
    
    return 0;
}

// Simulates a light computational task
void taskA() {
    double result = do_work(LIGHT_WORK_SIZE);
    printf("Task A completed with result: %.2f\n", result);
}

// Simulates a moderate computational task
void taskB() {
    double result = do_work(MODERATE_WORK_SIZE);
    printf("Task B completed with result: %.2f\n", result);
}

// Simulates a heavy computational task
void taskC() {
    double result = do_work(HEAVY_WORK_SIZE);
    printf("Task C completed with result: %.2f\n", result);
}

// Function to simulate computational work
// Returns a result to prevent compiler optimization
double do_work(long size) {
    double sum = 0.0;
    for (long i = 0; i < size; i++) {
        sum += sin(i) * cos(i);
    }
    return sum;
}