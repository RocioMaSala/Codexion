/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/26 17:07:54 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_start_time(t_simulation *sim)
{
	struct timeval	actual_time;

	gettimeofday(&actual_time, NULL);
	sim->time_start_sim = actual_time.tv_sec * 1000000
		+ actual_time.tv_usec;
}

static void	join_coders(t_simulation *sim, pthread_t *thread_ids)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(thread_ids[i], NULL);
		i++;
	}
}

static void	stop_all(t_simulation *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop_simulation = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_lock(&sim->waiter);
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->waiter);
}

static int	run(t_simulation *sim, pthread_t *thread_ids)
{
	pthread_t	monitor_id;

	set_start_time(sim);
	if (create_threads(sim, thread_ids) != 0)
		return (1);
	if (pthread_create(&monitor_id, NULL, monitor_routine, sim) != 0)
	{
		stop_all(sim);
		join_coders(sim, thread_ids);
		return (1);
	}
	join_coders(sim, thread_ids);
	pthread_join(monitor_id, NULL);
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		*thread_ids;
	int				ret;

	if (parse_args(argc, argv, &sim.config))
		return (1);
	if (init_simulation(&sim))
		return (1);
	assign_data_persons(&sim);
	thread_ids = malloc(sizeof(pthread_t) * sim.config.number_of_coders);
	if (!thread_ids)
	{
		cleanup_simulation(&sim);
		return (1);
	}
	ret = run(&sim, thread_ids);
	free(thread_ids);
	cleanup_simulation(&sim);
	return (ret);
}
