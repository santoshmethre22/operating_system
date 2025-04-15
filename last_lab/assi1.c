#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 1024
#define MAX_TOKENS 100

typedef enum { NONE, SEQUENTIAL, AND, OR } Operator;

typedef struct {
    char *command;
    Operator op;
} CommandChunk;



int main(int argc, char *argv[]) {
    FILE *input = stdin;             
    char line[MAX_CMD_LEN];
    // create a batch mode also-----------------------

    //////////////////
    while (1) {
        if (input == stdin) {
            printf("mysh> ");         
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), input) == NULL) break;
    
        int n = strlen(line);

        // Remove trailing newline character if exists
        if (n > 0 && line[n - 1] == '\n') {
            line[n - 1] = '\0';
        }

        // Skip empty lines
        if (strlen(line) == 0) continue;

        printf("You entered: %s\n", line);
    }

    // implement the second part  tokenizeation



 // ----------------------
        // TOKENIZATION STARTS
        // ----------------------
        CommandChunk chunks[MAX_TOKENS];
        int chunk_count = 0;

        char *ptr = line;
        Operator last_op = NONE;

        while (*ptr) {
            //  ---------// Skip leading spaces
            while (*ptr == ' ') ptr++;




            char *start = ptr;
            char *end = NULL;

            // Find next operator
            while (ptr!=NULL) {
                if (ptr[0] == ';') {
                    end = ptr;
                    last_op = SEQUENTIAL;
                    break;
                } else if (ptr[0] == '&' && ptr[1] == '&') {
                    end = ptr;
                    last_op = AND;
                    break;
                } else if (ptr[0] == '|' && ptr[1] == '|') {
                    end = ptr;
                    last_op = OR;
                    break;
                }
                ptr++;
            }
            //-------------------------------
            // If no operator was found, set end to end of line
            if (!end) {
                end = ptr + strlen(ptr);  // point to null terminator
                last_op = NONE;
            }

           
            int len = end - start;
            while (len > 0 && start[len - 1] == ' ') len--;  // trim trailing spaces

            char *command = (char *)malloc(len + 1);
            strncpy(command, start, len);
            command[len] = '\0';

            // Save command and operator
            chunks[chunk_count].command = command;
            chunks[chunk_count].op = chunk_count == 0 ? NONE : chunks[chunk_count - 1].op;
            if (chunk_count > 0) chunks[chunk_count - 1].op = last_op; // set the *previous* op

            chunk_count++;

            // Move pointer past the operator
            if (ptr[0] == ';') {
                ptr += 1;
            } else if ((ptr[0] == '&' && ptr[1] == '&') || (ptr[0] == '|' && ptr[1] == '|')) {
                ptr += 2;
            }
       }

        // Print parsed tokens (debug)
        printf("Parsed Commands:\n");
        for (int i = 0; i < chunk_count; i++) {
            char *op_str = "";
            switch (chunks[i].op) {
                case NONE:       op_str = "NONE"; break;
                case SEQUENTIAL: op_str = "SEQUENTIAL ';'"; break;
                case AND:        op_str = "AND '&&'"; break;
                case OR:         op_str = "OR '||'"; break;
            }
            printf("  [%d] Command: '%s'  Operator: %s\n", i, chunks[i].command, op_str);
        }

        // Free memory
        for (int i = 0; i < chunk_count; i++) {
            free(chunks[i].command);
        }

        return 0;
    }
