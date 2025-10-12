/* Parallel Version of pi.c */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 1.0
#define MIN -1.0

typedef struct thread_data{
    long long int tosses;
    long long int hits;
}thread_data;

void *run(void *threadarg){
    thread_data *my_data = (thread_data *) threadarg;
    long long int hit_count = 0;
    long long int toss_count = my_data->tosses; 
    unsigned int seed = 0;

    for (long long int toss = 0; toss < toss_count; toss++) {
        double x = (double)rand_r(&seed) / (RAND_MAX + 1.0);
        double y = (double)rand_r(&seed) / (RAND_MAX + 1.0);

        double distance_squared = x*x + y*y;
        if ( distance_squared <= 1.0) hit_count++;
    }
    my_data->hits = hit_count;
    return NULL;
}

int main(int argc, char** argv){
    if (argc != 3){
        fprintf(stderr, "The correct usage is %s <num_threads> <num_tosses>\n", argv[0]);
        return 1;
    }

    int thread_num = atoi(argv[1]);
    pthread_t threads[thread_num];
    thread_data thread_data_arr[thread_num];

    long long int total_tosses = atoll(argv[2]);
    long long int total_hits = 0;
    double pi_estimate;

    long long base = total_tosses / thread_num;
    long long remainder = total_tosses % thread_num;

    // Create threads 
    for (int i=0; i < thread_num; i++){
        // printf("In main: creating thread %d\n", i);
        // Allocate tosses
        thread_data_arr[i].tosses = (i != thread_num-1) ? base: base+remainder;

        int failed = pthread_create(&threads[i], NULL, run,  (void *)&thread_data_arr[i]);
        if (failed) fprintf(stderr, "Error code from create_thread: %d\n", failed);
    }

    // Join all threads
    for (int j=0; j<thread_num; j++){
        pthread_join(threads[j], NULL);
    }

    // Estimate pi
    for (int k=0; k<thread_num; k++){
        total_hits += thread_data_arr[k].hits;
    }
    pi_estimate = 4.0 * (double)total_hits /(double) total_tosses;
    printf("%f\n", pi_estimate);

    return 0;
}