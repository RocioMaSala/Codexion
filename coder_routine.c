#include "codexion.h"

void *coder_routine(void *arg)
{
    t_person *person;

    person = (t_person *)arg;

    while (!simulation_finished(person->sim))
    {
        take_dongles(person);
        compile(person);
        release_dongles(person);
        debug(person);
        refactor(person);
    }
    return (NULL);
}