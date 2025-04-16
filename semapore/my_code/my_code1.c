#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

union semun {
    int val;
};

int main() {
    key_t key = ftok("myfile.txt", 'A');
    int semid = semget(key, 1, IPC_CREAT | 0666);

    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // Only one process should do this (e.g., server)
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // Wait
    semop(semid, &p, 1);

    // Critical section
    printf("Process %d: Inside critical section\n", getpid());
    sleep(2); // Simulate work
    printf("Process %d: Exiting critical section\n", getpid());

    // Signal
    semop(semid, &v, 1);

    return 0;
}
