# **Codexion**

This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].

Description

Codexion is a multithreaded C project that simulates programmers competing for shared resources called dongles.

Each programmer is represented by a thread and must acquire two dongles to compile. After compiling, the dongles are released and the programmer continues with debugging and refactoring.

The main goal is to manage concurrency safely while preventing deadlocks, starvation and race conditions.

Instructions
Compilation
make

To recompile from scratch:

make re

To clean object files:

make clean

To remove all generated files:

make fclean
Execution
./codexion <arguments>

Check the project subject for the required arguments and their order.

Blocking cases handled
Deadlocks: both dongles are acquired under the same synchronization mechanism, preventing threads from holding one dongle indefinitely while waiting for another.
Coffman conditions: the resource acquisition strategy prevents circular wait and hold-and-wait situations.
Starvation: waiting queues and FIFO/priority scheduling ensure that waiting programmers are considered fairly.
Dongle cooldown: each dongle stores its release time and cannot be reused before the configured cooldown has elapsed.
Termination: the simulation stop flag is protected by a mutex and checked by all programmer threads.
Log serialization: logging is protected by a dedicated mutex so messages from different threads do not overlap.
Thread synchronization mechanisms

The project uses POSIX threads and the following synchronization primitives:

pthread_mutex_t — protects shared resources and data such as dongles, waiting queues, programmer state, the simulation status and logging.
pthread_cond_t — allows programmers to wait efficiently when dongles are unavailable and be notified when resources are released.
Custom event mechanism — condition variables are used to notify waiting threads when the shared resource state changes.

For example, dongle acquisition is protected by the waiter mutex, ensuring that checking and modifying the dongle state cannot be performed simultaneously by different threads.

When dongles are released:

pthread_cond_broadcast(&sim->cond);

waiting programmers are notified and can safely check the resources again.

The monitor and programmer threads communicate through shared state protected by mutexes, preventing data races.

Resources
42 project subject and documentation.
POSIX Threads (pthread) documentation.
pthread_mutex_t and pthread_cond_t documentation.
Valgrind / Helgrind documentation for memory and concurrency debugging.
Coffman's deadlock conditions.
AI usage

AI was used as a support tool to:

Understand pthreads, mutexes and condition variables.
Review concurrency and synchronization logic.
Identify potential race conditions and deadlocks.
Understand Valgrind and Helgrind.
Help structure and review the project documentation.

The implementation and final decisions were made by the project authors.