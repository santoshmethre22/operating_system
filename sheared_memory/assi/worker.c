// worker.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "task.h"

int main() {
    // Connect to shared memory
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    struct task *solve = (struct task *)shmat(shmid, NULL, 0);
    if (solve == (struct task *)-1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        if (solve->status == -1) {
            printf("Worker %d: Server has shut down. Exiting.\n", getpid());
            break;
        }

        if (solve->status == 1) {
            solve->status = 2;
            solve->worker_pid = getpid();

            int len = strlen(solve->data);
            snprintf(solve->data, sizeof(solve->data), "%d", len);
            solve->status = 3;

            printf("Worker %d: Computed length %d\n", getpid(), len);
        }

        usleep(100000);
    }

    return 0;
}
