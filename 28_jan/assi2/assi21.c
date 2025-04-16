// forkNexec4.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>

extern char **environ;

// Custom strtok-like tokenizer
char *next_token = NULL;

char *my_strtok(char *str, char delimiter) {
    if (str != NULL)
        next_token = str;

    if (next_token == NULL || *next_token == '\0')
        return NULL;

    // Skip leading delimiters
    while (*next_token == delimiter)
        next_token++;

    if (*next_token == '\0')
        return NULL;

    char *token_start = next_token;

    while (*next_token != '\0' && *next_token != delimiter)
        next_token++;

    if (*next_token != '\0') {
        *next_token = '\0';
        next_token++;
    }

    return token_start;
}

int main() {
    char input[256];

    while (1) {
        printf("Enter a command (executable + args): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nError reading input. Exiting.\n");
            break;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            printf("Child received command: %s\n", input);

            // Tokenize input into argv[]
                char *argv[64];  // Up to 63 args + NULL
                int argc = 0;
                char *token = my_strtok(input, ' ');

            while (token != NULL && argc < 63) {
                argv[argc++] = token;
                token = my_strtok(NULL, ' ');
            }
            argv[argc] = NULL;

            // Attempt to execute the command
            execve(argv[0], argv, environ);

            // If execve fails
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
