#include <stdio.h>
#include <stdlib.h>

struct Process {
    char *pid;
    int arrivalTime;
    int jobTime;
    int remainingTime;
    int queueIndex;
};

int compareProcessesByRemainingTime(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;
    return processA->remainingTime - processB->remainingTime;
}

int compareProcessesByArrivalTime(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;
    return processA->arrivalTime - processB->arrivalTime;
}

void swap(struct Process *x, struct Process *y) {
    struct Process temp = *x;
    *x = *y;
    *y = temp;
}

void minHeapify(struct Process *heap, int size, int i) {
    int smallest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if(left < size && heap[left].remainingTime < heap[smallest].remainingTime)
        smallest = left;

    if(right < size && heap[right].remainingTime < heap[smallest].remainingTime)
        smallest = right;

    if(smallest != i) {
        swap(&heap[i], &heap[smallest]);
        minHeapify(heap, size, smallest);
    }
}

void buildMinHeap(struct Process *heap, int size) {
    for(int i = (size / 2) - 1; i >= 0; i--) {
        minHeapify(heap, size, i);
    }
}

struct Process extractMin(struct Process *heap, int *size) {
    struct Process minProcess = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    minHeapify(heap, *size, 0);
    return minProcess;
}

void scheduleFCFS(struct Process *processes, int numProcesses) {
    int currentTime = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;
    for (int i = 0; i < numProcesses; i++) {
        struct Process *currentProcess = &processes[i];

        int responseTime = currentTime - currentProcess->arrivalTime;
        totalResponseTime += responseTime;

        currentTime += currentProcess->jobTime;

        int turnaroundTime = currentTime - currentProcess->arrivalTime;
        totalTurnaroundTime += turnaroundTime;

        printf("%s %d %d ", currentProcess->pid, currentTime - currentProcess->jobTime, currentTime);
    }

    double avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    double avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.2f %.2f\n", avgTurnaroundTime, avgResponseTime);
}

void scheduleRR(struct Process *processes, int numProcesses, int timeSlice) {
    int currentTime = 0;
    int completedProcesses = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;

    for (int i = 0; i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].jobTime;
    }

    while (completedProcesses < numProcesses) {
        int foundProcess = 0;

        for (int i = 0; i < numProcesses; i++) {
            struct Process *currentProcess = &processes[i];

            if (currentProcess->remainingTime > 0 && currentProcess->arrivalTime <= currentTime) {
                foundProcess = 1;

                int responseTime = (currentProcess->remainingTime == currentProcess->jobTime) ? currentTime - currentProcess->arrivalTime : 0;
                totalResponseTime += responseTime;

                int executionTime;
                if(currentProcess->remainingTime <= timeSlice) executionTime = currentProcess->remainingTime;
                else executionTime = timeSlice;

                currentTime += executionTime;
                currentProcess->remainingTime -= executionTime;

                if (currentProcess->remainingTime == 0) {
                    completedProcesses++;
                    int turnaroundTime = currentTime - currentProcess->arrivalTime;
                    totalTurnaroundTime += turnaroundTime;
                }

                printf("%s %d %d ", currentProcess->pid, currentTime - executionTime, currentTime);
            }
        }

        if (!foundProcess) currentTime++;
    }

    double avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    double avgResponseTime = totalResponseTime / numProcesses;

    printf("\n");
    printf("%.2f %.2f\n", avgTurnaroundTime, avgResponseTime);
}

void scheduleSJF(struct Process *processes, int numProcesses) {
    int currentTime = 0;
    int heapSize = 0;
    int processIndex = 0;
    struct Process minHeap[numProcesses];

    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;

    while (processIndex < numProcesses || heapSize > 0) {
        while (processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime) {
            minHeap[heapSize] = processes[processIndex];
            heapSize++;
            processIndex++;
        }

        buildMinHeap(minHeap, heapSize);

        struct Process runningProcess = extractMin(minHeap, &heapSize);

        if (runningProcess.pid != NULL) {
            totalResponseTime += currentTime - runningProcess.arrivalTime;
            currentTime += runningProcess.remainingTime;
            totalTurnaroundTime += currentTime - runningProcess.arrivalTime;
            printf("%s %d %d ", runningProcess.pid, currentTime - runningProcess.remainingTime, currentTime);
        } else {
            if (processIndex < numProcesses) {
                currentTime = processes[processIndex].arrivalTime;
            } else {
                currentTime++;
            }
        }
    }

    double avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    double avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.2f %.2f\n", avgTurnaroundTime, avgResponseTime);
}


void scheduleSRTF(struct Process *processes, int numProcesses) {
    int currentTime = 0;
    int processIndex = 0;
    int heapSize = 0;
    struct Process minHeap[numProcesses];
}


void scheduleMLFQ(struct Process *processes, int numProcesses, int TsMLFQ1, int TsMLFQ2, int TsMLFQ3, int BMLFQ) {
    // Implement MLFQ here
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        printf("Invalid number of arguments.\n");
        return 1;
    }

    char *inputFilePath = argv[1];
    char *outputFilePath = argv[2];
    int TsRR = atoi(argv[3]);
    int TsMLFQ1 = atoi(argv[4]);
    int TsMLFQ2 = atoi(argv[5]);
    int TsMLFQ3 = atoi(argv[6]);
    int BMLFQ = atoi(argv[7]);

    FILE *inputFile = fopen(inputFilePath, "r");
    // FILE *outputFile = freopen(outputFilePath, "w", stdout);


    int numProcesses = 0;

    while (!feof(inputFile)) {
        char pid[50];
        int startTime, runTime;
        if (fscanf(inputFile, "%s %d %d", pid, &startTime, &runTime) == 3) {
            numProcesses++;
        }
    }
    
    rewind(inputFile);

    struct Process *processes = malloc(numProcesses * sizeof(struct Process));
    if (processes == NULL) {
        printf("Memory allocation error.\n");
        fclose(inputFile);
        return 1;
    }

    for (int i = 0; i < numProcesses; i++) {
        processes[i].pid = malloc(50 * sizeof(char));
        fscanf(inputFile, "%s %d %d", processes[i].pid, &processes[i].arrivalTime, &processes[i].jobTime);
    }
    qsort(processes, numProcesses, sizeof(struct Process), compareProcessesByArrivalTime);
    // scheduleFCFS(processes, numProcesses);
    // scheduleRR(processes, numProcesses, TsRR);
    scheduleSJF(processes, numProcesses);
    // scheduleSRTF(processes, numProcesses);
    // scheduleMLFQ(processes, numProcesses, TsMLFQ1, TsMLFQ2, TsMLFQ3, BMLFQ);

    // Free allocated memory
    free(processes);

    // Close files
    fclose(inputFile);
    // fclose(outputFile);

    return 0;
}

