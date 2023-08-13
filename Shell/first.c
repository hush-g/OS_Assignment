#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

bool IsArg (char arg[20], char check[20]){
    if (strcmp(arg, check) == 0){
        return true;
    }
    return false;
}

void RunCommand (char arguments[20][20]) {
    char *myargs[20];

    if (IsArg(arguments[0], "ls") || IsArg(arguments[0], "cat")){
        // if the input is ls command
        int i = 0;
        for (; !IsArg(arguments[i], ""); i++){
            myargs[i] = strdup(arguments[i]);
        }
        myargs[i] = NULL;

        execvp(myargs[0], myargs);

        // If execvp fails (this code is reached only if execvp fails)
        perror("execvp");
    }else {
        printf("command not found: %s \n", arguments[0]);
    }
}

int main(int argc, char *argv[]){
    while (true) {

        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("Dir: %s MTL458 > ", cwd);

        // Reading the input
        char input[1024];
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove the newline character at the end, if present
            if (input[strlen(input) - 1] == '\n') {
                input[strlen(input) - 1] = '\0';
            }
        } else {
            printf("Error reading input.\n");
            continue;
        }

        // parsing the arguments and command from the input
        char *argument;
        char arguments[20][20]; // Assuming a maximum of 20 words, each up to 20 characters

        int argumentCount = 0;

        argument = strtok(input, " ");

        while (argument != NULL) {
            strcpy(arguments[argumentCount], argument);
            argument = strtok(NULL, " ");
            argumentCount++;
        }

        int rc = fork();

        if (rc < 0) { 
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            RunCommand(arguments);
        } else { 
            // parent
            int wc = wait(NULL);
            if (strcmp(arguments[0], "cat") == 0){
                printf("\n");
            }
        }
    }
    return 0;
}