#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define NUM_CARS 10
#define NUM_SPOTS 3

sem_t parking_semaphore;

pthread_mutex_t draw_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

int parking_spots[NUM_SPOTS];
int total_cars_parked = 0;
double total_wait_time = 0.0;

void draw_parking()
{
    pthread_mutex_lock(&draw_mutex);

    /*
    if (system("clear") == -1)
    {
        perror("system");
        exit(EXIT_FAILURE);
    }
    */

    printf("\n=== Parking Lot ===\n");
    for (int i = 0; i < NUM_SPOTS; i++)
    {
        if (parking_spots[i] == 0)
            printf("[   ] ");
        else
            printf("[C%2d] ", parking_spots[i]);
    }
    printf("\n===================\n");

    pthread_mutex_unlock(&draw_mutex);
}

void log_event(int car_id, const char *message)
{
    pthread_mutex_lock(&log_mutex);

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[24] = '\0';
    printf("[%s] Car %d: %s\n", timestamp, car_id, message);

    pthread_mutex_unlock(&log_mutex);
}

void *car_thread(void *arg)
{
    int car_id = *(int *)arg;
    free(arg);

    // Simulate arrival
    struct timespec arrival;
    clock_gettime(CLOCK_MONOTONIC, &arrival);

    log_event(car_id, "arrived at the parking lot");

    sem_wait(&parking_semaphore);

    // Simulate parking
    struct timespec parked;
    clock_gettime(CLOCK_MONOTONIC, &parked);
    double wait_time = (parked.tv_sec - arrival.tv_sec) + (parked.tv_nsec - arrival.tv_nsec) / 1e9;

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "parked (waited %.3f seconds)", wait_time);
    log_event(car_id, buffer);

    for (int i = 0; i < NUM_SPOTS; i++)
    {
        if (parking_spots[i] == 0)
        {
            parking_spots[i] = car_id;
            break;
        }
    }
    draw_parking();

    // Simulate leaving after 1-5 seconds
    int park_duration = (rand() % 5) + 1;
    sleep(park_duration);

    snprintf(buffer, sizeof(buffer), "leaving after %d seconds", park_duration);
    log_event(car_id, buffer);

    sem_post(&parking_semaphore);

    for (int i = 0; i < NUM_SPOTS; i++)
    {
        if (parking_spots[i] == car_id)
        {
            parking_spots[i] = 0;
            break;
        }
    }
    draw_parking();

    // Update statistics
    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_time;
    pthread_mutex_unlock(&stats_mutex);

    return NULL;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    if (sem_init(&parking_semaphore, 0, NUM_SPOTS) != 0)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_CARS];

    for (int i = 0; i < NUM_CARS; i++)
    {
        int *id = malloc(sizeof(int));
        if (!id)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        *id = i + 1;
        pthread_create(&threads[i], NULL, car_thread, id);
    }

    for (int i = 0; i < NUM_CARS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Print statistics
    printf("Total cars parked : %d\n", total_cars_parked);
    printf("Average wait time : %.3f s\n",
           total_cars_parked > 0 ? total_wait_time / total_cars_parked : 0.0);

    if (sem_destroy(&parking_semaphore) != 0)
    {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }

    return 0;
}
