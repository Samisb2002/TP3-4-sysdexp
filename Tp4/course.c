#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_TOURS 10

typedef struct {
    int id;
    int tours;
} Voiture;

pthread_barrier_t depart;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int voitures_en_course;
Voiture* classement[100];
int index_classement = 0;

void* voiture_routine(void* arg) {
    Voiture* voiture = (Voiture*) arg;
    // Attente du signal de départ
    pthread_barrier_wait(&depart);

    for (int i = 0; i < NUM_TOURS; i++) {
        usleep(rand() % 1000000);  // Simulation du temps aléatoire par tour

        pthread_mutex_lock(&mutex);
        voiture->tours++;
        printf("Voiture %d a terminé le tour %d\n", voiture->id, voiture->tours);
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    voitures_en_course--;
    classement[index_classement++] = voiture;
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* arbitre_routine(void* arg) {
    // Attente de toutes les voitures au départ
    pthread_barrier_wait(&depart);
    printf("Le départ est donné!\n");

    while (voitures_en_course > 0) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&condition, &mutex);
        pthread_mutex_unlock(&mutex);
    }

    printf("Classement final :\n");
    for (int i = 0; i < index_classement; i++) {
        printf("%d. Voiture %d\n", i + 1, classement[i]->id);
    }

    return NULL;
}

int main() {
    srand(time(NULL));
    int num_voitures;
    printf("Entrez le nombre de voitures : ");
    scanf("%d", &num_voitures);

    voitures_en_course = num_voitures;

    // Initialisation de la barrière de synchronisation
    pthread_barrier_init(&depart, NULL, num_voitures + 1);

    pthread_t voitures[num_voitures];
    Voiture voiture_data[num_voitures];

    // Création des threads voitures
    for (int i = 0; i < num_voitures; i++) {
        voiture_data[i].id = i + 1;
        voiture_data[i].tours = 0;
        pthread_create(&voitures[i], NULL, voiture_routine, &voiture_data[i]);
    }

    // Création du thread arbitre
    pthread_t arbitre;
    pthread_create(&arbitre, NULL, arbitre_routine, NULL);

    // Attente de la fin de tous les threads
    for (int i = 0; i < num_voitures; i++) {
        pthread_join(voitures[i], NULL);
    }

    pthread_join(arbitre, NULL);
    pthread_barrier_destroy(&depart);

    return 0;
}
