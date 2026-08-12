#include <codexion.h>
#include <pthread.h>

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

