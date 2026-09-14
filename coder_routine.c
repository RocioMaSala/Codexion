/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 12:39:44 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	tiempo_actual_relativo(t_simulation *sim)
{
	struct timeval	tv;
	long long		ahora_absoluto;

	gettimeofday(&tv, NULL);
	ahora_absoluto = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ahora_absoluto - sim->time_start_sim);
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
	person->number_of_compilations++;
}

void	debug(t_person *person)
{
	long long	actual_time;

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
	long long	actual_time;

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
			if (!should_stop(person->sim))
			{
				debug(person);
				if (!should_stop(person->sim))
					refactor(person);
			}
		}
	}
	return (NULL);
}
