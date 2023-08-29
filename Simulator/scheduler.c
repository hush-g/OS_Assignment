#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX_RUNTIME 10

struct Process {
    int pid;
    int startTime;
    int runTime;
};

struct CPU {
    int startTime;
    int endTime;
};

struct ProcessList {
    struct Process *processes;
    int numProcesses;
};

struct Simulator {
    double lambda1;
    double lambda2;
    struct ProcessList processes;
};

struct Simulator *createSimulator(double lambda1, double lambda2) {
    struct Simulator *s = malloc(sizeof(struct Simulator));
    s->lambda1 = lambda1;
    s->lambda2 = lambda2;
    s->processes.processes = NULL;
    s->processes.numProcesses = 0;
    return s;
}

struct Process *createProcess(int pid, int startTime, int runTime) {
    struct Process *p = malloc(sizeof(struct Process));
    p->pid = pid;
    p->startTime = startTime;
    p->runTime = runTime;
    return p;
}

void addProcess(struct Simulator *s, struct Process *p) {
    s->processes.numProcesses++;
    s->processes.processes = realloc(s->processes.processes, s->processes.numProcesses * sizeof(struct Process));
    s->processes.processes[s->processes.numProcesses - 1] = *p;
}

void destroySimulator(struct Simulator *s) {
    free(s->processes.processes);
    free(s);
}

void createProcesses(struct Simulator *s, int numProcesses) {
    int timeNow = 0;
    for (int i = 0; i < numProcesses; i++) {
        timeNow += (int)(-floor(log(1 - drand48()) / s->lambda1));
        
        int runTime = (int)(1 + (MAX_RUNTIME - 1) * drand48());

        struct Process *p = createProcess(i + 1, timeNow, runTime);
        addProcess(s, p);
    }
}

void showProcesses(struct Simulator *s) {
    printf("PID\tEntryTime\tTimeToCompletion\n");
    for (int i = 0; i < s->processes.numProcesses; i++) {
        struct Process p = s->processes.processes[i];
        printf("%d\t%d\t\t%d\n", p.pid, p.startTime, p.runTime);
    }
}


// Function to schedule processes using the First Come First Serve (FCFS) strategy
void scheduleFCFS(struct Simulator *s) {
    struct ProcessList *processList = &(s->processes);
    int numProcesses = processList->numProcesses;
    struct Process *processes = processList->processes;

    int currentTime = 0;
    struct CPU cpu;
    cpu.startTime = currentTime;
    cpu.endTime = 0;

    for (int i = 0; i < numProcesses; i++) {
        // Update the current time to the maximum of the process start time and the current time
        currentTime = MAX(currentTime, processes[i].startTime);

        // Set the end time of the CPU as the sum of current time and the process run time
        cpu.endTime = currentTime + processes[i].runTime;

        // Print the scheduling information
        printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
               processes[i].pid, currentTime, cpu.endTime);

        // Update the current time to the end time of the CPU
        currentTime = cpu.endTime;
    }
}

// Helper function to sort processes based on their runTime in ascending order
int compareProcessesByRunTime(const void *a, const void *b) {
    return ((struct Process *)a)->runTime - ((struct Process *)b)->runTime;
}

// Function to schedule processes using the Shortest Job First (SJF) strategy
void scheduleSJF(struct Simulator *s) {
    struct ProcessList *processList = &(s->processes);
    int numProcesses = processList->numProcesses;
    struct Process *processes = processList->processes;

    // Sort processes based on their runTime
    qsort(processes, numProcesses, sizeof(struct Process), compareProcessesByRunTime);

    int currentTime = 0;

    for (int i = 0; i < numProcesses; i++) {
        // Update the current time to the maximum of the process start time and the current time
        currentTime = MAX(currentTime, processes[i].startTime);

        // Set the end time of the CPU as the sum of current time and the process run time
        int endTime = currentTime + processes[i].runTime;

        // Print the scheduling information
        printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
               processes[i].pid, currentTime, endTime);

        // Update the current time to the end time of the CPU
        currentTime = endTime;
    }
}

// Function to schedule processes using the Round Robin (RR) strategy
void scheduleRR(struct Simulator *s, int timeSlice) {
    struct ProcessList *processList = &(s->processes);
    int numProcesses = processList->numProcesses;
    struct Process *processes = processList->processes;

    int currentTime = 0;
    int queueFront = 0;

    while (queueFront < numProcesses) {
        // Get the next process from the queue
        struct Process *currentProcess = &processes[queueFront];

        // Update the current time to the maximum of the process start time and the current time
        currentTime = MAX(currentTime, currentProcess->startTime);

        // Calculate the remaining time for the process after executing for the time slice
        int remainingTime = currentProcess->runTime - timeSlice;

        if (remainingTime <= 0) {
            // The process completes within the time slice
            int endTime = currentTime + currentProcess->runTime;
            printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
                   currentProcess->pid, currentTime, endTime);

            // Move to the next process in the queue
            queueFront++;
            currentTime = endTime;
        } else {
            // The process still needs more time to complete
            int endTime = currentTime + timeSlice;
            printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
                   currentProcess->pid, currentTime, endTime);

            // Update the start time of the process for the next iteration
            currentProcess->startTime = endTime;

            // Move the current process to the end of the queue
            queueFront = (queueFront + 1) % numProcesses;

            // Update the current time to the end time of the CPU
            currentTime = endTime;
        }
    }
}

// // Comparator function for the min heap
// int compareProcessesByRunTime(const void *a, const void *b) {
//     return ((struct Process *)a)->runTime - ((struct Process *)b)->runTime;
// }

