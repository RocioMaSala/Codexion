/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_stop_simulation.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/26 17:07:34 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	should_stop(t_simulation *sim)
{
	int	stop;

	stop = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}
