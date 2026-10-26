/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_outils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romarti2 <romarti2@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:33:16 by romarti2          #+#    #+#             */
/*   Updated: 2026/10/26 17:54:52 by romarti2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

int	parse_positive_int(const char *str, const char *field_name,
		int min_value, int *result)
{
	if (parse_int_digits(str, result))
	{
		fprintf(stderr, "Error: %s must be a valid positive number\n",
			field_name);
		return (1);
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

int	parse_positive_ll(const char *str, const char *field_name,
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
