# Exercise 2: Bottlenecks

## Mutex and Channel basics

> * **Atomic Operations**: Atomic operations in concurrent programming are program operations that run completely independently of any other processes. An operation during which a processor can simultaneously read a location and write it in the same bus operation. This prevents any other processor or I/O device from writing or reading memory until the operation is complete. Atomic implies indivisibility and irreducibility, so an atomic operation must be performed entirely or not performed at all.
> * **Semaphore**: In computer science, a semaphore is a variable or abstract data type. A counting semaphore is a variable which you increment and decrement but the variable can’t go lower than zero.  If the semaphore is restricted to 0 and 1 it’s called a binary semaphore. “Think of semaphores as bouncers at a nightclub. There are a dedicated number of people that are allowed in the club at once. If the club is full no one is allowed to enter, but as soon as one person leaves another person might enter.”
> * **Mutex (Mutual exclusion)**: In computer science, mutual exclusion refers to the requirement of ensuring that no two concurrent processes are in their critical section at the same time; it is a basic requirement in concurrency control, to prevent race conditions.
> * **Critical Section**: In concurrent programming, a critical section is a part of a multi-process program that may not be concurrently executed by more than one of the program's processes. In other words, it is a piece of a program that requires mutual exclusion of access

*Shamelessly copy-pasted from Wikipedia (21/01/2016)*
