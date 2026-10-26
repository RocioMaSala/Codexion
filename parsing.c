/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com:w  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:16 by romarti2          #+#    #+#             */
/*   Updated: 2026/09/10 13:05:17 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_scheduler(const char *str, const char *field_name,
		t_scheduler *result)
{
	if (strcmp(str, "fifo") == 0)
	{
		*result = FIFO;
		return (0);
	}
	if (strcmp(str, "edf") == 0)
	{
		*result = EDF;
		return (0);
	}
	fprintf(stderr, "Error: %s must be either 'fifo' or 'edf'\n", field_name);
	return (1);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		fprintf(stderr, "Error: se esperaban 8 argumentos\n");
		return (1);
	}
	if (parse_positive_int(argv[1], "number_of_coders", 1,
			&config->number_of_coders)
		|| parse_positive_ll(argv[2], "time_to_burnout", 1,
			&config->time_to_burnout)
		|| parse_positive_ll(argv[3], "time_to_compile", 0,
			&config->time_to_compile)
		|| parse_positive_ll(argv[4], "time_to_debug", 0,
			&config->time_to_debug)
		|| parse_positive_ll(argv[5], "time_to_refactor", 0,
			&config->time_to_refactor)
		|| parse_positive_int(argv[6], "number_of_compiles_required", 1,
			&config->number_of_compiles_required)
		|| parse_positive_ll(argv[7], "dongle_cooldown", 1,
			&config->dongle_cooldown)
		|| parse_scheduler(argv[8], "scheduler", &config->scheduler))
		return (1);
	return (0);
}
