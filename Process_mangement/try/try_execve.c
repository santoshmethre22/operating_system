#include <stdio.h> /* needed also for perror() */
#include <errno.h> /* needed  for perror() */
#include <unistd.h> /* needed for execve() */

int main(int argc, char *argv[]) {
        int status;
        char *myargv[]={"./santu", NULL};
//       int execve(const char *filename, char *const argv[], char *const envp[]);
        status = execve("apth to executeble file ", myargv, NULL);
        if (status == -1) {
                perror ("Exec Fails: ");
        }

}