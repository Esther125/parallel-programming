/* Parallel Version of pi.c */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define RNG_GLOBAL_SEED 123456789ULL

typedef struct thread_data{
    long long int tosses;
    long long int hits;
    int tid;
}thread_data;

// ---- Random Helper ----
// 1. seed mixer
static inline uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

// 2. 64-bit 亂數產生器 
static inline uint64_t xorshift64s(uint64_t *s) {
    uint64_t x = *s;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *s = x;
    return x * 0x2545F4914F6CDD1Dull;
}

// 3. 把亂數映射到 [0, 1) 
static inline float u32_to_unit(uint32_t u) {
    return (float)u * (1.0f / 4294967296.0f); // [0,1)
}

void *run(void *threadarg){
    thread_data *my_data = (thread_data *) threadarg;
    long long int hit_count = 0;
    long long int toss_count = my_data->tosses; 
    
    uint64_t seed = RNG_GLOBAL_SEED ^ (0x9E3779B97F4A7C15ULL * (uint64_t)(my_data->tid + 1));
    seed = splitmix64(&seed);

    long long int i = 0;

    for (i = 0; i + 3 < toss_count; i += 4) {
        // 一次產生四個隨機值
        uint64_t r0 = xorshift64s(&seed);
        uint64_t r1 = xorshift64s(&seed);
        uint64_t r2 = xorshift64s(&seed);
        uint64_t r3 = xorshift64s(&seed);

        uint32_t rx0 = (uint32_t)r0;
        uint32_t ry0 = (uint32_t)(r0 >> 32);
        uint32_t rx1 = (uint32_t)r1;
        uint32_t ry1 = (uint32_t)(r1 >> 32);
        uint32_t rx2 = (uint32_t)r2;
        uint32_t ry2 = (uint32_t)(r2 >> 32);
        uint32_t rx3 = (uint32_t)r3;
        uint32_t ry3 = (uint32_t)(r3 >> 32);

        // 映射到 [-1,1)
        float x0 = 2.0f * u32_to_unit(rx0) - 1.0f;
        float y0 = 2.0f * u32_to_unit(ry0) - 1.0f;
        float x1 = 2.0f * u32_to_unit(rx1) - 1.0f;
        float y1 = 2.0f * u32_to_unit(ry1) - 1.0f;
        float x2 = 2.0f * u32_to_unit(rx2) - 1.0f;
        float y2 = 2.0f * u32_to_unit(ry2) - 1.0f;
        float x3 = 2.0f * u32_to_unit(rx3) - 1.0f;
        float y3 = 2.0f * u32_to_unit(ry3) - 1.0f;

        // 距離平方 <= 1 擊中
        float d2_0 = x0*x0 + y0*y0;
        float d2_1 = x1*x1 + y1*y1;
        float d2_2 = x2*x2 + y2*y2;
        float d2_3 = x3*x3 + y3*y3;

        if (d2_0 <= 1.0f) { hit_count++; }
        if (d2_1 <= 1.0f) { hit_count++; }
        if (d2_2 <= 1.0f) { hit_count++; }
        if (d2_3 <= 1.0f) { hit_count++; }
    }

    // tail：收掉剩餘不到 4 次的擲點
    while (i < toss_count) {
        uint64_t r = xorshift64s(&seed);
        uint32_t rx = (uint32_t)r;
        uint32_t ry = (uint32_t)(r >> 32);
        float x = 2.0f * u32_to_unit(rx) - 1.0f;
        float y = 2.0f * u32_to_unit(ry) - 1.0f;
        float d2 = x * x + y * y;
        if (d2 <= 1.0f) hit_count++;
        i++;
    }
    my_data->hits = hit_count;
    return NULL;
}

int main(int argc, char** argv){
    if (argc != 3) return 1;

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
        // Allocate tosses
        thread_data_arr[i].tosses = base + (i < remainder ? 1 : 0);        
        thread_data_arr[i].hits = 0;
        thread_data_arr[i].tid = i;   
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