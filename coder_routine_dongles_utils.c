/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_dongles_utils.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/26 17:55:15 by romarti2         ###   ########.fr       */
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

int	can_take(t_person *p, t_dongle *d, long long ahora)
{
	return (dongle_disponible(d, ahora, p->sim->config.dongle_cooldown)
		&& tiene_prioridad(&d->waiting_queue, p));
}
