*This project has been created as part of the 42 curriculum by romarti2.*

# Codexion

## Description

Codexion is a multithreaded C simulation of coders sitting in a circle who
share USB dongles. Each coder is a thread and needs **two** dongles to
compile. After compiling, it releases them and continues with debugging and
refactoring. A coder who does not start a compile within `time_to_burnout`
burns out and the simulation stops.

Two scheduling policies decide who gets a contested dongle:
- **fifo**: first to ask, first served.
- **edf** (earliest deadline first): the coder closest to burning out goes first.

A monitor thread detects burnouts and the end of the simulation.

## Instructions

- `make`: compile the project
- `make clean`: remove object files
- `make fclean`: remove object files and the binary
- `make re`: recompile from scratch

## Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Times are in milliseconds and `scheduler` is `fifo` or `edf`. Example:

```
./codexion 5 800 200 200 200 3 50 fifo
```

## Blocking cases handled

- **Deadlock / Coffman conditions**: a coder takes both dongles at once under
  the `waiter` mutex or takes none. It never holds one dongle while waiting
  for the other, so *hold-and-wait* (and therefore circular wait) cannot occur.
- **Starvation**: each dongle has a waiting queue ordered by priority (arrival
  time for fifo, burnout deadline for edf). Only the head of the queue can take
  the dongle.
- **Dongle cooldown**: each dongle stores its release time and cannot be taken
  again until `dongle_cooldown` ms have passed.
- **Single coder**: with one coder there is one dongle, so it can never
  compile and burns out at `time_to_burnout`.
- **Termination**: the stop flag is protected by a mutex. Coders check it
  while waiting for dongles and between compile, debug and refactor; waiting
  threads are woken with a broadcast when the simulation stops.
- **Log serialization**: a single function prints under the log mutex and
  checks the stop flag first, so messages never overlap and nothing is printed
  after a burnout.

## Thread synchronization mechanisms

- `pthread_mutex_t waiter`: protects dongle state and queues during acquisition
  and release.
- `pthread_mutex_t stop_mutex`: protects the stop flag.
- `pthread_mutex_t log`: serializes output.
- `pthread_mutex_t state_mutex` (one per coder): protects its state and
  `last_compile_start`, read by the monitor.
- `pthread_cond_t cond`: coders wait on it with `pthread_cond_timedwait` when
  dongles are unavailable. On release and on stop, `pthread_cond_broadcast`
  wakes them to re-check.

The mutexes are always taken in the same order (`waiter` → `stop_mutex` →
`log`), which avoids deadlocks between them. There are no global variables:
all shared data lives in a `t_simulation` struct passed by pointer.

## Resources

- POSIX Threads documentation (`man pthread_mutex_lock`,
  `pthread_cond_timedwait`, `pthread_cond_broadcast`)
- Coffman conditions for deadlock
- 42 project subject

AI usage: used as a tutor to understand mutexes, condition variables, race
conditions and deadlocks, and to review this README.