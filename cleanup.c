/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:32:41 by romarti2          #+#    #+#             */
/*   Updated: 2026/08/17 16:46:30 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_init_failure(t_simulation *sim, int hasta, int stage)
{
	int	i;

	i = 0;
	if (stage >= 0)
		free(sim->persons);
	if (stage >= 1)
		free(sim->dongles);
	if (stage >= 2)
		pthread_mutex_destroy(&sim->waiter);
	if (stage >= 3)
		pthread_mutex_destroy(&sim->log);
	if (stage >= 4)
		pthread_mutex_destroy(&sim->stop_mutex);
	if (stage >= 5)
		pthread_cond_destroy(&sim->cond);
	if (stage >= 6)
	{
		while (i < hasta)
		{
			pthread_mutex_destroy(&sim->persons[i].state_mutex);
			pthread_cond_destroy(&sim->dongles[i].cond);
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			i++;
		}
	}
}

void	cleanup_simulation(t_simulation *sim)
{
	int i;

	i = 0;
	pthread_mutex_destroy(&sim->waiter);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log);
	pthread_cond_destroy(&sim->cond);
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->persons[i].state_mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->persons);
	free(sim->dongles);
}
