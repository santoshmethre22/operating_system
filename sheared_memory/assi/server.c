// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "task.h"

struct task *solve;
int shmid;

void cleanup(int sig) {
    if (solve) {
        solve->status = -1; // Indicate shutdown to workers
    }
    printf("\nServer shutting down.\n");
    exit(0);
}

void generate_random_string(char *str, size_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (size_t i = 0; i < length - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0';
}

int main() {
    signal(SIGINT, cleanup);

    // Get or create shared memory
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach to shared memory
    solve = (struct task *)shmat(shmid, NULL, 0);
    if (solve == (struct task *)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize if status is undefined
    if (solve->status != -1) {
        solve->status = 0;
    }

    srand(time(NULL));
    while (1) {
        while (solve->status != 0 && solve->status != 4) {
            usleep(100000);
        }

        char rand_str[100];
        generate_random_string(rand_str, rand() % 90 + 10);

        strcpy(solve->data, rand_str);
        solve->status = 1;

        printf("Server: Generated string \"%s\"\n", rand_str);

        while (solve->status != 3) {
            usleep(100000);
        }

        int len = atoi(solve->data);
        printf("Server: Received length %d from worker %d\n", len, solve->worker_pid);
        solve->status = 4;
        sleep(1);
    }

    return 0;
}
