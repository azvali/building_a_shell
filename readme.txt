# Project Scheduler

## Overview

This project implements a simple process scheduler in C, demonstrating basic scheduling algorithms such as First-Come, First-Served (FCFS) and Round Robin (RR). It comprises a main scheduler program (`shell`) and a simulated task program (`task`) to showcase the scheduling functionality.

## Prerequisites

- GCC (GNU Compiler Collection)
- Linux or Unix-like operating system

## Compilation

To compile the scheduler and task programs manually without using a Makefile, navigate to the project directory and run the following commands:

```bash
gcc task.c -o task
gcc main.c -o shell



clean build 

rm -f task shell


usage 

./shell





Scheduler Commands
The scheduler program accepts several commands for managing processes:

c <n>: Create <n> number of tasks. Example: c 5 creates 5 tasks.
l: List all processes, showing their PID, process number, and state.
s rr <quantum>: Set the scheduler to Round Robin mode with a specified quantum time in seconds. Example: s rr 2 sets a quantum of 2 seconds.
s fcfs: Set the scheduler to First-Come, First-Served mode.
k <process_number>: Terminate a specific process by its process number. Example: k 3 terminates process 3.
r <process_number>: Resume a specific suspended process. Example: r 4 resumes process 4.
r all: Resume all suspended processes.
x: Exit the scheduler and terminate all running and suspended processes.





