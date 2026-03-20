#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define NUM_CARS 10
#define NUM_SPOTS 3
#define MIN_TIME_PER_EVENT 1
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 200

sem_t parking_semaphore;

pthread_mutex_t draw_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

int parking_spots[NUM_SPOTS];
int total_cars_parked = 0;
double total_wait_time = 0.0;

void draw_text(SDL_Renderer *renderer, const char *text, int font_size, int x, int y)
{
    TTF_Font *font = TTF_OpenFont("./fonts/Roboto-VariableFont_wdth,wght.ttf", font_size);
    if (!font)
    {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return;
    }

    SDL_Color background = {0, 0, 0, 255}; // black
    SDL_Color fill = {255, 255, 255, 255}; // white

    SDL_Surface *text_surface = TTF_RenderText_Shaded(font, text, fill, background);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_Rect text_rect = {x, y, text_surface->w, text_surface->h};

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    TTF_CloseFont(font);
}

void draw_parking(SDL_Renderer *renderer)
{
    pthread_mutex_lock(&draw_mutex);

    char text[64];

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black background
    SDL_RenderClear(renderer);

    for (int i = 0; i < NUM_SPOTS; i++)
    {
        SDL_Rect rect = {50 + i * 100, 50, 80, 80};

        snprintf(text, sizeof(text), "Spot %i", i);
        draw_text(renderer, text, 24, rect.x + 5, rect.y - 40);

        if (parking_spots[i] == 0)
        {
            SDL_SetRenderDrawColor(renderer, 102, 153, 0, 255); // empty = green
            SDL_RenderFillRect(renderer, &rect);
            draw_text(renderer, "Empty", 12, rect.x + 20, rect.y + 80);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 204, 0, 0, 255); // occupied = red
            SDL_RenderFillRect(renderer, &rect);
            snprintf(text, sizeof(text), "Car ID: %i", parking_spots[i]);
            draw_text(renderer, text, 12, rect.x + 18, rect.y + 30);
            draw_text(renderer, "Occupied", 12, rect.x + 15, rect.y + 80);
        }
    }

    SDL_RenderPresent(renderer);

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
    // This was added, because otherwise the events would be hard to notice in the gui
    sleep(MIN_TIME_PER_EVENT);

    // Simulate waiting for a spot
    sem_wait(&parking_semaphore);

    // This was added, because otherwise the events would be hard to notice in the gui
    sleep(MIN_TIME_PER_EVENT);

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

    // Initialize semaphore
    if (sem_init(&parking_semaphore, 0, NUM_SPOTS) != 0)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Parking Lot",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Initialize SDL_ttf
    TTF_Init();

    // Initialize cars (threads)
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

    // Main loop to handle SDL events and redraw parking lot
    SDL_Event event;
    int running = 1;
    while (running)
    {
        draw_parking(renderer);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
                break;
            }
        }
    }

    // Wait for all cars to finish
    for (int i = 0; i < NUM_CARS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Print statistics
    printf("Total cars parked : %d\n", total_cars_parked);
    printf("Average wait time : %.3f s\n",
           total_cars_parked > 0 ? total_wait_time / total_cars_parked : 0.0);

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    if (sem_destroy(&parking_semaphore) != 0)
    {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }

    return 0;
}