// Function to schedule processes using the Shortest Time-to-Completion (STC) strategy
void scheduleSTC(struct Simulator *s) {
    struct ProcessList *processList = &(s->processes);
    int numProcesses = processList->numProcesses;
    struct Process *processes = processList->processes;

    int currentTime = 0;
    int processIndex = 0;

    // Sort the processes by run time using a min heap
    qsort(processes, numProcesses, sizeof(struct Process), compareProcessesByRunTime);

    while (processIndex < numProcesses) {
        // Get the next process with the shortest run time
        struct Process *currentProcess = &processes[processIndex];

        // Update the current time to the maximum of the process start time and the current time
        currentTime = MAX(currentTime, currentProcess->startTime);

        // Calculate the end time for the current process
        int endTime = currentTime + currentProcess->runTime;

        printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
               currentProcess->pid, currentTime, endTime);

        // Move to the next process in the sorted order
        processIndex++;
        currentTime = endTime;
    }
}

// Function to schedule processes using the Multi-Level Feedback Queue (MLFQ) strategy
void scheduleMLFQ(struct Simulator *s, int slice1, int slice2, int slice3, int boost) {
    struct ProcessList *processList = &(s->processes);
    int numProcesses = processList->numProcesses;
    struct Process *processes = processList->processes;

    int currentTime = 0;
    int processIndex = 0;

    while (processIndex < numProcesses) {
        // printf("numProcesses = %d, processIndex = %d\n", numProcesses, processIndex);
        // Check if the boost time has been reached, reset the queues if needed
        if (currentTime >= boost) {
            // printf("Boosting at time %d\n", currentTime);
            processIndex = 0;
            currentTime += boost;
        }

        // Find the minimum start time among all available processes
        int nextStartTime = INT_MAX;
        for (int i = processIndex; i < numProcesses; i++) {
            if (processes[i].startTime < nextStartTime) {
                nextStartTime = processes[i].startTime;
            }
        }

        // Increment currentTime to the nextStartTime only if it's greater
        if (nextStartTime > currentTime) {
            currentTime = nextStartTime;
        }

        // Increment currentTime to the nextStartTime
        currentTime = nextStartTime;

        // Queue 1: Time slice = slice1
        while (processIndex < numProcesses && processes[processIndex].startTime <= currentTime) {
            // Schedule processes in Queue 1
            struct Process *currentProcess = &processes[processIndex];
            int endTime = currentTime + MIN(slice1, currentProcess->runTime);

            printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
                   currentProcess->pid, currentTime, endTime);

            currentTime = endTime;
            processIndex++;
        }

        // Queue 2: Time slice = slice2
        while (processIndex < numProcesses && processes[processIndex].startTime <= currentTime) {
            // Schedule processes in Queue 2
            struct Process *currentProcess = &processes[processIndex];
            int endTime = currentTime + MIN(slice2, currentProcess->runTime);

            printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
                   currentProcess->pid, currentTime, endTime);

            currentTime = endTime;
            processIndex++;
        }

        // Queue 3: Time slice = slice3
        while (processIndex < numProcesses && processes[processIndex].startTime <= currentTime) {
            // Schedule processes in Queue 3
            struct Process *currentProcess = &processes[processIndex];
            int endTime = currentTime + MIN(slice3, currentProcess->runTime);

            printf("Scheduled process %d: Start Time = %d, End Time = %d\n",
                   currentProcess->pid, currentTime, endTime);

            currentTime = endTime;
            processIndex++;
        }
    }
}


int main() {

    // FILE *outputFile = fopen("scheduling_info.txt", "w");
    // if (outputFile == NULL) {
    //     printf("Error opening file for writing.\n");
    //     return 1;
    // }

    srand48(time(NULL));

    struct Simulator *simulator = createSimulator(0.2, 0.3);

    printf("Enter number of processes: ");
    int numProcesses;
    scanf("%d", &numProcesses);
    createProcesses(simulator, numProcesses);

    showProcesses(simulator);

    int choice;
    printf("Choose scheduler:\n");
    printf("1. FCFS\n");
    printf("2. SJF\n");
    printf("3. RR\n");
    printf("4. STC\n");
    printf("5. MLFQ\n");
    printf("6. All\n");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleFCFS(simulator);
            break;
        case 2:
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleSJF(simulator);
            break;
        case 3:
            printf("Enter RR time slice: ");
            int timeSlice;
            scanf("%d", &timeSlice);
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleRR(simulator, timeSlice);
            break;
        case 4:
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleSTC(simulator);
            break;
        case 5:
            printf("Enter MLFQ slice times and boost: ");
            int slice1, slice2, slice3, boost;
            scanf("%d %d %d %d", &slice1, &slice2, &slice3, &boost);
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleMLFQ(simulator, slice1, slice2, slice3, boost);
            break;
        case 6:
            printf("Enter RR time slice: ");
            int rrTimeSlice;
            scanf("%d", &rrTimeSlice);
            printf("Enter MLFQ slice times and boost: ");
            int mlfqSlice1, mlfqSlice2, mlfqSlice3, mlfqBoost;
            scanf("%d %d %d %d", &mlfqSlice1, &mlfqSlice2, &mlfqSlice3, &mlfqBoost);
            // freopen("scheduling_info.txt", "w", stdout);
            scheduleFCFS(simulator);
            scheduleSJF(simulator);
            scheduleRR(simulator, rrTimeSlice);
            scheduleSTC(simulator);
            scheduleMLFQ(simulator, mlfqSlice1, mlfqSlice2, mlfqSlice3, mlfqBoost);
            break;
        default:
            printf("Invalid choice.\n");
            // freopen("scheduling_info.txt", "w", stdout);
    }

    // Clean up
    // fclose(outputFile);
    destroySimulator(simulator);

    return 0;
}
