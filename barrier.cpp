// barrier.cpp

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h> 

using namespace std;

void *highway(void *N);

class Barrier {
public:
    Barrier(int n) {
        pthread_mutex_init(&mutex ,NULL);
        pthread_cond_init(&condvar, NULL);
        capacity = n;
    };

    ~Barrier() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condvar);
    };

    void wait() { // do all work in class to keep things encapsulated

        pthread_mutex_lock(&mutex);
        
        while(!atCapacity) {
            
            printf("Waiting for barrier... (%ld) (counter: %d)\n", pthread_self(), car_counter);
            sleep(1);
            pthread_cond_wait(&condvar, &mutex);
            
        }
        pthread_mutex_unlock(&mutex);

        printf("\nResetting barrier... \n");
        sleep(1);
        atCapacity = false;
    }

    void counter() { // I created another method so that when the threads lock up at wait, 
                     // broadcast in the other method wakes them up

        pthread_mutex_lock(&mutex);
        car_counter++;
        if (car_counter == capacity) {
            printf("Barrier Released! (%ld)\n", pthread_self());
            sleep(1);
            atCapacity = true; // releases the while loop in wait()
            car_counter = 0; // Placed between mutexes for thread safety.
            
            pthread_cond_broadcast(&condvar);
        }
        pthread_mutex_unlock(&mutex);
    }

private:
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
    int car_counter = 0;
    bool atCapacity = false;
    int capacity;
};

Barrier barrier(5);

int main() {

    int N = 5;

    pthread_t tid[N];     // The thread identifier
    pthread_attr_t attr;  // Set of thread attributes

    pthread_attr_init(&attr);
    

    for (int i = 0; i < N; i++) {
        pthread_create(&tid[i], &attr, highway,  NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_attr_destroy(&attr);
    barrier.~Barrier();
    return 0;
}

void *highway(void* ptr) { // Proxy function so that I can call my class methods more easily.

    for (int i = 0; i < 5; i++) {
        barrier.counter();
        barrier.wait();
    }
    return 0;
}