#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int MAX_VALUE_FOR_JOBTIME;
int MAX_VALUE_FOR_ARRIVALTIME;

float max(float a, float b) {
    return (a > b) ? a : b;
}

struct Process {
    char pid[10];
    float arrivalTime;
    float jobTime;

    float remainingTime;
    float responseTime;
    float turnaroundTime;
    float currentSwitchRunTime;
};

struct CPU {
    char PID[10];
    float startTime;
    float endTime;
};

struct CPUArray {
    struct CPU *entries;
    int size;
    int capacity;
};

struct CPUArray *initializeCPUArray() {
    struct CPUArray *cpuArray = malloc(sizeof(struct CPUArray));
    if (cpuArray == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    cpuArray->size = 0;
    cpuArray->capacity = 10;
    cpuArray->entries = malloc(cpuArray->capacity * sizeof(struct CPU));
    if (cpuArray->entries == NULL) {
        printf("Memory allocation error.\n");
        free(cpuArray);
        exit(1);
    }
    return cpuArray;
}

void addCPUEntry(struct CPUArray *cpuArray, char *pid, float startTime, float endTime) {
    if (cpuArray->size >= cpuArray->capacity) {
        cpuArray->capacity *= 2;

        cpuArray->entries = realloc(cpuArray->entries, cpuArray->capacity * sizeof(struct CPU));
        if (cpuArray->entries == NULL) {
            printf("Memory allocation error.\n");
            free(cpuArray);
            exit(1);
        }
    }

    if (cpuArray->size > 0 && strcmp(cpuArray->entries[cpuArray->size - 1].PID, pid) == 0) {
        cpuArray->entries[cpuArray->size - 1].endTime = endTime;
    } else {
        struct CPU cpuEntry;
        strncpy(cpuEntry.PID, pid, sizeof(cpuEntry.PID) - 1);
        cpuEntry.PID[sizeof(cpuEntry.PID) - 1] = '\0'; // Ensure null-terminated string
        cpuEntry.startTime = startTime;
        cpuEntry.endTime = endTime;

        cpuArray->entries[cpuArray->size++] = cpuEntry;
    }
}

void printCPUArray(struct CPUArray *cpuArray) {
    for (int i = 0; i < cpuArray->size; i++) {
        printf("%s %.2f %.2f ", cpuArray->entries[i].PID, cpuArray->entries[i].startTime, cpuArray->entries[i].endTime);
    }
}

void freeCPUArray(struct CPUArray *cpuArray) {
    free(cpuArray->entries);
    free(cpuArray);
}


int compareProcessesByArrivalTime(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;
    return processA->arrivalTime - processB->arrivalTime;
}

void swap(struct Process *x, struct Process *y) {
    struct Process temp = *x; *x = *y; *y = temp;
}

void minHeapify(struct Process *heap, int size, int i) {
    int smallest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if(left < size && heap[left].remainingTime < heap[smallest].remainingTime) smallest = left;

    if(right < size && heap[right].remainingTime < heap[smallest].remainingTime) smallest = right;

    if(smallest != i) {
        swap(&heap[i], &heap[smallest]);
        minHeapify(heap, size, smallest);
    }
}

void buildMinHeap(struct Process *heap, int size) {
    for(int i = (size / 2) - 1; i >= 0; i--) minHeapify(heap, size, i);
}

struct Process extractMin(struct Process *heap, int *size) {
    struct Process minProcess = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    minHeapify(heap, *size, 0);
    return minProcess;
}

struct Node {
    struct Process *data;
    struct Node *next;
};

struct Queue {
    struct Node *front;
    struct Node *rear;
};

void initializeQueue(struct Queue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

int isEmpty(struct Queue *queue) {
    return queue->front == NULL;
}

void enqueue(struct Queue *queue, struct Process *process) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = process;
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

struct Process *dequeue(struct Queue *queue) {
    if (isEmpty(queue)) return NULL;

    struct Node *temp = queue->front;
    struct Process *data = temp->data;
    if(queue->front == queue->rear) {
        queue->front = NULL;
        queue->rear = NULL;
    } else queue->front = temp->next;

    free(temp);
    return data;
}

void scheduleFCFS(struct Process *processes, int numProcesses) {
    float currentTime = 0;
    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;

    struct CPUArray *cpuArray = initializeCPUArray();
    
    for (int i = 0; i < numProcesses; i++) {
        struct Process *currentProcess = &processes[i];

        if(currentTime < currentProcess->arrivalTime) currentTime = currentProcess->arrivalTime;

        currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
        totalResponseTime += currentProcess->responseTime;

        currentTime += currentProcess->jobTime;

        currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime;
        totalTurnaroundTime += currentProcess->turnaroundTime;
        
        addCPUEntry(cpuArray, currentProcess->pid, currentTime - currentProcess->jobTime, currentTime);
        // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - currentProcess->jobTime, currentTime);
    }

    printCPUArray(cpuArray);
    freeCPUArray(cpuArray);

    double avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    double avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.3f %.3f\n", avgTurnaroundTime, avgResponseTime);
}

void scheduleRR(struct Process *processes, int numProcesses, int timeSlice) {
    float currentTime = 0;
    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;
    int completedProcesses = 0;
    
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    
    struct CPUArray *cpuArray = initializeCPUArray();

    if(queue == NULL) {
        printf("Memory allocation error.\n");
        return;
    }

    for(int i = 0; i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].jobTime;
    }

    initializeQueue(queue);

    int processIndex = 0;
    
    while (!isEmpty(queue) || processIndex < numProcesses) {
        if(isEmpty(queue)) {
            currentTime = max(currentTime, processes[processIndex].arrivalTime);
        }
        for(; processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime + timeSlice; processIndex++) {
            enqueue(queue, &processes[processIndex]);
        }
        
        struct Process *currentProcess = dequeue(queue);

        if(currentProcess->jobTime == currentProcess->remainingTime) { // First time running
            currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
            totalResponseTime += currentProcess->responseTime;
        }

        if(currentProcess->remainingTime <= timeSlice || completedProcesses == numProcesses-1) { // Process finished or last process, so no need to divide into slices
            currentTime += currentProcess->remainingTime;
            currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime;
            totalTurnaroundTime += currentProcess->turnaroundTime;

            addCPUEntry(cpuArray, currentProcess->pid, currentTime - currentProcess->remainingTime, currentTime);
            // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - currentProcess->remainingTime, currentTime);
            currentProcess->remainingTime = 0;
            completedProcesses++;
        } else { // Process not finished, enqueue again
            currentProcess->remainingTime -= timeSlice;
            currentTime += timeSlice;
            enqueue(queue, currentProcess);

            addCPUEntry(cpuArray, currentProcess->pid, currentTime - timeSlice, currentTime);
            // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - timeSlice, currentTime);
        }
    }

    printCPUArray(cpuArray);
    freeCPUArray(cpuArray);

    double avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    double avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.3f %.3f\n", avgTurnaroundTime, avgResponseTime);
    
}

void scheduleSJF(struct Process *processes, int numProcesses) {
    float currentTime = 0;
    int heapSize = 0;
    int processIndex = 0;
    struct Process minHeap[numProcesses];

    struct CPUArray *cpuArray = initializeCPUArray();

    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;

    for(int i = 0;  i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].jobTime;
        processes[i].currentSwitchRunTime = 0;
    }

