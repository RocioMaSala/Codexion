

typedef enum e_state
{
    TAKING_A_DONGLE,
    COMPILING,
    DEBUGGING,
    REFACTORING,
    BURNED_OUT    
}t_state;

typedef struct s_dongle
{
    int is_free;
    long long time_liberation;
    pthread_mutex_t mutex;
    t_queue *waiting_queue;
}t_dongle;

typedef struct s_person
{
    int id;
    t_dongle *dongle_left;
    t_dongle *dongle_right;
    long long last_compile_start;
    int number_of_compilations;
    t_state actual_state;
    pthread_mutex_t state_mutex;
}t_person;

typedef struct s_sim
{
    long long time_start_sim;


}