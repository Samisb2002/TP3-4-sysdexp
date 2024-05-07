 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables */
int x = 0;
sem_t sync1, sync2;

/* Thread 1 functions */
void a1(){
  printf("a1()\n");
  sleep(1);
}

void a2(){
  printf("a2()\n");
}

void *thread1(void *arg)
{
    /* a1() before b2() */
    a1();
    sem_post(&sync1); // signal that a1() is done
    sem_wait(&sync2); // wait for b2() to finish
    a2();
}

/* Thread 2 functions */
void b1(){
  printf("b1()\n");
  sleep(1);
}

void b2(){
  printf("b2()\n");
}

void *thread2(void *arg)
{
    /* b1() before a2() */
    b1();
    sem_post(&sync1); // signal that b1() is done
    sem_wait(&sync2); // wait for a2() to finish
    b2();
}

int main () {
    pthread_t t1, t2;

    /* initialize semaphores */
    if (sem_init(&sync1, 0, 0) == -1) {
        perror("Could not initialize sync1 semaphore");
        exit(2);
    }
    if (sem_init(&sync2, 0, 0) == -1) {
        perror("Could not initialize sync2 semaphore");
        exit(2);
    }

    /* create threads */
    if (pthread_create(&t1, NULL, thread1, NULL) < 0) {
        perror("Error: thread 1 cannot be created");
        exit(1);
    }
    if (pthread_create(&t2, NULL, thread2, NULL) < 0) {
        perror("Error: thread 2 cannot be created");
        exit(1);
    }

    /* wait for threads to terminate */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /* destroy semaphores */
    sem_destroy(&sync1);
    sem_destroy(&sync2);

    exit(0);
}
