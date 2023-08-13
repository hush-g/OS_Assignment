#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <errno.h>

#define MAX_COMMANDS 20
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64

void processArguments(char *command, char *args[MAX_ARGS]) {
    int arg_count = 0;
    char *token = strtok(command, " ");
    while (token != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // Terminate the argument list

}

int executeSystemCommand(char *commands[][MAX_COMMAND_LENGTH]) {
    char *args[MAX_ARGS];
    processArguments(*commands[0], args);

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: Missing argument\n");
        } else {
            chdir(args[1]);
        }
        return 1;
    } else if (strcmp(args[0], "ls") == 0 || strcmp(args[0], "echo") == 0 || strcmp(args[0], "cat") == 0) {
        if (strcmp(args[0], "cat") == 0 && args[1] == NULL) {
            fprintf(stderr, "cat: Missing argument\n");
            return 1;
        }

        // Forking a child
        pid_t pid = fork();
        if(pid < 0) {
            printf("\nFailed forking child..");
            return 0;
        } else if(pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            // waiting for child to terminate
            wait(NULL);
            if (strcmp(args[0], "cat") == 0) {
                printf("\n");
            }
            return 1;
        }
        printf("\n");
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "sleep") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "sleep: Missing argument\n");
        } else {
            int seconds = atoi(args[1]);
            sleep(seconds);
        }
        return 1;
    }
    return 0;
}

void executeCommand(char *commands[][1024]) {
    printf("exec com");
    // Forking a child
    pid_t pid = fork();
    char **args = commands[0];
    if(pid < 0) {
        printf("\nFailed forking child..");
        return;
    } else if(pid == 0) {
        if(execvp(args[0], args) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

void executePipedCommands(char *commands[][1024], int num_commands) {
    printf("exec pipe");
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
            // exit(EXIT_FAILURE);
        }
    }

    // Execute commands
    for (int i = 0; i < num_commands; i++) {
        char **args = commands[i];
        int arg_count = 0;
        while (args[arg_count] != NULL) {
            arg_count++;
        }

        if (i < num_commands - 1) {
            // Redirect stdout to pipe write end
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (i > 0) {
            // Redirect stdin to pipe read end
            dup2(pipes[i - 1][0], STDIN_FILENO);
            close(pipes[i - 1][0]);
            close(pipes[i - 1][1]);
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execvp(args[0], args);
            perror("execvp");
            // break;
            // exit(EXIT_FAILURE);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork");
            // break;
            // exit(EXIT_FAILURE);
        }
    }

    // Close remaining pipes in parent
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for child processes to finish
    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}

int main() {
    char *username = getenv("USER");
    int count = 0;
    while (1) {  
        count++;
        char currentDirectory[1024];
	    getcwd(currentDirectory, sizeof(currentDirectory));
	    printf("%s:%s MTL 458 > ", username, currentDirectory);

        char inputString[MAX_COMMAND_LENGTH];
        int numCommands = 0;
        char *commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
        fgets(inputString, sizeof(inputString), stdin);
        inputString[strcspn(inputString, "\n")] = '\0'; // Remove newline

        // add_history(inputString);

        // Convert input into commands and arguments
        char *token = strtok(inputString, "|");
        while (token != NULL && numCommands < MAX_COMMANDS) {
            commands[numCommands++][0] = token;
            token = strtok(NULL, "|");
        }

        int isPipe = 0;

	    if (numCommands < 2) {
		    isPipe = 0; // zero if no pipe is found.
	    } else {
		    isPipe = 1;
	    }
        if(executeSystemCommand(commands) == 0) {
            if(isPipe) {
                executePipedCommands(commands, numCommands);
            } else {
                executeCommand(commands);
            }
        }
    }
    printf("**%d***", count);
    return 0;
}
