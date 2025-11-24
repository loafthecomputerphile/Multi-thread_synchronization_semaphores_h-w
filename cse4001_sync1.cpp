#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

/* Simple semaphore wrapper */
class Semaphore {
public:
        Semaphore(int initialValue) {
                sem_init(&mSemaphore, 0, initialValue);
        }
        ~Semaphore() {
                sem_destroy(&mSemaphore);
        }

        void wait() {
                sem_wait(&mSemaphore);
        }
        void signal() {
                sem_post(&mSemaphore);
        }
private:
        sem_t mSemaphore;
};

const int NUM_READERS = 5;
const int NUM_WRITERS = 5;

/* Shared counter */
int readCount = 0;


Semaphore mutexSem(1);         // protects readCount
Semaphore roomEmpty(1);        // writers get exclusive access
Semaphore turnstile(1);        // blocks readers when writer arrives


void* Reader(void* threadID)
{
        long id = (long)threadID;

        while (1)
        {
                
                // Readers must pass the turnstile but do not keep it
                turnstile.wait();
                turnstile.signal();

                
                mutexSem.wait();
                readCount++;
                if (readCount == 1) {
                        roomEmpty.wait(); // first reader blocks writers
                }
                mutexSem.signal();

                printf("Reader %ld is reading.\n", id);
                fflush(stdout);
                sleep(1);  // simulate reading

                
                mutexSem.wait();
                readCount--;
                if (readCount == 0) {
                        roomEmpty.signal(); // last reader frees room for writers
                }
                mutexSem.signal();

                sleep(1);
        }
}



void* Writer(void* threadID)
{
        long id = (long)threadID;

        while (1)
        {
                
                turnstile.wait();

                
                roomEmpty.wait();

                printf("Writer %ld is writing!\n", id);
                fflush(stdout);
                sleep(2); // writing work simulation

                
                roomEmpty.signal();
                turnstile.signal();

                sleep(2);
        }
}


int main(int argc, char** argv)
{
        pthread_t readers[NUM_READERS];
        pthread_t writers[NUM_WRITERS];

        // Create readers
        for (long r = 1; r <= NUM_READERS; r++) {
                int rc = pthread_create(&readers[r-1], NULL, Reader, (void*)r);
                if (rc) {
                        printf("Error creating reader %ld\n", r);
                        exit(1);
                }
        }

        // Create writers
        for (long w = 1; w <= NUM_WRITERS; w++) {
                int rc = pthread_create(&writers[w-1], NULL, Writer, (void*)w);
                if (rc) {
                        printf("Error creating writer %ld\n", w);
                        exit(1);
                }
        }

        printf("Main thread initialized readers and writers.\n");

        pthread_exit(NULL);
}
