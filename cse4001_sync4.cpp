#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <stdio.h>

using namespace std;

class Semaphore {
public:
        Semaphore(int initialValue) {
                sem_init(&sem, 0, initialValue);
        }
        ~Semaphore() {
                sem_destroy(&sem);
        }
        void wait() {
                sem_wait(&sem);
        }
        void signal() {
                sem_post(&sem);
        }
private:
        sem_t sem;
};

const int NUM_PHILOSOPHERS = 5;

Semaphore forks[NUM_PHILOSOPHERS] = {
        Semaphore(1), Semaphore(1), Semaphore(1),
        Semaphore(1), Semaphore(1)
};

int left(int i)  { return i; }
int right(int i) { return (i + 1) % NUM_PHILOSOPHERS; }

void think(int i) {
        printf("Philosopher %d is thinking.\n", i);
        fflush(stdout);
        sleep(1);
}

void eat(int i) {
        printf("Philosopher %d is eating.\n", i);
        fflush(stdout);
        sleep(2);
}

void get_forks(int i)
{
        if (i % 2 == 0) {
                forks[left(i)].wait();
                forks[right(i)].wait();
        } else {
                forks[right(i)].wait();
                forks[left(i)].wait();
        }
        printf("Philosopher %d picked up forks %d and %d.\n", i, left(i), right(i));
        fflush(stdout);
}

void put_forks(int i)
{
        forks[left(i)].signal();
        forks[right(i)].signal();
        printf("Philosopher %d put down forks %d and %d.\n", i, left(i), right(i));
        fflush(stdout);
}

void* philosopher(void* arg)
{
        int i = (long)arg;
        while (true) {
                think(i);
                get_forks(i);
                eat(i);
                put_forks(i);
        }
        return NULL;
}

int main()
{
        pthread_t threads[NUM_PHILOSOPHERS];
        for (long i = 0; i < NUM_PHILOSOPHERS; i++) {
                pthread_create(&threads[i], NULL, philosopher, (void*)i);
        }
        pthread_exit(NULL);
}
