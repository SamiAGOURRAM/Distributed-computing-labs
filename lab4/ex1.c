#include <stdio.h>
#include <mpi.h>
 
int main(int argc, char** argv) {
    int rank,size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
     
    if (rank == 0) {
        printf("Hello World from the master process (rank 0). Total processes: %d\n", size);
    }
     
    MPI_Finalize();
    return 0;
}
