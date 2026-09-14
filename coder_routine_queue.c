/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_queue.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 13:36:22 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	insertar_en_cola(t_queue *queue, t_person *person, long long key)
{
	t_heap_entry	tmp;

	if (queue->size >= 2)
		return ;
	queue->entries[queue->size].person = person;
	queue->entries[queue->size].priority_key = key;
	queue->size++;
	if (queue->size == 2
		&& queue->entries[1].priority_key < queue->entries[0].priority_key)
	{
		tmp = queue->entries[0];
		queue->entries[0] = queue->entries[1];
		queue->entries[1] = tmp;
	}
}

void	eliminar_de_cola(t_queue *queue, t_person *person)
{
	if (queue->size == 0)
		return ;
	if (queue->entries[0].person->id == person->id)
	{
		if (queue->size == 2)
			queue->entries[0] = queue->entries[1];
		queue->size--;
		return ;
	}
	if (queue->size == 2 && queue->entries[1].person->id == person->id)
		queue->size--;
}

long long	calcular_priority_key(t_person *person, long long ahora)
{
	long long	last_compile;

	if (person->sim->config.scheduler == FIFO)
		return (ahora);
	pthread_mutex_lock(&person->state_mutex);
	last_compile = person->last_compile_start;
	pthread_mutex_unlock(&person->state_mutex);
	return (last_compile + person->sim->config.time_to_burnout);
}