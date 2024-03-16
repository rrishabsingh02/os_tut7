#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main(void) {
    pid_t pid;

    pid = fork(); // Create a new child process

    if (pid == -1) {
        // If fork() returns -1, an error occurred
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This is the child process
        printf("Child process (PID: %d)\n", getpid());

        // Replace the child process with the 'process' program
        char *args[] = {"./process", NULL};
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // This is the parent process
        printf("Parent process (PID: %d), Child PID: %d\n", getpid(), pid);

        // Sleep for 5 seconds before suspending the child process
        sleep(5);
        printf("Sending SIGTSTP to suspend the child process.\n");
        if (kill(pid, SIGTSTP) == -1) {
            perror("kill failed to send SIGTSTP");
            exit(EXIT_FAILURE);
        }

        // Sleep for 10 seconds while the child process is suspended
        sleep(10);
        printf("Sending SIGCONT to resume the child process.\n");
        if (kill(pid, SIGCONT) == -1) {
            perror("kill failed to send SIGCONT");
            exit(EXIT_FAILURE);
        }

        // Wait for the child process to exit and collect its status
        int status;
        waitpid(pid, &status, 0);
        printf("Child process terminated\n");
    }

    return 0;
}
