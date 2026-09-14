/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:34:17 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 13:05:40 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>   // malloc, free, atoi
# include <string.h>   // strcmp, strlen
# include <sys/time.h> // gettimeofday
# include <unistd.h>   // usleep, write

typedef enum e_state
{
	TAKING_A_DONGLE,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNED_OUT
}							t_state;

typedef enum e_scheduler
{
	FIFO,
	EDF
}							t_scheduler;

typedef struct s_config
{
	int						number_of_coders;
	long long				time_to_burnout;
	long long				time_to_compile;
	long long				time_to_debug;
	long long				time_to_refactor;
	int						number_of_compiles_required;
	long long				dongle_cooldown;
	t_scheduler				scheduler;
}							t_config;

typedef struct s_person		t_person;

typedef struct s_heap_entry
{
	t_person				*person;
	long long				priority_key;
}							t_heap_entry;

typedef struct s_queue
{
	t_heap_entry			entries[2];
	int						size;
}							t_queue;

typedef struct s_dongle
{
	int						is_free;
	long long				time_liberation;
	pthread_mutex_t			mutex;
	pthread_cond_t			cond;
	t_queue					waiting_queue;
}							t_dongle;

typedef struct s_simulation	t_simulation;

struct						s_person
{
	int						id;
	t_dongle				*dongle_left;
	t_dongle				*dongle_right;
	long long				last_compile_start;
	int						number_of_compilations;
	t_state					actual_state;
	pthread_mutex_t			state_mutex;
	t_simulation			*sim;
};

struct						s_simulation
{
	long long				time_start_sim;
	t_config				config;
	t_person				*persons;
	t_dongle				*dongles;
	pthread_mutex_t			waiter;
	pthread_mutex_t			log;
	pthread_cond_t			cond;
	pthread_mutex_t			stop_mutex;
	int						stop_simulation;
};

int							parse_args(int argc, char **argv, t_config *config);
int							main(int argc, char **argv);
int							create_threads(t_simulation *sim,
								pthread_t *thread_ids);
int							assign_data_persons(t_simulation *sim);
int							init_simulation(t_simulation *sim);
void						*coder_routine(void *arg);
void						cleanup_simulation(t_simulation *sim);
void						cleanup_init_failure(t_simulation *sim, int hasta,
								int stage);
long long					tiempo_actual_relativo(t_simulation *sim);
int							dongle_disponible(t_dongle *dongle, long long ahora,
								long long cooldown);
int							should_stop(t_simulation *sim);
int							take_dongles(t_person *person);
void						compile(t_person *person);
void						release_one_dongle(t_dongle *dongle,
								t_simulation *sim);
void						release_dongles(t_person *person);
void						debug(t_person *person);
void						refactor(t_person *person);
int							all_completed(t_simulation *sim);
void						one_burnout(t_simulation *sim);
void						*monitor_routine(void *arg);
void						insertar_en_cola(t_queue *queue, t_person *person,
								long long key);
int							tiene_prioridad(t_queue *queue, t_person *person);
void						eliminar_de_cola(t_queue *queue, t_person *person);
long long					calcular_priority_key(t_person *person,
								long long ahora);

#endif
