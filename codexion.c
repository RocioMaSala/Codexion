#include "codexion.h"

int main(int argc, char **argv)
{
	t_simulation sim;
	struct timeval actual_time;
	long long time;

	if (parse_args(argc, argv, &sim.config))
        return (1);
	time = gettimeofday(&actual_time, NULL)

}


