# Deadline-Driven Scheduler
A Deadline-driven scheduler implemented by c code on MQX. 

## Introduction
5 periodic tasks are created with preset deadline, arrival time and execution time. 8 aperiodic tasks are created with randomly 
generated deadline arrival time and execution time. Schedulers schedules all tasks. Preemtion occurs when a task with lower 
deadline arriving. Overdue tasks will be pushed to overdue list.

##Usage
Run the code on MQX