    while (processIndex < numProcesses || heapSize > 0) {
        if(heapSize == 0) currentTime = max(currentTime, processes[processIndex].arrivalTime);
        for (; processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime; processIndex++) {
            minHeap[heapSize] = processes[processIndex];
            heapSize++;
        }

        buildMinHeap(minHeap, heapSize);

        struct Process runningProcess = extractMin(minHeap, &heapSize);

        if(runningProcess.jobTime == runningProcess.remainingTime) { // First time running
            runningProcess.responseTime = currentTime - runningProcess.arrivalTime;
            totalResponseTime += runningProcess.responseTime;
        }

        currentTime += runningProcess.jobTime;

        runningProcess.turnaroundTime = currentTime - runningProcess.arrivalTime;
        totalTurnaroundTime += runningProcess.turnaroundTime;

        addCPUEntry(cpuArray, runningProcess.pid, currentTime - runningProcess.jobTime, currentTime);
        // printf("%s %.2f %.2f ", runningProcess.pid, currentTime - runningProcess.jobTime, currentTime);
    }

    printCPUArray(cpuArray);
    freeCPUArray(cpuArray);

    float avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    float avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.2f %.2f\n", avgTurnaroundTime, avgResponseTime);
}


void scheduleSRTF(struct Process *processes, int numProcesses) {
    float currentTime = processes[0].arrivalTime;
    int heapSize = 0;
    int processIndex = 0;
    int completedProcesses = 0;
    struct Process minHeap[numProcesses];

    struct CPUArray *cpuArray = initializeCPUArray();

    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;

    for(int i = 0;  i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].jobTime;
        processes[i].currentSwitchRunTime = 0;
    }

    while(processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime) { // Adding processes with smallest arrival time
        minHeap[heapSize] = processes[processIndex];
        heapSize++;
        processIndex++;
    }

    buildMinHeap(minHeap, heapSize);

    while(completedProcesses < numProcesses) {
        if(heapSize == 0) currentTime = max(currentTime, processes[processIndex].arrivalTime);
        while (processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime) {
            minHeap[heapSize] = processes[processIndex];
            heapSize++;
            processIndex++;
        }

        buildMinHeap(minHeap, heapSize);

        float nextArrivalTime = __INT_MAX__;
        float nextJobTime = __INT_MAX__;
        if(processIndex < numProcesses) {
            nextArrivalTime = processes[processIndex].arrivalTime;
            nextJobTime = processes[processIndex].jobTime;
        }

        struct  Process runningProcess = extractMin(minHeap, &heapSize);

        if(runningProcess.jobTime == runningProcess.remainingTime) { // First time running
            runningProcess.responseTime = currentTime - runningProcess.arrivalTime;
            totalResponseTime += runningProcess.responseTime;
        }
        
        if(runningProcess.remainingTime <= nextArrivalTime - currentTime) { // Process finished
            runningProcess.currentSwitchRunTime += runningProcess.remainingTime;
            currentTime += runningProcess.remainingTime;
            runningProcess.remainingTime = 0;
            runningProcess.turnaroundTime = currentTime - runningProcess.arrivalTime;
            totalTurnaroundTime += runningProcess.turnaroundTime;
            completedProcesses++;

            addCPUEntry(cpuArray, runningProcess.pid, currentTime - runningProcess.currentSwitchRunTime, currentTime);
            // printf("%s %.2f %.2f ", runningProcess.pid, currentTime - runningProcess.currentSwitchRunTime, currentTime);
        } else if(runningProcess.remainingTime - (nextArrivalTime - currentTime) < nextJobTime) { // New process came but it is longer
            runningProcess.currentSwitchRunTime += (nextArrivalTime - currentTime);
            runningProcess.remainingTime -= (nextArrivalTime - currentTime);
            currentTime = nextArrivalTime;
            minHeap[heapSize] = runningProcess;
            heapSize++;
        } else { // Process not finished and new process is shorter
            runningProcess.currentSwitchRunTime += nextArrivalTime - currentTime;
            runningProcess.remainingTime -= nextArrivalTime - currentTime;
            currentTime = nextArrivalTime;

            addCPUEntry(cpuArray, runningProcess.pid, currentTime - runningProcess.currentSwitchRunTime, currentTime);
            // printf("%s %.2f %.2f ", runningProcess.pid, currentTime - runningProcess.currentSwitchRunTime, currentTime);
            runningProcess.currentSwitchRunTime = 0;
            minHeap[heapSize] = runningProcess;
            heapSize++;
        }          
    }

    printCPUArray(cpuArray);
    freeCPUArray(cpuArray);

    float avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    float avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.2f %.2f\n", avgTurnaroundTime, avgResponseTime); 
}


