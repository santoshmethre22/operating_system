// forkNexec3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

int main() {
    char input[256];

    while (1) {
        printf("Enter an executable name: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nError reading input. Exiting.\n");
            break;
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            printf("You entered the file name -----> : %s\n", input);

            char *myargv[] = {input, NULL};
            execve(input, myargv, environ);

            // If execve fails, print reason and exit
            perror("execve failed");
            exit(EXIT_FAILURE);
        }

        // Parent process
        int status;
        waitpid(pid, &status, 0);

        printf("Child process (PID %d) terminated. ", pid);

        if (WIFEXITED(status)) {
            printf("Exited normally with status %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Terminated by signal %d (%s).\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
        } else if (WIFSTOPPED(status)) {
            printf("Stopped by signal %d.\n", WSTOPSIG(status));
        } else {
            printf("Unknown termination.\n");
        }
    }

    return 0;
}
