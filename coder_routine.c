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

void	take_dongles(t_person *person)
{
	long long	ahora;
	long long	cooldown;

	cooldown = person->sim->config.dongle_cooldown;
	while (1)
	{
		ahora = tiempo_actual_relativo(person->sim);
		pthread_mutex_lock(&person->sim->waiter);
		if (dongle_disponible(person->dongle_left, ahora, cooldown)
			&& dongle_disponible(person->dongle_right, ahora, cooldown))
		{
			person->dongle_left->is_free = 0;
			person->dongle_right->is_free = 0;
			pthread_mutex_unlock(&person->sim->waiter);
			break ;
		}
		pthread_mutex_unlock(&person->sim->waiter);
		pthread_cond_timedwait();
	}
}

void	*coder_routine(void *arg)
{
	t_person	*person;

	person = (t_person *)arg;
	while (!simulation_finished(person->sim))
	{
		take_dongles(person);
		compile(person);
		release_dongles(person);
		debug(person);
		refactor(person);
	}
	return (NULL);
}