void scheduleMLFQ(struct Process *processes, int numProcesses, float TsMLFQ1, float TsMLFQ2, float TsMLFQ3, float BMLFQ) {
    if(BMLFQ < TsMLFQ1 || BMLFQ < TsMLFQ2 || BMLFQ < TsMLFQ3) {
        printf("Invalid time quantum.\n");
        return;
    }

    struct CPUArray *cpuArray = initializeCPUArray();

    struct Queue *queue1 = (struct Queue *)malloc(sizeof(struct Queue));
    struct Queue *queue2 = (struct Queue *)malloc(sizeof(struct Queue));
    struct Queue *queue3 = (struct Queue *)malloc(sizeof(struct Queue));

    if(queue1 == NULL || queue2 == NULL || queue3 == NULL) {
        printf("Memory allocation error.\n");
        return;
    }

    initializeQueue(queue1);
    initializeQueue(queue2);
    initializeQueue(queue3);

    float currentTime = processes[0].arrivalTime;
    int completedProcesses = 0;
    int processIndex = 0;
    float nextBoostTime = BMLFQ;

    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;

    for(int i = 0; i < numProcesses; i++) {
        processes[i].remainingTime = processes[i].jobTime;
        processes[i].currentSwitchRunTime = 0;
    }

    int k = 0;
    while (completedProcesses < numProcesses) {

        for(; processIndex < numProcesses && processes[processIndex].arrivalTime <= currentTime; processIndex++) {
            enqueue(queue1, &processes[processIndex]);
        }
        if (!isEmpty(queue1)) {
            struct Process *currentProcess = queue1->front->data;

            if(currentProcess->remainingTime == currentProcess->jobTime) {
                currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
                totalResponseTime += currentProcess->responseTime;
            }

            if(currentProcess->remainingTime <= TsMLFQ1) {
                completedProcesses++;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                currentTime += currentProcess->remainingTime;
                currentProcess->remainingTime = 0;
                currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime;
                totalTurnaroundTime += currentProcess->turnaroundTime;
                dequeue(queue1);
            } else {
                currentTime += TsMLFQ1;
                currentProcess->remainingTime -= TsMLFQ1;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime - TsMLFQ1, currentTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - TsMLFQ1, currentTime);
                dequeue(queue1);
                enqueue(queue2, currentProcess);
            }

        } else if (!isEmpty(queue2)) {
            struct Process *currentProcess = queue2->front->data;

            if(currentProcess->remainingTime == currentProcess->jobTime) {
                currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
                totalResponseTime += currentProcess->responseTime;
            }

            if(currentProcess->remainingTime <= TsMLFQ2) {
                completedProcesses++;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                currentTime += currentProcess->remainingTime;
                currentProcess->remainingTime = 0;
                currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime;
                totalTurnaroundTime += currentProcess->turnaroundTime;
                dequeue(queue2);
            } else {
                currentTime += TsMLFQ2;
                currentProcess->remainingTime -= TsMLFQ2;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime - TsMLFQ2, currentTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - TsMLFQ2, currentTime);
                dequeue(queue2);
                enqueue(queue3, currentProcess);
            }

        } else if(!isEmpty(queue3)) {
            struct Process *currentProcess = queue3->front->data;

            if(currentProcess->remainingTime == currentProcess->jobTime) {
                currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
                totalResponseTime += currentProcess->responseTime;
            }

            if(currentProcess->remainingTime <= TsMLFQ3) {
                completedProcesses++;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime, currentTime + currentProcess->remainingTime);
                currentTime += currentProcess->remainingTime;
                currentProcess->remainingTime = 0;
                currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime;
                totalTurnaroundTime += currentProcess->turnaroundTime;
                dequeue(queue3);
            } else {
                currentTime += TsMLFQ3;
                currentProcess->remainingTime -= TsMLFQ3;
                addCPUEntry(cpuArray, currentProcess->pid, currentTime - TsMLFQ3, currentTime);
                // printf("%s %.2f %.2f ", currentProcess->pid, currentTime - TsMLFQ1, currentTime);
                dequeue(queue3);
                enqueue(queue3, currentProcess);
            }

        } else {
            float nextArrivalTime = __INT_MAX__;
            if(processIndex < numProcesses) {
                nextArrivalTime = processes[processIndex].arrivalTime;
            }
            currentTime = nextArrivalTime;
        }
        if(currentTime >= nextBoostTime) { // Boosting
            nextBoostTime += BMLFQ;
            while(!isEmpty(queue2)) {
                struct Process *currentProcess = dequeue(queue2);
                enqueue(queue1, currentProcess);
            }
            while(!isEmpty(queue3)) {
                struct Process *currentProcess = dequeue(queue3);
                enqueue(queue1, currentProcess);
            }
        }
    }

    printCPUArray(cpuArray);
    freeCPUArray(cpuArray);

    float avgTurnaroundTime = totalTurnaroundTime / numProcesses;
    float avgResponseTime = totalResponseTime / numProcesses;

    printf("\n%.2f %.2f\n", avgTurnaroundTime, avgResponseTime); 
}


