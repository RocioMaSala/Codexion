#include "codexion.h"

void cleanup_simulation(t_simulation *sim)
{
    pthread_mutex_destroy(&sim->waiter);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log);
    pthread_cond_destroy(&sim->cond);
    pthread_mutex_destroy(&sim->persons->state_mutex);
    pthread_cond_destroy(&sim->dongles->cond);
    pthread_mutex_destroy(&sim->dongles->mutex);
	free(sim->persons);
	free(sim->dongles);
}

