# Project Scheduler

## Overview

This project implements a simple process scheduler in C, demonstrating basic scheduling algorithms such as First-Come, First-Served (FCFS) and Round Robin (RR). It comprises a main scheduler program (`shell`) and a simulated task program (`task`) to showcase the scheduling functionality.

## Prerequisites

- GCC (GNU Compiler Collection)
- Linux or Unix-like operating system

## Compilation

To compile the scheduler (`shell`) and the task (`task`) programs, navigate to the project directory where the source files and `Makefile` are located. Use the following command to compile the project:

```bash
make




Cleaning Build Files

To remove the compiled executables and clean up your project directory, run the following command:

make clean


Starting the Scheduler
To start the main scheduler program, execute:

./shell




ChatGPT
Yes, updating your README to reflect the use of the Makefile for compiling your project would be beneficial. This will provide clear, concise instructions for anyone who wants to build and run your project, ensuring they use the intended build process. Here's how you can adjust the Compilation and Cleaning Build Files sections of your README to guide users through using the Makefile:

markdown
Copy code
# Project Scheduler

## Overview

This project implements a simple process scheduler in C, demonstrating basic scheduling algorithms such as First-Come, First-Served (FCFS) and Round Robin (RR). It comprises a main scheduler program (`shell`) and a simulated task program (`task`) to showcase the scheduling functionality.

## Prerequisites

- GCC (GNU Compiler Collection)
- Linux or Unix-like operating system

## Compilation

To compile the scheduler (`shell`) and the task (`task`) programs, navigate to the project directory where the source files and `Makefile` are located. Use the following command to compile the project:

```bash
make
This command reads the Makefile in the current directory, compiling the source files into two executables named shell and task.

Cleaning Build Files

To remove the compiled executables and clean up your project directory, run the following command:

bash
Copy code
make clean
This uses the clean target specified in the Makefile to remove the shell and task executables, ensuring a clean state for a fresh compile.

Usage

Starting the Scheduler
To start the main scheduler program, execute:

bash
Copy code
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


