// forkNexec1.c
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
 #include <unistd.h>

 #include <sys/wait.h>
#include <unistd.h>


int main() {
    char input[256];
    while (1) {
        printf("Enter an executable name: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nError reading input. Exiting.\n");
            break;
        }

        // Remove newline character from input, if any
        // remove new other way

        pid_t pid=fork();

        if(pid==0){

            input[strcspn(input, "\n")] = 0;

            printf("you entered the file name -----> : %s\n ",input);
            
            char *myargv[]={input,NULL};
            
            execve(input,myargv,NULL);

        }

        wait(NULL);


        //printf("You entered: %s\n", input);
    }

    return 0;
}