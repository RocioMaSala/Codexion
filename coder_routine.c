/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/08/17 16:54:10 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void esperar_turno(t_dongle *dongle_right)
{
    struct timeval      tv;
    struct timespec     limite;
    long long           nsec;

    gettimeofday(&tv, NULL);
    limite.tv_sec = tv.tv_sec;
    nsec = (tv.tv_usec * 1000) + (5 * 1000000); // tiempo actual en ns + 5ms en ns
    limite.tv_sec += nsec / 1000000000;         // por si nsec se pasa de 1 segundo completo
    limite.tv_nsec = nsec % 1000000000;

    pthread_mutex_lock(&dongle_right->mutex);
    pthread_cond_timedwait(&dongle_right->cond, &dongle_right->mutex, &limite);
    pthread_mutex_unlock(&dongle_right->mutex);
}


long long	tiempo_actual_relativo(t_simulation *sim)
{
	struct timeval	tv;
	long long		ahora_absoluto;

	gettimeofday(&tv, NULL);
	ahora_absoluto = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ahora_absoluto - sim->time_start_sim);
}

int	dongle_disponible(t_dongle *dongle, long long ahora, long long cooldown)
{
	if (!dongle->is_free)
	{
		return (0);
	}
	if (ahora - dongle->time_liberation < cooldown)
	{
		return (0);
	}
	return (1);
}


void	insertar_en_cola(t_queue *queue, t_person *person, long long key)
{
	t_heap_entry tmp;
    
    if (queue->size >= 2)
		return;

	queue->entries[queue->size].person = person;
	queue->entries[queue->size].priority_key = key;
	queue->size++;

	if (queue->size == 2 && queue->entries[1].priority_key < queue->entries[0].priority_key)
	{
		tmp = queue->entries[0];
		queue->entries[0] = queue->entries[1];
		queue->entries[1] = tmp;
	}
}

int	tiene_prioridad(t_queue *queue, t_person *person)
{
	if (queue->size == 0)
		return (1);
	return (queue->entries[0].person->id == person->id);
}


void eliminar_de_cola(t_queue *queue, t_person *person)
{
    if (queue->size == 0)
        return;
    if (queue->entries[0].person->id == person->id)
    {
        if (queue->size == 2)
            queue->entries[0] = queue->entries[1];
        queue->size--;
        return;
    }
    if (queue->size == 2 && queue->entries[1].person->id == person->id)
        queue->size--;
}

long long calcular_priority_key(t_person *person, long long ahora)
{
    long long last_compile;
	
	if (person->sim->config.scheduler == FIFO)
        return (ahora);
	pthread_mutex_lock(&person->state_mutex);
    last_compile = person->last_compile_start;
    pthread_mutex_unlock(&person->state_mutex);
    return (last_compile + person->sim->config.time_to_burnout);
}


int should_stop (t_simulation *sim)
{
	int stop;
	
	stop = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}


int take_dongles(t_person *person)
{
    long long   ahora;
    long long   cooldown;
    int         puedo_izq;
    int         puedo_der;
    int ya_en_cola_izq;
    int ya_en_cola_der;
    struct timespec limite;

    cooldown = person->sim->config.dongle_cooldown;
    ya_en_cola_izq = 0;
    ya_en_cola_der = 0;
    
    pthread_mutex_lock(&person->sim->waiter);
    while (!should_stop(person->sim))
    {
        ahora = tiempo_actual_relativo(person->sim);
        puedo_izq = dongle_disponible(person->dongle_left, ahora, cooldown)
            && tiene_prioridad(&person->dongle_left->waiting_queue, person);
        puedo_der = dongle_disponible(person->dongle_right, ahora, cooldown)
            && tiene_prioridad(&person->dongle_right->waiting_queue, person);
        if (puedo_izq && puedo_der)
        {
            person->dongle_left->is_free = 0;
            person->dongle_right->is_free = 0;
            eliminar_de_cola(&person->dongle_left->waiting_queue, person);
            eliminar_de_cola(&person->dongle_right->waiting_queue, person);
            pthread_mutex_unlock(&person->sim->waiter);
            return (1);
        }
        if (!puedo_izq  && !ya_en_cola_izq)
        {
            insertar_en_cola(&person->dongle_left->waiting_queue, person,
                calcular_priority_key(person, ahora));
            ya_en_cola_izq = 1;
        }
        if (!puedo_der && !ya_en_cola_der)
        {
            insertar_en_cola(&person->dongle_right->waiting_queue, person,
                calcular_priority_key(person, ahora));
            ya_en_cola_der = 1;
            }
        clock_gettime(CLOCK_REALTIME, &limite);
		limite.tv_nsec += 5 * 1000000;
		if (limite.tv_nsec >= 1000000000)
		{
			limite.tv_sec++;
			limite.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait(
			&person->sim->cond,
			&person->sim->waiter,
			&limite);
    }
    pthread_mutex_unlock(&person->sim->waiter);
    return (0);
}


void release_one_dongle(t_dongle *dongle, t_simulation *sim)
{
	dongle->is_free = 1;
	dongle->time_liberation = tiempo_actual_relativo(sim);
}

void release_dongles(t_person *person)
{
    t_simulation *sim;

    sim = person->sim;

    pthread_mutex_lock(&sim->waiter);

    release_one_dongle(person->dongle_left, sim);
    release_one_dongle(person->dongle_right, sim);

    pthread_cond_broadcast(&sim->cond);

    pthread_mutex_unlock(&sim->waiter);
}

void	compile(t_person *person)
{
	pthread_mutex_lock(&person->state_mutex);
	person->actual_state = COMPILING;
	person->last_compile_start = tiempo_actual_relativo(person->sim);
	pthread_mutex_unlock(&person->state_mutex);
	pthread_mutex_lock(&person->sim->log);
	printf("%lld %d is compiling\n", person->last_compile_start, person->id);
	pthread_mutex_unlock(&person->sim->log);
	usleep(person->sim->config.time_to_compile * 1000);
	person->number_of_compilations ++;
}


void	debug(t_person *person)
{
	long long actual_time;
	
	pthread_mutex_lock(&person->state_mutex);
	person->actual_state = DEBUGGING;
	actual_time = tiempo_actual_relativo(person->sim);
	pthread_mutex_unlock(&person->state_mutex);
	pthread_mutex_lock(&person->sim->log);
	printf("%lld %d is debugging\n", actual_time, person->id);
	pthread_mutex_unlock(&person->sim->log);
	usleep(person->sim->config.time_to_debug * 1000);
}

void	refactor(t_person *person)
{
	long long actual_time;
	
	pthread_mutex_lock(&person->state_mutex);
	person->actual_state = REFACTORING;
	actual_time = tiempo_actual_relativo(person->sim);
	pthread_mutex_unlock(&person->state_mutex);
	pthread_mutex_lock(&person->sim->log);
	printf("%lld %d is refactoring\n", actual_time, person->id);
	pthread_mutex_unlock(&person->sim->log);
	usleep(person->sim->config.time_to_refactor * 1000);
}


void	*coder_routine(void *arg)
{
	t_person	*person;

	person = (t_person *)arg;
	while (!should_stop(person->sim))
	{
		if (take_dongles(person) != 0)
		{
			compile(person);
			release_dongles(person);
			if(!should_stop(person->sim))
			{
				debug(person);
				if(!should_stop(person->sim))
					refactor(person);
			}
		}
	}
	return (NULL);
}
