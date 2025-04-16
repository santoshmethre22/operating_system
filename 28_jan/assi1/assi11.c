// forkNexec1.c
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

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
        input[strcspn(input, "\n")] = 0;

        printf("You entered: %s\n", input);
    }

    return 0;
}
