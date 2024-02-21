#include <stdio.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t keep_running = 1;

void my_sig_handler(int signum) {
    if (signum == SIGINT) {
        keep_running = 0;
    }
}

int main() {
    int count = 0;
    int mypid = getpid();

    signal(SIGINT, my_sig_handler);

    while(keep_running) {
        printf("Process %d at iteration %d\n", mypid, count++);
        sleep(1); 
    }

    return 0;
}