struct Process generateRandomProcess(int pidNumber) {
    struct Process process;
    sprintf(process.pid, "J%d", pidNumber);
    process.arrivalTime = rand() % 10;
    process.jobTime = -log(1.0 - ((double)rand() / RAND_MAX)) * 25;
    return process;
}


struct Process* generateRandomProcesses(int numProcesses) {
    struct Process* processes = malloc(numProcesses * sizeof(struct Process));
    if (processes == NULL) {
        printf("Memory allocation error.\n");
        return NULL;
    }

    for (int i = 0; i < numProcesses; i++) {
        processes[i] = generateRandomProcess(i + 1);
    }

    return processes;
}

void runSchedulers(int numProcesses, float TsRR, float TsMLFQ1, float TsMLFQ2, float TsMLFQ3, float BMLFQ) {
    struct Process *processes = generateRandomProcesses(numProcesses);

    qsort(processes, numProcesses, sizeof(struct Process), compareProcessesByArrivalTime);

    scheduleFCFS(processes, numProcesses);
    scheduleRR(processes, numProcesses, TsRR);
    scheduleSJF(processes, numProcesses);
    scheduleSRTF(processes, numProcesses);
    scheduleMLFQ(processes, numProcesses, TsMLFQ1, TsMLFQ2, TsMLFQ3, BMLFQ);

}

int main(int argc, char *argv[]) {

    int numProcesses;
    printf("Enter Number Of Processes : ");
    scanf("%d", &numProcesses);
    printf("Enter MaxValue for JobTime : ");
    scanf("%d", &MAX_VALUE_FOR_JOBTIME);
    printf("Enter MaxValue for ArrivalTime : ");
    scanf("%d", & MAX_VALUE_FOR_ARRIVALTIME);

    FILE *outputFile = freopen("out.txt", "w", stdout);

    float TsRR = -log(1.0 - ((double)rand() / RAND_MAX)) * 10;
    float TsMLFQ1 = -log(1.0 - ((double)rand() / RAND_MAX)) * 10;
    float TsMLFQ2 = -log(1.0 - ((double)rand() / RAND_MAX)) * 15;
    float TsMLFQ3 = -log(1.0 - ((double)rand() / RAND_MAX)) * 20;
    float BMLFQ = -log(1.0 - ((double)rand() / RAND_MAX)) * 25;

    runSchedulers(numProcesses, TsRR, TsMLFQ1, TsMLFQ2, TsMLFQ3, BMLFQ);

    fclose(outputFile);

    return 0;
}

