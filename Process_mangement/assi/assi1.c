#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Function to reverse a string in place
void reverse_string(char str[]) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s S1 S2 ... Sn\n", argv[0]);
        return 1;
    }

    int n = argc - 1;
    pid_t pids[n];

    for (int i = 0; i < n; ++i) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pids[i] == 0) { // Child process
            int len = strlen(argv[i + 1]);
            char *copy = (char *)malloc((len + 1) * sizeof(char));
            if (!copy) {
                perror("malloc failed");
                exit(1);
            }

            strcpy(copy, argv[i + 1]);

            reverse_string(copy);
            printf("Child %d (PID %d): %s\n", i + 1, getpid(), copy);

            free(copy);
            exit(0);
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < n; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
