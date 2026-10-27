/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_status.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/27 17:12:32 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_person *person, char *msg)
{
	t_simulation	*sim;

	sim = person->sim;
	pthread_mutex_lock(&sim->stop_mutex);
	if (!sim->stop_simulation)
	{
		pthread_mutex_lock(&sim->log);
		printf("%lld %d %s\n", tiempo_actual_relativo(sim), person->id, msg);
		pthread_mutex_unlock(&sim->log);
	}
	pthread_mutex_unlock(&sim->stop_mutex);
}
