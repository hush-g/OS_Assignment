#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(int argc, char *argv[])
        {
                printf("First: %d \n", (int) getpid());
                int rc = fork();
                if (rc == 0) {
        printf("Second: (pid:%d)\n", (int) getpid());
        char *myargs[3];
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        myargs[2] = NULL;
        execvp(myargs[0], myargs);
        printf("Second: %d\n", (int) getpid());
} else {
        wait(NULL);
printf("First: %d\n Second:%d\n", rc, (int) getpid());
} return 0;
}