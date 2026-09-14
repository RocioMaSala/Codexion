/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_taking_dongles.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 13:36:16 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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


int	tiene_prioridad(t_queue *queue, t_person *person)
{
	if (queue->size == 0)
		return (1);
	return (queue->entries[0].person->id == person->id);
}

int	take_dongles(t_person *person)
{
	long long		ahora;
	long long		cooldown;
	int				puedo_izq;
	int				puedo_der;
	int				ya_en_cola_izq;
	int				ya_en_cola_der;
	struct timespec	limite;

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
		if (!puedo_izq && !ya_en_cola_izq)
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
		pthread_cond_timedwait(&person->sim->cond, &person->sim->waiter,
			&limite);
	}
	pthread_mutex_unlock(&person->sim->waiter);
	return (0);
}