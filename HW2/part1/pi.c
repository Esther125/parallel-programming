/* Parallel Version of pi.c */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 1.0
#define MIN -1.0

int main(int argc, char** argv){
    if (argc != 3){
        fprintf(stderr, "The correct usage is %s <num_threads> <num_tosses>\n", argv[0]);
        return 1;
    }
    long long int number_of_tosses = atoll(argv[2]);
    long long int number_in_circle = 0;
    long double pi_estimate;

    for (long long int toss = 0; toss < number_of_tosses; toss ++) {
        double x = (MAX - MIN) * drand48() + MIN;
        double y = (MAX - MIN) * drand48() + MIN; 

        double distance_squared = x * x + y * y;
        if ( distance_squared <= 1) number_in_circle++;
    }
    pi_estimate = 4.0L * (long double)number_in_circle /(long double) number_of_tosses;
    printf("%Lf\n", pi_estimate);
    return 0;
}