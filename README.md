🍕 Multithreaded Pizza Ordering System

A multithreaded simulation of a pizza delivery service implemented in C using POSIX threads, mutexes, and condition variables.
Each customer is represented as a thread that goes through the full order workflow, while shared resources (phone operators, pizza makers, ovens, delivery drivers) are synchronized to avoid race conditions.

🔧 Main Concept

Main thread: Creates customer threads, initializes/destroys synchronization structures, and prints final statistics.

Order process:

Phone ordering & payment

Pizza preparation

Baking in shared ovens

Delivery & return

Synchronization: All shared resources are protected using mutexes and condition variables.

Statistics: Tracks revenue, successful/failed orders, preparation & delivery times.

🛠 Technologies

C • POSIX Threads • Mutexes • Condition Variables • Multithreading
