#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <termios.h> 

#define max_length 1024 // Define maximum input length
#define max_processes 10 // Define maximum number of processes

// Global variables for scheduler type and quantum time
int sched_type = 0; // 0 for FCFS, 1 for Round Robin
int quantum = 1; // Quantum time for Round Robin scheduling

// Define process states
typedef enum {
    READY,
    RUNNING,
    SUSPENDED,
    TERMINATED
} process_state_t;

// Define process structure
typedef struct {
    pid_t pid; // Process ID
    int Process_num; // Custom process number for identification
    process_state_t state; // Current state of the process
} process_t;

process_t processes[max_processes]; // Array to store processes
int process_amount = 0; // Counter for the number of processes
int current_process = -1; // Index of the currently running process

// Function to initialize process array
void initialize_processes() {
    for (int i = 0; i < max_processes; i++) {
        processes[i].pid = -1;
        processes[i].Process_num = -1;
        processes[i].state = TERMINATED;
    }
}

// Signal handler for SIGINT to suspend the current running process
void handle_sigint(int sig) {
    if (current_process != -1) {
        kill(processes[current_process].pid, SIGSTOP);
        processes[current_process].state = SUSPENDED;
        printf("Process %d suspended.\n", processes[current_process].Process_num);
        current_process = -1;
    }
}

// Function to update process states for FCFS scheduling
void update_process_states_for_fcfs() {
    for (int i = 0; i < process_amount; i++) {
        if (processes[i].state == READY) {
            kill(processes[i].pid, SIGCONT);
            processes[i].state = RUNNING;
            current_process = i;
            printf("Process %d started.\n", processes[i].Process_num);
            break;
        }
    }
}

// Function to create a specified number of processes
void create(int n) {
    char *args[] = {"./task", NULL};
    for (int i = 0; i < n; i++) {
        if (process_amount >= max_processes) {
            printf("Maximum number of processes reached.\n");
            return;
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            execvp("./task", args);
            perror("execvp"); 
            exit(EXIT_FAILURE);
        } else {
            processes[process_amount].pid = pid;
            processes[process_amount].Process_num = process_amount + 1;
            processes[process_amount].state = READY;
            process_amount++;
        }
    }
    if (sched_type == 0) {
        update_process_states_for_fcfs(); 
    }
}

// Function to terminate a specific process by process number
void end(int Process_num) {
    for (int i = 0; i < process_amount; i++) {
        if (processes[i].Process_num == Process_num) {
            if (processes[i].state != TERMINATED) {
                kill(processes[i].pid, SIGKILL);
                waitpid(processes[i].pid, NULL, 0); 
                processes[i].state = TERMINATED;
                printf("Process %d killed.\n", Process_num);
                if (i == current_process) {
                    current_process = -1; // Reset the current running process
                }
            } else {
                printf("Process %d is already terminated.\n", Process_num);
            }
            return;
        }
    }
    printf("Process %d not found.\n", Process_num); // If process number not found
}
// Function to list all processes and their states
void list() {
    printf("PID\tProcess Number\tState\n");
    for (int i = 0; i < process_amount; i++) {
        char *state_str;
        switch (processes[i].state) {
            case READY: state_str = "Ready"; break;
            case RUNNING: state_str = "Running"; break;
            case SUSPENDED: state_str = "Suspended"; break;
            case TERMINATED: state_str = "Terminated"; break;
            default: state_str = "Unknown"; break;
        }
        printf("%d\t%d\t\t%s\n", processes[i].pid, processes[i].Process_num, state_str); // Print process info
    }
}

// Handler for Round Robin scheduling
void round_robin_handler(int sig) {
    if (sched_type != 1) return; // Only execute for Round Robin scheduling

    int prev_process = current_process;
    if (prev_process != -1 && processes[prev_process].state == RUNNING) {
        kill(processes[prev_process].pid, SIGSTOP);
        processes[prev_process].state = SUSPENDED;
    }

    // Find the next process to run
    int found = 0;
    for (int attempts = 0; attempts < process_amount; attempts++) {
        current_process = (current_process + 1) % process_amount;
        if (processes[current_process].state == READY || processes[current_process].state == SUSPENDED) {
            kill(processes[current_process].pid, SIGCONT);
            processes[current_process].state = RUNNING;
            found = 1;
            break;
        }
    }

    if (!found) {
        current_process = -1; // No suitable process found
    }

    alarm(quantum); // Set up the alarm for the next quantum
}

