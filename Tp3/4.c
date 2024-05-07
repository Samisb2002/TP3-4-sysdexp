#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10 // Taille du tampon

typedef struct {
    int id;
    char *message;
} MESS;

MESS T[N]; // Tampon
int id = 0; // Indice d'écriture
int ir = 0; // Indice de lecture

sem_t sprod, scons; // Sémaphores pour contrôler l'accès au tampon

void Produire(MESS *message) {
    message->id = id++;
    char buffer[20];
    sprintf(buffer, "Message %d", message->id);
    message->message = strdup(buffer);
    printf("Producteur : %s\n", message->message);
}

void Consommer(MESS *message) {
    char *ptr = message->message;
    while (*ptr) {
        *ptr = toupper(*ptr);
        ptr++;
    }
    printf("Consommateur : %s\n", message->message);
    free(message->message);
}

void Deposer(MESS *message) {
    T[id % N] = *message;
}

void Retirer(MESS *message) {
    *message = T[ir % N];
}

void *Production(void *arg) {
    while (1) {
        MESS message;
        Produire(&message);
        sem_wait(&sprod);
        Deposer(&message);
        sem_post(&scons);
    }
}

void *Consommation(void *arg) {
    while (1) {
        MESS message;
        sem_wait(&scons);
        Retirer(&message);
        sem_post(&sprod);
        Consommer(&message);
    }
}

int main() {
    sem_init(&sprod, 0, N); // Initialisation du sémaphore sprod à N, le nombre de cases du tampon
    sem_init(&scons, 0, 0); // Initialisation du sémaphore scons à 0, car il n'y a pas encore de message à consommer

    pthread_t prod, cons;
    pthread_create(&prod, NULL, Production, NULL);
    pthread_create(&cons, NULL, Consommation, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
} 
