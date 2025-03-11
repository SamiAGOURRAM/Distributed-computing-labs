#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 100

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;           
    int out;          
    int count;        
    omp_lock_t lock;  
    omp_lock_t not_full;    
    omp_lock_t not_empty;  
} circular_buffer_t;

void init_buffer(circular_buffer_t *cb) {
    cb->in = 0;
    cb->out = 0;
    cb->count = 0;
    
    omp_init_lock(&cb->lock);
    omp_init_lock(&cb->not_full);
    omp_init_lock(&cb->not_empty);
    
    omp_set_lock(&cb->not_empty);
}

void destroy_buffer(circular_buffer_t *cb) {
    omp_destroy_lock(&cb->lock);
    omp_destroy_lock(&cb->not_full);
    omp_destroy_lock(&cb->not_empty);
}

void producer(circular_buffer_t *cb, int id) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = id * 1000 + i; 
        
        if (cb->count == BUFFER_SIZE) {
            omp_set_lock(&cb->not_full);
        }
        
        omp_set_lock(&cb->lock);
        
        cb->buffer[cb->in] = item;
        cb->in = (cb->in + 1) % BUFFER_SIZE;
        cb->count++;
        
        printf("Producer %d: Produced item %d (Count: %d)\n", id, item, cb->count);
        
        if (cb->count == 1) {
            omp_unset_lock(&cb->not_empty);
        }
        
        omp_unset_lock(&cb->lock);
        
        usleep(rand() % 50000);  
    }
}

void consumer(circular_buffer_t *cb, int id) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        if (cb->count == 0) {
            omp_set_lock(&cb->not_empty);
        }
        
        omp_set_lock(&cb->lock);
        
        int item = cb->buffer[cb->out];
        cb->out = (cb->out + 1) % BUFFER_SIZE;
        cb->count--;
        
        printf("Consumer %d: Consumed item %d (Count: %d)\n", id, item, cb->count);
        
        if (cb->count == BUFFER_SIZE - 1) {
            omp_unset_lock(&cb->not_full);
        }
        
        omp_unset_lock(&cb->lock);
        
        usleep(rand() % 100000); 
    }
}

int main() {
    circular_buffer_t buffer;
    init_buffer(&buffer);
    
    double start_time = omp_get_wtime();
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            producer(&buffer, 0);
        }
        
        #pragma omp section
        {
            consumer(&buffer, 0);
        }
    }
    
    double end_time = omp_get_wtime();
    printf("Total execution time: %.4f seconds\n", end_time - start_time);
    
    destroy_buffer(&buffer);
    return 0;
}

int main_enhanced() {
    circular_buffer_t buffer;
    init_buffer(&buffer);
    
    double start_time = omp_get_wtime();
    
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        if (id < 2) {
            producer(&buffer, id);
        } else {
            consumer(&buffer, id - 2);
        }
    }
    
    double end_time = omp_get_wtime();
    printf("Total execution time with multiple producers/consumers: %.4f seconds\n", 
           end_time - start_time);
    
    destroy_buffer(&buffer);
    return 0;
}