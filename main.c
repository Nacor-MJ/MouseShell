#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char **get_input(char*);
#define BUFFER_LEN 100
int main() {
    printf("Welcome to MSchell\n");
    
    pid_t child_pid;
    char input[BUFFER_LEN];
    char **command;
    
    while (1) {
        printf("~~~~(Ɛ:> ");
        fgets(input, BUFFER_LEN, stdin);
        
        input[(strlen(input) - 1)] = '\0';

        command = get_input(input);

        if (strcmp(command[0], "cd") == 0) {
            if (chdir(command[1]) < 0) {
                perror(command[1]);
                continue;
            }
            command[0] = "pwd";
            command[1] = NULL;
        } else if (strcmp(command[0], "exit") == 0) {
            printf("Goodbye <3\n");
            exit(0);
        } else if (strcmp(command[0], "helpme") == 0) {
            printf("Type 'exit' to exit\n");
            continue;
        }

        child_pid = fork();
        if (child_pid == 0) { // if this is the child process
            if (execvp(command[0], command) < 0) {
                perror(command[0]);
                exit(1);
            }
        } else if (child_pid < 0) {
            perror("Fork failed");
            exit(1);
        } else { // main process
            int stat_loc;
            pid_t wait_result = waitpid(child_pid, &stat_loc, WUNTRACED);
        }
        free(command);
    }
    
    free(input);
    
    return 0;
}

char **get_input(char* input) {
    int buffer_len = 8;
    // the output char array
    char **command = malloc(buffer_len * sizeof(char *));
    if (command == NULL) {
        perror("Malloc failed");
        exit(1);
    }
    // commands are separated by a space
    char *separator = " ";
    // the currently parse word
    char *parsed;
    // index :)
    int index = 0;

    // first word
    parsed = strtok(input, separator);
    while (parsed != NULL) {
        // assign the word to the command array
        command[index] = parsed;
        index++;

        // as from the man page
        // In each subsequent call that should parse the same string, str must be NULL.
        parsed = strtok(NULL, separator);
        
        // if the buffer is not large enough 
        if (index > buffer_len) {
            printf("realloc\n");
            buffer_len *= 2;
            command = realloc(command, buffer_len * sizeof(char *));
            if (command == NULL) {
                perror("realloc failed");
                exit(1);
            }
        }
    }
    // NULL terminated thing
    command[index] = NULL;
    return command;
}