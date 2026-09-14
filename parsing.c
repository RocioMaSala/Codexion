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

static int	parse_int_digits(const char *str, int *result)
{
	int		i;
	long	acum;
	int		digit;

	i = 0;
	if (str[0] == '\0')
		return (1);
	acum = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		digit = str[i] - '0';
		if (acum > (INT_MAX - digit) / 10)
			return (1);
		acum = acum * 10 + digit;
		i++;
	}
	*result = acum;
	return (0);
}

static int	parse_positive_int(const char *str, const char *field_name,
		int min_value, int *result)
{
	if (parse_int_digits(str, result))
	{
		fprintf(stderr, "Error: %s must be a valid positive number\n",
			field_name);
		return (1); // 1 es igual a error y 0 es igual a ok
	}
	if (*result < min_value)
	{
		fprintf(stderr, "Error: %s must be at least %d\n", field_name,
			min_value);
		return (1);
	}
	return (0);
}

static int	parse_ll_digits(const char *str, long long *result)
{
	int			i;
	long long	acum;
	int			digit;

	i = 0;
	if (str[0] == '\0')
		return (1);
	acum = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		digit = str[i] - '0';
		if (acum > (LLONG_MAX - digit) / 10)
			return (1);
		acum = acum * 10 + digit;
		i++;
	}
	*result = acum;
	return (0);
}

static int	parse_positive_ll(const char *str, const char *field_name,
		long long min_value, long long *result)
{
	if (parse_ll_digits(str, result))
	{
		fprintf(stderr, "Error: %s must be a valid positive number\n",
			field_name);
		return (1);
	}
	if (*result < min_value)
	{
		fprintf(stderr, "Error: %s must be at least %lld\n", field_name,
			min_value);
		return (1);
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		fprintf(stderr, "Error: se esperaban 8 argumentos\n");
		return (1);
	}
	if (parse_positive_int(argv[1], "number_of_coders", 1,
			&config->number_of_coders))
		return (1);
	if (parse_positive_ll(argv[2], "time_to_burnout", 1,
			&config->time_to_burnout))
		return (1);
	if (parse_positive_ll(argv[3], "time_to_compile", 0,
			&config->time_to_compile))
		return (1);
	if (parse_positive_ll(argv[4], "time_to_debug", 0, &config->time_to_debug))
		return (1);
	if (parse_positive_ll(argv[5], "time_to_refactor", 0,
			&config->time_to_refactor))
		return (1);
	if (parse_positive_int(argv[6], "number_of_compiles_required", 1,
			&config->number_of_compiles_required))
		return (1);
	if (parse_positive_ll(argv[7], "dongle_cooldown", 1,
			&config->dongle_cooldown))
		return (1);
	if (parse_scheduler(argv[8], "scheduler", &config->scheduler))
		return (1);
	return (0);
}
