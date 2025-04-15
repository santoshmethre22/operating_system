#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Function to reverse a string
void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s S1 S2 ... Sn\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = argc - 1;
    pid_t pids[n];

    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // Child process
            char *str = strdup(argv[i + 1]); // Duplicate string to avoid modifying original
            reverse_string(str);
            printf("Child %d (PID %d): Reversed string: %s\n", i + 1, getpid(), str);
            free(str);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process stores child's PID
            pids[i] = pid;
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < n; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    return EXIT_SUCCESS;
}
