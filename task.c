#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Declares a volatile sig_atomic_t variable to safely change it inside the signal handler.
volatile sig_atomic_t keep_running = 1;

// Signal handler function that sets keep_running to 0 when SIGINT is received.
void my_sig_handler(int signum) {
    // Checks if the signal received is SIGINT.
    if (signum == SIGINT) {
        keep_running = 0; // Stops the main loop and allows the program to exit gracefully.
    }
}

int main() {
    int count = 0; // Initialize a counter for the number of iterations.
    int mypid = getpid(); // Gets the process ID of the current process.

    // Registers the signal handler for SIGINT.
    signal(SIGINT, my_sig_handler);

    // Continues to run until keep_running is set to 0 by the signal handler.
    while(keep_running) {
        // Prints the process ID and the current iteration count.
        printf("Process %d at iteration %d\n", mypid, count++);
        sleep(1); // Pauses execution for 1 second, simulating work being done.
    }

    return 0; // Exits the program normally.
}
