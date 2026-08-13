#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <stdlib.h>   // malloc, free, atoi
#include <pthread.h>  // todo lo relacionado con hilos, mutex, cond vars
#include <string.h>   // strcmp, strlen
#include <sys/time.h> // gettimeofday
#include <unistd.h>   // usleep, write
#include <limits.h>

typedef enum e_state
{
    TAKING_A_DONGLE,
    COMPILING,
    DEBUGGING,
    REFACTORING,
    BURNED_OUT    
}t_state;

typedef enum e_scheduler
{
    FIFO,
    EDF
}t_scheduler;

typedef struct s_config
{
    int number_of_coders;
    long long time_to_burnout;
    long long time_to_compile;
    long long time_to_debug;
    long long time_to_refactor;
    int number_of_compiles_required;
    long long dongle_cooldown;
    t_scheduler scheduler;
}t_config;

typedef struct s_heap_entry
{
    t_person    *person;
    long long   priority_key;
} t_heap_entry;

typedef struct s_queue
{
    t_heap_entry    entries[2];
    int             size;
} t_queue;

typedef struct s_dongle
{
    int is_free;
    long long time_liberation;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_queue *waiting_queue;
}t_dongle;

typedef struct s_simulation t_simulation;

typedef struct s_person
{
    int id;
    t_dongle *dongle_left;
    t_dongle *dongle_right;
    long long last_compile_start;
    int number_of_compilations;
    t_state actual_state;
    pthread_mutex_t state_mutex;
    t_simulation    *sim;
}t_person;

struct s_simulation
{
    long long time_start_sim;
    t_config config;
    t_person *persons;
    t_dongle *dongles;
    pthread_mutex_t waiter;
    pthread_mutex_t log;
    pthread_cond_t cond;
    pthread_mutex_t stop_mutex;
    int stop_simulation;
};

int parse_args(int argc, char **argv, t_config *config)

#endif

//
