/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/26 17:55:43 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	assign_data_persons(t_simulation *sim)
// asignar dongles izq y dcha de cada uno
{
	int		i;
	int		num;

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
	int		i;

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

static int	init_sync(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->waiter, NULL) != 0)
	{
		cleanup_init_failure(sim, 0, 1);
		return (1);
	}
	if (pthread_mutex_init(&sim->log, NULL) != 0)
	{
		cleanup_init_failure(sim, 0, 2);
		return (1);
	}
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		cleanup_init_failure(sim, 0, 3);
		return (1);
	}
	sim->stop_simulation = 0;
	if (pthread_cond_init(&sim->cond, NULL) != 0)
	{
		cleanup_init_failure(sim, 0, 4);
		return (1);
	}
	return (0);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
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

int	init_simulation(t_simulation *sim)
{
	sim->persons = malloc(sizeof(t_person) * sim->config.number_of_coders);
	if (!sim->persons)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->config.number_of_coders);
	if (!sim->dongles)
	{
		cleanup_init_failure(sim, 0, 0);
		return (1);
	}
	if (init_sync(sim))
		return (1);
	return (init_dongles(sim));
}
