#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <termios.h> 

#define MAX_COMMAND_LENGTH 1024
#define MAX_NUMBER_OF_PROCESSES 10

int scheduler_type = 0; 
int quantum = 1; 

typedef enum {
    READY,
    RUNNING,
    SUSPENDED,
    TERMINATED
} process_state_t;

typedef struct {
    pid_t pid;
    int process_number;
    process_state_t state;
} process_t;

process_t processes[MAX_NUMBER_OF_PROCESSES];
int process_count = 0;
int current_running_process = -1; 

void initialize_processes() {
    for (int i = 0; i < MAX_NUMBER_OF_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].process_number = -1;
        processes[i].state = TERMINATED;
    }
}

void handle_sigint(int sig) {
    if (current_running_process != -1) {
        kill(processes[current_running_process].pid, SIGSTOP);
        processes[current_running_process].state = SUSPENDED;
        printf("Process %d suspended.\n", processes[current_running_process].process_number);
        current_running_process = -1;
    }
}

void update_process_states_for_fcfs() {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].state == READY) {
            kill(processes[i].pid, SIGCONT);
            processes[i].state = RUNNING;
            current_running_process = i;
            printf("Process %d started.\n", processes[i].process_number);
            break;
        }
    }
}


void create_processes(int n) {
    char *args[] = {"./task", NULL};
    for (int i = 0; i < n; i++) {
        if (process_count >= MAX_NUMBER_OF_PROCESSES) {
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
            processes[process_count].pid = pid;
            processes[process_count].process_number = process_count + 1;
            processes[process_count].state = READY;
            process_count++;
        }
    }
    if (scheduler_type == 0) {
        update_process_states_for_fcfs(); 
    }
}


void kill_process(int process_number) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].process_number == process_number) {
            if (processes[i].state != TERMINATED) {
                kill(processes[i].pid, SIGKILL);
                waitpid(processes[i].pid, NULL, 0); 
                processes[i].state = TERMINATED;
                printf("Process %d killed.\n", process_number);
                if (i == current_running_process) {
                    current_running_process = -1;
                }
            } else {
                printf("Process %d is already terminated.\n", process_number);
            }
            return;
        }
    }
    printf("Process %d not found.\n", process_number);
}

void list_processes() {
    printf("PID\tProcess Number\tState\n");
    for (int i = 0; i < process_count; i++) {
        char *state_str;
        switch (processes[i].state) {
            case READY: state_str = "Ready"; break;
            case RUNNING: state_str = "Running"; break;
            case SUSPENDED: state_str = "Suspended"; break;
            case TERMINATED: state_str = "Terminated"; break;
            default: state_str = "Unknown"; break;
        }
        printf("%d\t%d\t\t%s\n", processes[i].pid, processes[i].process_number, state_str);
    }
}

void round_robin_handler(int sig) {
    if (scheduler_type != 1) return; 

    int previous_process = current_running_process;
    if (previous_process != -1 && processes[previous_process].state == RUNNING) {
        kill(processes[previous_process].pid, SIGSTOP);
        processes[previous_process].state = SUSPENDED;
    }

    int found = 0;
    for (int attempts = 0; attempts < process_count; attempts++) {
        current_running_process = (current_running_process + 1) % process_count;
        if (processes[current_running_process].state == READY || processes[current_running_process].state == SUSPENDED) {
            kill(processes[current_running_process].pid, SIGCONT);
            processes[current_running_process].state = RUNNING;
            found = 1;
            break;
        }
    }

    if (!found) {
        current_running_process = -1;
    }

    alarm(quantum);
}

void set_scheduler_round_robin(int q) {
    scheduler_type = 1; 
    quantum = q;
    printf("Round Robin scheduling selected with a quantum of %d seconds.\n", quantum);
    signal(SIGALRM, round_robin_handler);
    alarm(quantum); 
}

void set_scheduler_fcfs() {
    scheduler_type = 0; 
    printf("Setting scheduler to FCFS.\n");
    alarm(0); 
    update_process_states_for_fcfs(); 
}


void resume_process(int process_number) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].process_number == process_number && processes[i].state == SUSPENDED) {
            kill(processes[i].pid, SIGCONT);
            processes[i].state = READY;
            printf("Process %d resumed.\n", process_number);
            return;
        }
    }
    printf("Process %d not found or not in a suspended state.\n", process_number);
}

void resume_all_processes() {
    int resumed_count = 0;
    for (int i = 0; i < process_count; i++) {
        if (processes[i].state == SUSPENDED) {
            kill(processes[i].pid, SIGCONT);
            processes[i].state = READY;
            resumed_count++;
            printf("Process %d resumed.\n", processes[i].process_number); 
        }
    }
    if (resumed_count == 0) {
        printf("No suspended processes to resume.\n");
    } else {
        printf("%d suspended processes resumed.\n", resumed_count);
    }
}

int main() {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t read_size;

    signal(SIGINT, handle_sigint);
    initialize_processes();
    signal(SIGALRM, round_robin_handler);

    while(1) {
        printf("shell 5500>>>");
        fflush(stdout);
        read_size = getline(&buffer, &size, stdin);

        if (read_size <= 0) {
            if (feof(stdin)) {
                printf("\nExiting shell.\n");
                break;
            }
            printf("Error reading input. Try again.\n");
            continue;
        }

        buffer[read_size - 1] = '\0'; 

        if (strcmp(buffer, "x") == 0) {
            printf("Exiting and terminating all processes.\n");
            for (int i = 0; i < process_count; i++) {
                if (processes[i].state != TERMINATED) {
                    kill(processes[i].pid, SIGKILL);
                }
            }
            break;
        } else if (strncmp(buffer, "c ", 2) == 0) {
            int n = atoi(buffer + 2);
            create_processes(n);
        } else if (strcmp(buffer, "l") == 0) {
            list_processes();
        } else if (strncmp(buffer, "s rr ", 5) == 0) {
            int quantum = atoi(buffer + 5);
            set_scheduler_round_robin(quantum);
        } else if (strcmp(buffer, "s fcfs") == 0) {
            set_scheduler_fcfs();
        } else if (strncmp(buffer, "k ", 2) == 0) {
            int process_number = atoi(buffer + 2);
            kill_process(process_number);
        } else if (strncmp(buffer, "r ", 2) == 0) {
            if (strcmp(buffer + 2, "all") == 0) {
                resume_all_processes();
            } else {
                int process_number = atoi(buffer + 2);
                resume_process(process_number);
            }
        } else {
            printf("Command not recognized.\n");
        }
    }

    free(buffer);
    return 0;
}