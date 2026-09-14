/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 12:40:36 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	sim->persons = malloc(sizeof(t_person) * sim->config.number_of_coders);
	if (!sim->persons)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->config.number_of_coders);
	if (!sim->dongles)
	{
		cleanup_init_failure(sim, i, 0);
		return (1);
	}
	if (pthread_mutex_init(&sim->waiter, NULL) != 0)
	{
		cleanup_init_failure(sim, i, 1);
		return (1);
	}
	if (pthread_mutex_init(&sim->log, NULL) != 0)
	{
		cleanup_init_failure(sim, i, 2);
		return (1);
	}
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		cleanup_init_failure(sim, i, 3);
		return (1);
	}
	sim->stop_simulation = 0;
	if (pthread_cond_init(&sim->cond, NULL) != 0)
	{
		cleanup_init_failure(sim, i, 4);
		return (1);
	}
	while (i < sim->config.number_of_coders)
	{
		sim->dongles[i].waiting_queue.size = 0;
		sim->dongles[i].time_liberation = -sim->config.dongle_cooldown;
		sim->dongles[i].is_free = 1;
		if (pthread_mutex_init(&sim->persons[i].state_mutex, NULL) != 0)
		{
			cleanup_init_failure(sim, i, 5);
			return (1);
		}
		i++;
	}
	return (0);
}

int	assign_data_persons(t_simulation *sim)
// asignar dongles izq y dcha de cada uno
{
	int i;
	int num;

	num = sim->config.number_of_coders;
	i = 0;
	while (i < num)
	{
		sim->persons[i].id = i + 1;
		sim->persons[i].sim = sim;
		sim->persons[i].last_compile_start = 0;
		sim->persons[i].number_of_compilations = 0;
		sim->persons[i].actual_state = TAKING_A_DONGLE;
		if (num == 1)
		{
			sim->persons[i].dongle_left = &sim->dongles[i];
			sim->persons[i].dongle_right = &sim->dongles[i];
		}
		else
		{
			sim->persons[i].dongle_left = &sim->dongles[i];
			sim->persons[i].dongle_right = &sim->dongles[(i + 1) % num];
		}
		i++;
	}
	return (0);
}

int	create_threads(t_simulation *sim, pthread_t *thread_ids)
// Crear todos los hilos
{
	int i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&thread_ids[i], NULL, coder_routine,
				&sim->persons[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	struct timeval	actual_time;
	long long		timems;
	pthread_t		*thread_ids;
	pthread_t		monitor_id;
	int				i;

	if (parse_args(argc, argv, &sim.config))
		return (1);
	gettimeofday(&actual_time, NULL);
	timems = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000;
	sim.time_start_sim = timems;
	if (init_simulation(&sim))
		return (1);
	assign_data_persons(&sim);
	thread_ids = malloc(sizeof(pthread_t) * sim.config.number_of_coders);
	if (!thread_ids)
	{
		cleanup_simulation(&sim);
		return (1);
	}
	if (create_threads(&sim, thread_ids) != 0)
	{
		free(thread_ids);
		cleanup_simulation(&sim);
		return (1);
	}
	if (pthread_create(&monitor_id, NULL, monitor_routine, &sim) != 0)
	{
		pthread_mutex_lock(&sim.stop_mutex);
		sim.stop_simulation = 1;
		pthread_mutex_unlock(&sim.stop_mutex);
		pthread_mutex_lock(&sim.waiter);
		pthread_cond_broadcast(&sim.cond);
		pthread_mutex_unlock(&sim.waiter);
		i = 0;
		while (i < sim.config.number_of_coders)
		{
			pthread_join(thread_ids[i], NULL);
			i++;
		}
		free(thread_ids);
		cleanup_simulation(&sim);
		return (1);
	}
	i = 0;
	while (i < sim.config.number_of_coders)
	{
		pthread_join(thread_ids[i], NULL);
		i++;
	}
	pthread_join(monitor_id, NULL);
	free(thread_ids);
	cleanup_simulation(&sim);
	return (0);
}
