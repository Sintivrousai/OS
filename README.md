# 🍕 Multithreaded Pizza Ordering System

A multithreaded simulation of a pizza delivery service implemented in C using **POSIX threads**, **mutexes**, and **condition variables**.  
Each customer is represented as a thread that goes through the full order workflow, while shared resources (phone operators, pizza makers, ovens, delivery drivers) are synchronized to avoid race conditions.

## 🔧 Main Concept

- **Main Thread**  
  Initializes synchronization structures, creates customer threads, waits for completion, and prints final statistics.

- **Order Workflow**  
  - Phone ordering & payment  
  - Pizza preparation  
  - Baking using shared oven resources  
  - Delivery & return simulation  

- **Synchronization**  
  Shared resources (operators, makers, ovens, drivers) are protected with mutexes and condition variables.

- **Statistics**  
  Tracks total revenue, successful/failed orders, and timing metrics (service time, delivery time).

## 🛠 Technologies
C • POSIX Threads • Mutexes • Condition Variables • Multithreading
