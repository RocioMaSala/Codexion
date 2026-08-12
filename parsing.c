#include "codexion.h"

int	ft_atoi(const char *str)
{
	int	i;
	int	countneg;
	int	sum;

	i = 0;
	countneg = 0;
	while ((str[i] == ' ') || ((str[i] >= 9) && (str[i] <= 13)))
	{
		i++;
	}
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			countneg++;
		i++;
	}
	sum = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		sum = sum * 10 + (str[i] - '0');
		i++;
	}
	if (countneg % 2 == 1)
		sum = sum * (-1);
	return (sum);
}

int is_number(const char *str)
{
    int i;
    long acum;
    int digit;
    
    i = 0;
    if(str[0] == '\0')
        return 0;
    acum = 0;
    while(str[i])
    {
        if(str[i] < '0' || str[i] > '9')
            return 0;
        digit = str[i] - '0';
        if (acum > (INT_MAX - digit) / 10)
            return 0;
        acum = acum * 10 + digit;
        i++;
    }
    return 1;
}

int parse_positive_int(const char *str, const char *field_name, int min_value, int *result)
{
    if (!es_numero_valido(str))
    {
        fprintf(stderr, "Error: %s must be a valid positive number\n", field_name);
        return (0);
    }
    *result = atoi(str);
    if (*result < min_value)
    {
        fprintf(stderr, "Error: %s must be at least %d\n", field_name, min_value);
        return (1);
    }
    return (0);
}

int parse_args(int argc, char **argv, t_config *config)
{
    if(argc != 9)
    {
        fprintf(stderr, "Error: se esperaban 8 argumentos\n");
        return (0);
    }
    if (parse_positive_int(argv[1], "number_of_coders", 1, &config->number_of_coders))
        return (1);
    if (parse_positive_int(argv[2], "time_to_burnout", 1, &config->time_to_burnout))
        return (1);
    
    return (0);
}