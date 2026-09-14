/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_release_dongles_outils.c             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 13:34:03 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_one_dongle(t_dongle *dongle, t_simulation *sim)
{
	dongle->is_free = 1;
	dongle->time_liberation = tiempo_actual_relativo(sim);
}

void	release_dongles(t_person *person)
{
	t_simulation	*sim;

	sim = person->sim;
	pthread_mutex_lock(&sim->waiter);
	release_one_dongle(person->dongle_left, sim);
	release_one_dongle(person->dongle_right, sim);
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->waiter);
}
