#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Collect in_circle counts from all ranks 
    long long int buffers[world_size];

    // Toss for every rank
    long long int number_in_circle = 0;

    long long int base = tosses / world_size;
    long long int rem  = tosses % world_size;
    long long int local_tosses = base + (world_rank < rem ? 1 : 0);

    srand(time(NULL) + world_rank);
    for(long long int toss = 0; toss < local_tosses; toss ++){
        double x = 2.0 * ((double)rand() / RAND_MAX) - 1.0; // random num between -1 ~ 1
        double y = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
        double distance_squared = x*x + y*y;
        if (distance_squared <= 1) number_in_circle++;
    }

    // TODO: MPI workers
    if (world_rank > 0){
        int count = 1;
        int dest = 0, tag = 0;
        MPI_Send(&number_in_circle, count, MPI_LONG_LONG, dest, tag, MPI_COMM_WORLD);
    }
    else if (world_rank == 0){
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        int count = 1;
        int tag = 0;

        MPI_Request requests[world_size-1];
        MPI_Status statuses[world_size-1];

        buffers[0] = number_in_circle;
        
        for(int i=1; i<world_size; i++){
            MPI_Irecv(&buffers[i], count, MPI_LONG_LONG, i, tag, MPI_COMM_WORLD, &requests[i-1]);
        }
        MPI_Waitall(world_size-1, requests, statuses);
    }

    if (world_rank == 0){
        // TODO: PI result
        long long int total_in_circle = 0;
        for (int j=0; j<world_size; j++){
            total_in_circle += buffers[j];
        }
        pi_result = 4.0 * total_in_circle/((double)tosses);
        
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
