#include <stdio.h>
#include <unistd.h>       // for fork(), sleep()
#include <sys/wait.h>     // for wait()
#include <sys/types.h>    // for pid_t
#include <sys/ipc.h>      // for shmget(), shmctl()
#include <sys/shm.h>      // for shmget(), shmctl(), shmat(), shmdt()
#include <signal.h>       // for signal(), kill()
#include <errno.h>        // for perror()
#include <stdlib.h>       // for exit()



int shmid; // Global shared memory id for signal handler



typedef void (*sighandler_t)(int);



// Signal handler to clean up shared memory and kill all processes
void releaseSHM(int signum) {
    int status;

    // Remove shared memory
    status = shmctl(shmid, IPC_RMID, NULL);
    if (status == 0) {
        fprintf(stderr, "Removed shared memory with id = %d.\n", shmid);
    } else {
        perror("Cannot remove shared memory");
    }

    // Kill all processes in the same process group
    status = kill(0, SIGKILL);
    if (status == -1) {
        perror("kill failed");
    }
    // Note: Successful kill will terminate processes, so this line might not execute.
}

int main() {
    int status;
    pid_t pid;

    // Install signal handler for Ctrl+C
    signal(SIGINT, releaseSHM);

    // Create shared memory segment
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
    if (shmid == -1) {
        perror("shmget() failed");
        exit(1);
    }

    printf("shmget() returns shmid = %d.\n", shmid);

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        int *pi_child = (int *)shmat(shmid, NULL, 0);
        if (pi_child == (void *)-1) {
            perror("shmat() failed in child");
            exit(1);
        }

        for (int i = 0; i < 50; i++) {
            printf("Child reads: %d\n", *pi_child);
            getchar();  // Wait for Enter key
        }

        shmdt(pi_child); // Detach shared memory
        exit(0);
    } else {
        // Parent process
        int *pi_parent = (int *)shmat(shmid, NULL, 0);
        if (pi_parent == (void *)-1) {
            perror("shmat() failed in parent");
            exit(1);
        }

        for (int i = 0; i < 50; i++) {
            *pi_parent = i;
            printf("Parent writes: %d\n", *pi_parent);
            getchar();  // Wait for Enter key
        }

        wait(&status);
        printf("Child exited with status = %d\n", status);

        shmdt(pi_parent); // Detach shared memory
        shmctl(shmid, IPC_RMID, NULL); // Clean up (if signal handler didn’t run)
        exit(0);
    }
}
