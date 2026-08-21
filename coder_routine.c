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
	pthread_mutex_lock(&dongle->mutex);
	if (!dongle->is_free)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	if (ahora - dongle->time_liberation < cooldown)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}


int should_stop (t_simulation *sim)
{
	int ok_to_go;
	
	ok_to_go = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	ok_to_go = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (ok_to_go);
}


int	take_dongles(t_person *person)
{
	long long	ahora;
	long long	cooldown;
	int	ok_to_go;

	cooldown = person->sim->config.dongle_cooldown;
	ok_to_go = 0;
	while (ok_to_go == 0)
	{
		ok_to_go = should_stop (person->sim);
		ahora = tiempo_actual_relativo(person->sim);
		pthread_mutex_lock(&person->sim->waiter);
		if (dongle_disponible(person->dongle_left, ahora, cooldown)
			&& dongle_disponible(person->dongle_right, ahora, cooldown))
		{
			person->dongle_left->is_free = 0;
			person->dongle_right->is_free = 0;
			pthread_mutex_unlock(&person->sim->waiter);
			return (0);
		}
		pthread_mutex_unlock(&person->sim->waiter);
		esperar_turno(person->dongle_right);
	}
	return (1);
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


void release_one_dongle(t_dongle *dongle, t_simulation *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_free = 1;
	dongle->time_liberation = tiempo_actual_relativo(sim);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void release_dongles(t_person *person)
{
	release_one_dongle(person->dongle_left, person->sim);
	release_one_dongle(person->dongle_right, person->sim);
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
