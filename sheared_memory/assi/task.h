// task.h
#ifndef TASK_H
#define TASK_H

#include <sys/types.h>

#define SHM_KEY 123 // Replace with any unique key
#define SHM_SIZE sizeof(struct task)

struct task {
    char data[100];
    pid_t worker_pid;
    int status;
};

#endif
