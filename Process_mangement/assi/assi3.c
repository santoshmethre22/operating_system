#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Manually convert a string to int without using atoi()
int string_to_int(const char *str) {
    int val = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            fprintf(stderr, "Invalid number: %s\n", str);
            exit(1);
        }
        val = val * 10 + (str[i] - '0');
    }
    return val;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s num1 num2 ... numN\n", argv[0]);
        return 1;
    }

    int n = argc - 1;
    pid_t pids[n];

    for (int i = 0; i < n; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pids[i] == 0) {
            // Child process: manually convert string to int and exit with that value
            int val = string_to_int(argv[i + 1]);

            // Warning: exit value must be in [0, 255] due to 8-bit exit status limit
            if (val > 255) {
                fprintf(stderr, "Warning: %s too large for exit status, truncating to 255\n", argv[i + 1]);
                val = 255;
            }

            exit(val);
        }
    }

    // Parent: wait for all children and find the max exit value
    int max_val = 0;
    for (int i = 0; i < n; i++) {
        int status = 0;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) {
            int child_val = WEXITSTATUS(status);
            if (child_val > max_val) {
                max_val = child_val;
            }
        }
    }

    printf("Maximum exit value: %d\n", max_val);
    return 0;
}
