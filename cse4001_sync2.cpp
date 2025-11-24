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


int readCount = 0;
int writeCount = 0;

Semaphore mutex(1);           // protects readCount
Semaphore rwMutex(1);         // actual critical section (read or write)
Semaphore wrtMutex(1);        // protects writeCount
Semaphore queue(1);           // prevents readers from bypassing waiting writers

const int numReaders = 5;
const int numWriters = 3;


void* Reader(void* idPtr)
{
        int id = (long)idPtr;

        while (1)
        {
                sleep(1);

                // ENTRY
                queue.wait();           // wait behind any writers waiting
                mutex.wait();
                readCount++;
                if (readCount == 1)
                        rwMutex.wait(); // first reader locks writers
                mutex.signal();
                queue.signal();

                // CRITICAL SECTION
                printf("Reader %d reading (readCount=%d)\n", id, readCount);
                fflush(stdout);
                sleep(2);

                // EXIT
                mutex.wait();
                readCount--;
                if (readCount == 0)
                        rwMutex.signal();   // last reader frees writers
                mutex.signal();
        }
        return NULL;
}


void* Writer(void* idPtr)
{
        int id = (long)idPtr;

        while (1)
        {
                sleep(3);

                // ENTRY
                wrtMutex.wait();
                writeCount++;
                if (writeCount == 1)
                        queue.wait();           // block new readers
                wrtMutex.signal();

                rwMutex.wait();                 // exclusive access

                // CRITICAL SECTION
                printf("Writer %d writing...\n", id);
                fflush(stdout);
                sleep(3);

                // EXIT
                rwMutex.signal();

                wrtMutex.wait();
                writeCount--;
                if (writeCount == 0)
                        queue.signal();         // allow readers again
                wrtMutex.signal();
        }
        return NULL;
}


int main(int argc, char** argv)
{
        pthread_t r[numReaders];
        pthread_t w[numWriters];

        for (long i=0; i<numReaders; i++){
                pthread_create(&r[i], NULL, Reader, (void*)(i+1));
        }
                

        for (long i=0; i<numWriters; i++){
                pthread_create(&w[i], NULL, Writer, (void*)(i+1));
        }

	
        printf("Main: writer-priority threads started.\n");
        pthread_exit(NULL);
}

