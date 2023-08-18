#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>

#define MAX_COMMANDS 20
#define MAX_COMMAND_LENGTH 2048
#define MAX_ARGS 64

typedef struct {
    char **data;
    size_t size;
    size_t capacity;
} Vector;

void init(Vector *vector, size_t initialCapacity) {
    vector->data = (char **)malloc(initialCapacity * sizeof(char *));
    vector->size = 0;
    vector->capacity = initialCapacity;
}

void addHistory(Vector *vector, char *command) {
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->data = (char **)realloc(vector->data, vector->capacity * sizeof(char *));
    }
    
    vector->data[vector->size] = strdup(command);
    vector->size++;
}

void clearHistory(Vector *vector) {
    for (size_t i = 0; i < vector->size; i++) {
        free(vector->data[i]);
    }
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

void printHistory(Vector *vector,  size_t n) {
    if (n > vector->size) {
        n = vector->size;
    }
    int counter = 1;
    for (size_t i = vector->size - n; i < vector->size; i++) {
        printf("%d. %s\n", counter++, vector->data[i]);
    }
}

char ** getHistory(Vector *vector, size_t n) {
    if (n > vector->size) {
        n = vector->size;
    }
    char **history = (char **)malloc(n * sizeof(char *));
    for (size_t i = vector->size - n; i < vector->size; i++) {
        history[i] = vector->data[i];
    }
    return history;
}

int getNum(char *arg) {
    char *endptr;
    long num = strtol(arg, &endptr, 10);

    if (endptr == arg) {
        printf("Input incorrect: Not a valid integer\n");
        return -1;
    } else if (*endptr != '\0') {
        printf("Input incorrect: Invalid character: %c\n", *endptr);
        return -1;
    }
    if(num < 0) {
        printf("Input incorrect: Negative number\n");
        return -1;
    }
    return (int)num;
}

void processArguments(char *command, char *args[MAX_ARGS]) {
    int arg_count = 0;
    char *token = strtok(command, " ");
    while (token != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

}

void executeCommand(char *args[MAX_ARGS]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: Missing argument\n");
        } else {
            if (strcmp(args[1], "~") == 0) {
                const char *home_dir = getenv("HOME");
                if (home_dir != NULL) {
                    chdir(home_dir);
                } else {
                    fprintf(stderr, "cd: Failed to get home directory\n");
                }
            } else {
                chdir(args[1]);
            }
        }
    } else {
        pid_t pid = fork();
        if(pid < 0) {
            printf("\nFork Failed");
        } else if(pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
            if (strcmp(args[0], "cat") == 0) {
                printf("\n");
            }
        }
    }
}

void executePipedCommands(Vector *history, char *commands[][MAX_COMMAND_LENGTH], int numCommands) {
    int pipes[numCommands - 1][2];
    pid_t pids[numCommands];

    for(int i = 0; i < numCommands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < numCommands; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            if (i < numCommands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            for (int j = 0; j < numCommands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            char *args[MAX_ARGS];
            processArguments(*commands[i], args);
            if(strcmp(args[0], "history") == 0) {
                int num = getNum(args[1]);
                if(num >= 0) {
                    printHistory(history, num);
                    char **historyData = getHistory(history, num);
                    for (int k = 0; k < num; k++) {
                        write(STDOUT_FILENO, historyData[k], strlen(historyData[k]));
                        write(STDOUT_FILENO, "\n", 1);
                    }
                }
            } else if (strcmp(args[0], "exit") == 0) {
                clearHistory(history);
                exit(0);
            } else {
                execvp(args[0], args);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork");
            return;
        }

        if (i < numCommands - 1) {
            close(pipes[i][1]);
        }
        if (i > 0) {
            close(pipes[i - 1][0]);
        }
    }

    for (int i = 0; i < numCommands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < numCommands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}

int main() {
    char *username = getenv("USER");

    Vector history;
    init(&history, 10);

    while (1) {  
        char currentDirectory[MAX_COMMAND_LENGTH];
	    getcwd(currentDirectory, sizeof(currentDirectory));
	    printf("%s:%s MTL 458 > ", username, currentDirectory);

        char inputString[MAX_COMMAND_LENGTH];
        int numCommands = 0;
        char *commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
        fgets(inputString, sizeof(inputString), stdin);
        inputString[strcspn(inputString, "\n")] = '\0';

        if(inputString[0] == '\0') {
            continue;
        }

        addHistory(&history, inputString);        

        char *token = strtok(inputString, "|");
        while (token != NULL && numCommands < MAX_COMMANDS) {
            commands[numCommands++][0] = token;
            token = strtok(NULL, "|");
        }

        int isPipe = 0;

	    if (numCommands < 2) {
		    isPipe = 0;
	    } else {
		    isPipe = 1;
	    }
        if(isPipe) {
            executePipedCommands(&history, commands, numCommands);
        } else {
            char *args[MAX_ARGS];
            processArguments(*commands[0], args);
            if(strcmp(args[0], "history") == 0) {
                int num = getNum(args[1]);
                if(num >= 0) printHistory(&history, num);
            } else if (strcmp(args[0], "exit") == 0) {
                clearHistory(&history);
                exit(0);
            } else {
                executeCommand(args);
            }
        }
    }
    clearHistory(&history);
    return 0;
}
