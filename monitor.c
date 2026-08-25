/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:00 by romarti2          #+#    #+#             */
/*   Updated: 2026/08/17 12:36:21 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


int all_completed(t_simulation *sim)
{
    int i;

    i = 0;
    while(i < sim->config.number_of_coders)
    {
        if(sim->persons[i].number_of_compilations < sim->config.number_of_compiles_required)
        {
            return 0;
        }
        i++;
    }
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop_simulation = 1;
    pthread_cond_broadcast(&sim->cond);
    pthread_mutex_unlock(&sim->stop_mutex);
    return (1);
}

void one_burnout(t_simulation *sim)
{
    int i;
    long long deadline;
    long long actual_time;

    i = 0;
    while(i < sim->config.number_of_coders)
    {
        pthread_mutex_lock(&sim->persons[i].state_mutex);
        if (sim->persons[i].actual_state != COMPILING)
        {
            deadline = sim->persons[i].last_compile_start + sim->config.time_to_burnout;
            actual_time = tiempo_actual_relativo(sim);
            if (actual_time > deadline)
            {
                pthread_mutex_unlock(&sim->persons[i].state_mutex);
                pthread_mutex_lock(&sim->stop_mutex);
                if(!sim->stop_simulation)
                {
                    sim->stop_simulation = 1;
                    pthread_mutex_lock(&sim->log);
                    printf("%lld %d burned out\n", actual_time, sim->persons[i].id);
                    pthread_mutex_unlock(&sim->log);
                    pthread_cond_broadcast(&sim->cond);
                }
                pthread_mutex_unlock(&sim->stop_mutex);
            }
        }
        pthread_mutex_unlock(&sim->persons[i].state_mutex);
        i++;
    }
    return;
}


void *monitor_routine(void *arg)
{
    t_simulation *sim;

    sim = (t_simulation*)arg;
    while ((!should_stop(sim)))
    {
        if(all_completed(sim))
            break;
        one_burnout(sim);
        usleep(1000);
    }
    return(NULL);
}
