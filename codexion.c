#include "codexion.h"

int init_simulation(t_simulation *sim)
{
	sim.time_start_sim = timems;
	sim.persons = malloc(sizeof(t_person) * sim.config.number_of_coders);
	if(!sim.persons)
		return(1);
	sim.dongles = malloc(sizeof(t_dongle) * sim.config.number_of_coders);
	if(!sim.dongles)
	{
		free (sim.persons);
		return(1);
	}
	if(pthread_mutex_init(&sim.waiter, NULL) != 0)
		return(1);
	if(pthread_mutex_init(&sim.log, NULL) != 0)
		return(1);
	if(pthread_mutex_init(&sim.stop_mutex, NULL) != 0)
		return(1);
	if(pthread_mutex_init(&sim->persons->state_mutex, NULL) != 0)
		return(1);
	if(pthread_mutex_init(&sim->dongles->mutex, NULL) != 0)
		return(1);
	return(0)
}


int assign_neighbors(t_simulation *sim) // asignar dongles izq y dcha de cada uno


int create_threads(t_simulation *sim, pthread_t *thread_ids) //Crear todos los hilos



int main(int argc, char **argv)
{
	t_simulation sim;
	struct timeval actual_time;
	long long timems;

	if(parse_args(argc, argv, &sim.config))
        return (1);
	gettimeofday(&actual_time, NULL);
	timems = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000;
	init_simulation(sim)
	return(0);
}


