#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define K 10 // Buffer size
#define N 5  // Number of consumers
#define M 2  // Number of producers (Added this!)

int buffer[K];
int read_count[K]; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

// Producer logic
void producer_logic(int item, int idx) {
    pthread_mutex_lock(&mutex);
    while (read_count[idx] < N) {
        pthread_cond_wait(&cond_producer, &mutex);
    }
    buffer[idx] = item;
    read_count[idx] = 0; 
    pthread_cond_broadcast(&cond_consumer); 
    pthread_mutex_unlock(&mutex);
}

// Consumer logic
void consumer_logic(int idx) {
    pthread_mutex_lock(&mutex);
    while (read_count[idx] >= N) {
        pthread_cond_wait(&cond_consumer, &mutex);
    }
    read_count[idx]++;
    if (read_count[idx] == N) {
        pthread_cond_signal(&cond_producer);
    }
    pthread_mutex_unlock(&mutex);
}

// Thread Wrapper Functions
void* producer_thread_func(void* arg) {
    for(int i = 0; i < 5; i++) { // Produce 5 items
        producer_logic(i, i % K);
    }
    return NULL;
}

void* consumer_thread_func(void* arg) {
    for(int i = 0; i < 5; i++) { // Consume 5 items
        consumer_logic(i % K);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // INITIALIZATION LOOP TO PREVENT DEADLOCK
    for (int i = 0; i < K; i++) {
	read_count[i] = N;
    }

    pthread_t producers[M];
    pthread_t consumers[N];

    // Create threads
    for (int i = 0; i < M; i++) pthread_create(&producers[i], NULL, producer_thread_func, NULL);
    for (int i = 0; i < N; i++) pthread_create(&consumers[i], NULL, consumer_thread_func, NULL);

    // Join threads
    for (int i = 0; i < M; i++) pthread_join(producers[i], NULL);
    for (int i = 0; i < N; i++) pthread_join(consumers[i], NULL);

    printf("Processing Complete!\n");
    return 0;
}