// Function to set scheduler to Round Robin and specify quantum
void set_scheduler_round_robin(int q) {
    sched_type = 1; // Set scheduler type to Round Robin
    quantum = q; // Set quantum time
    printf("Round Robin scheduling selected with a quantum of %d seconds.\n", quantum);
    signal(SIGALRM, round_robin_handler); // Set signal handler for alarm
    alarm(quantum); // Start the Round Robin scheduling
}

// Function to set scheduler to First-Come, First-Served
void set_scheduler_fcfs() {
    sched_type = 0; // Set scheduler type to FCFS
    printf("Setting scheduler to FCFS.\n");
    alarm(0); // Cancel any existing alarms
    update_process_states_for_fcfs(); // Update process states for FCFS
}

// Function to resume a specific suspended process
void resume(int Process_num) {
    for (int i = 0; i < process_amount; i++) {
        if (processes[i].Process_num == Process_num && processes[i].state == SUSPENDED) {
            kill(processes[i].pid, SIGCONT); // Send SIGCONT to resume the process
            processes[i].state = READY;
            printf("Process %d resumed.\n", Process_num);
            return;
        }
    }
    printf("Process %d not found or not in a suspended state.\n", Process_num);
}

// Function to resume all suspended processes
void resume_all() {
    int resumed_count = 0;
    for (int i = 0; i < process_amount; i++) {
        if (processes[i].state == SUSPENDED) {
            kill(processes[i].pid, SIGCONT); // Send SIGCONT to resume each suspended process
            processes[i].state = READY;
            resumed_count++;
            printf("Process %d resumed.\n", processes[i].Process_num); 
        }
    }
    if (resumed_count == 0) {
        printf("No suspended processes to resume.\n");
    } else {
        printf("%d suspended processes resumed.\n", resumed_count);
    }
}

// Main function to handle input and execute commands
int main() {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t read_size;

    // Set up signal handlers
    signal(SIGINT, handle_sigint);
    initialize_processes();
    signal(SIGALRM, round_robin_handler);

    // Main loop to process input commands
    while(1) {
        printf("shell 5500>>>");
        fflush(stdout);
        read_size = getline(&buffer, &size, stdin); // Read input from user

        if (read_size <= 0) {
            if (feof(stdin)) { // Check for end-of-file
                printf("\nExiting shell.\n");
                break;
            }
            printf("Error reading input. Try again.\n");
            continue;
        }

        buffer[read_size - 1] = '\0'; // Remove newline character

        // Process commands
        if (strcmp(buffer, "x") == 0) {
            printf("Exiting and terminating all processes.\n");
            for (int i = 0; i < process_amount; i++) {
                if (processes[i].state != TERMINATED) {
                    kill(processes[i].pid, SIGKILL); // Terminate each process
                }
            }
            break; // Exit the main loop
        } else if (strncmp(buffer, "c ", 2) == 0) {
            int n = atoi(buffer + 2);
            create(n); // Create n processes
        } else if (strcmp(buffer, "l") == 0) {
            list(); // List all processes
        } else if (strncmp(buffer, "s rr ", 5) == 0) {
            int quantum = atoi(buffer + 5);
            set_scheduler_round_robin(quantum); // Set scheduler to Round Robin
        } else if (strcmp(buffer, "s fcfs") == 0) {
            set_scheduler_fcfs(); // Set scheduler to FCFS
        } else if (strncmp(buffer, "k ", 2) == 0) {
            int Process_num = atoi(buffer + 2);
            end(Process_num); // Terminate a specific process
        } else if (strncmp(buffer, "r ", 2) == 0) {
            if (strcmp(buffer + 2, "all") == 0) {
                resume_all(); // Resume all suspended processes
            } else {
                int Process_num = atoi(buffer + 2);
                resume(Process_num); // Resume a specific process
            }
        } else {
            printf("Command not recognized.\n"); // Handle unknown command
        }
    }

    free(buffer); // Free allocated memory for input buffer
    return 0; // Exit program
}