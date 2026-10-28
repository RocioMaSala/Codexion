/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_taking_dongles.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/28 17:34:42 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static	void	take_both(t_person *p)
{
	p->dongle_left->is_free = 0;
	p->dongle_right->is_free = 0;
	eliminar_de_cola(&p->dongle_left->waiting_queue, p);
	eliminar_de_cola(&p->dongle_right->waiting_queue, p);
}

static	void	enqueue_once(t_person *p, t_dongle *d, int *ya, long long ahora)
{
	if (*ya)
		return ;
	insertar_en_cola(&d->waiting_queue, p, calcular_priority_key(p, ahora));
	*ya = 1;
}

static void	wait_turn(t_simulation *sim)
{
	struct timespec	limite;
	struct timeval	now;

	gettimeofday(&now, NULL);
	limite.tv_sec = now.tv_sec;
	limite.tv_nsec = now.tv_usec * 1000 + 5 * 1000000;
	if (limite.tv_nsec >= 1000000000)
	{
		limite.tv_sec++;
		limite.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&sim->cond, &sim->waiter, &limite);
}

static int	try_take(t_person *p, long long ahora, int *ya)
{
	int	izq;
	int	der;

	izq = can_take(p, p->dongle_left, ahora);
	der = can_take(p, p->dongle_right, ahora);
	if (izq && der && p->dongle_left != p->dongle_right)
	{
		take_both(p);
		return (1);
	}
	if (!izq)
		enqueue_once(p, p->dongle_left, &ya[0], ahora);
	if (!der)
		enqueue_once(p, p->dongle_right, &ya[1], ahora);
	return (0);
}

int	take_dongles(t_person *person)
{
	int	ya[2];

	ya[0] = 0;
	ya[1] = 0;
	pthread_mutex_lock(&person->sim->waiter);
	while (!should_stop(person->sim))
	{
		if (try_take(person, tiempo_actual_relativo(person->sim), ya))
		{
			pthread_mutex_unlock(&person->sim->waiter);
			print_status(person, "has taken a dongle");
			print_status(person, "has taken a dongle");
			return (1);
		}
		wait_turn(person->sim);
	}
	pthread_mutex_unlock(&person->sim->waiter);
	return (0);
}
