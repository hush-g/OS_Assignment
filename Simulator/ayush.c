#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

struct Process {
    int id;
    char pid[5];
    float arrivalTime;  // Change data type to float
    float burstTime;    // Change data type to float
    bool completed;
    // float remainingTime;
    // this->completed=0;
}; 

void swap(struct Process *a, struct Process *b) {
    struct Process temp = *a;
    *a = *b;
    *b = temp;
}

// Function to calculate average turnaround time
float calculateAvgTurnaroundTime(struct Process processes[], int n, float completionTime[]) {
    float totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalTurnaroundTime += completionTime[i] - processes[i].arrivalTime;
    }
    return totalTurnaroundTime / n;
}

// Function to calculate average response time
float calculateAvgResponseTime(float responseTime[], int n) {
    float totalResponseTime = 0;
    for (int i = 0; i < n; i++) {
        totalResponseTime += responseTime[i];
    }
    return totalResponseTime / n;
}

// Function for FCFS scheduling
void fcfs(struct Process processes[], int n, FILE *outputFile) {
    // float completionTime[n];
    // float responseTime[n];
    // dynamic arrays
    float *completionTime = (float *)malloc(n * sizeof(float));
    float *responseTime = (float *)malloc(n * sizeof(float));
    responseTime[0] = 0;
    completionTime[0] = processes[0].arrivalTime + processes[0].burstTime;

    for (int i = 1; i < n; i++) {
        if(completionTime[i-1]<processes[i].arrivalTime)
        {
            completionTime[i] = processes[i].arrivalTime + processes[i].burstTime;
        }
        else
        {
            completionTime[i] = completionTime[i - 1] + processes[i].burstTime;
        }
        if(processes[i].arrivalTime>completionTime[i-1])
        {
            responseTime[i]=0;
        }
        else
        {
            responseTime[i]=completionTime[i-1]-processes[i].arrivalTime;
        }
    }

    float avgTurnaroundTime = calculateAvgTurnaroundTime(processes, n, completionTime);
    float avgResponseTime = calculateAvgResponseTime(responseTime, n);

    // fprintf(outputFile, "FCFS blue print\n");

    // Print Gantt chart to the output file
    for (int i = 0; i < n; i++) {
        fprintf(outputFile, "%s %.3f %.3f ", processes[i].pid, completionTime[i] - processes[i].burstTime, completionTime[i]);
    }
    
    fprintf(outputFile, "\n%.3f ", avgTurnaroundTime);
    fprintf(outputFile, "%.3f\n", avgResponseTime);
    free(completionTime);
    free(responseTime);
}

void sjf(struct Process processes[], int n, FILE* outputFile) {
    float currentTime = 0;
    int completed = 0;

    for (int i = 0; i < n; i++) {
        processes[i].id = i; // Assign unique IDs
        processes[i].completed = false;
    }

    // fprintf(outputFile, "SJF blue print\n");

    // float completionTime[n];
    // float responseTime[n];
    // dynamic arrays
    float *completionTime = (float *)malloc(n * sizeof(float));
    float *responseTime = (float *)malloc(n * sizeof(float));
    responseTime[0] = 0;

    while (completed < n) {
        int shortestJob = -1;
        float shortestBurst = FLT_MAX;

        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime &&
                processes[i].burstTime < shortestBurst) {
                shortestJob = i;
                shortestBurst = processes[i].burstTime;
            }
        }

        if (shortestJob == -1) {
            float nextArrival = FLT_MAX;
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed && processes[i].arrivalTime > currentTime &&
                    processes[i].arrivalTime < nextArrival) {
                    nextArrival = processes[i].arrivalTime;
                }
            }
            currentTime = nextArrival;
        } else {
            fprintf(outputFile, "%s %.3f %.3f ",
                    processes[shortestJob].pid, currentTime,
                    currentTime + processes[shortestJob].burstTime);
            completionTime[shortestJob] = currentTime + processes[shortestJob].burstTime;
            responseTime[shortestJob] = currentTime - processes[shortestJob].arrivalTime;
            currentTime += processes[shortestJob].burstTime;
            processes[shortestJob].completed = true;
            completed++;
        }
    }

    float avgTurnaroundTime = 0;
    float avgResponseTime = 0;

    for (int i = 0; i < n; i++) {
        avgTurnaroundTime += completionTime[i] - processes[i].arrivalTime;
        avgResponseTime += responseTime[i];
    }

    avgTurnaroundTime /= n;
    avgResponseTime /= n;

    fprintf(outputFile, "\n%.3f ", avgTurnaroundTime);
    fprintf(outputFile, "%.3f\n", avgResponseTime);
    free(completionTime);
    free(responseTime);
}

// Function for Shortest Remaining Time First (SRTF) scheduling
void srtf(struct Process processes[], int n, FILE *outputFile) {
    // fprintf(outputFile, "Shortest Remaining Time First (SRTF) blueprint\n");

    // Sort processes by arrival time
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime) {
                swap(&processes[j], &processes[j + 1]);
            } else if (processes[j].arrivalTime == processes[j + 1].arrivalTime) {
                if (processes[j].burstTime > processes[j + 1].burstTime) {
                    swap(&processes[j], &processes[j + 1]);
                }
            }
        }
    }

    // float remainingTime[n];
    // dynamic allocation
    float *remainingTime = (float *)calloc(n, sizeof(float));
    for(int i = 0; i < n; i++) {
        remainingTime[i] = processes[i].burstTime;
        processes[i].id = i;
        processes[i].completed = false;
    }
    
    int shortestJob = 0;
    float currentTime = processes[shortestJob].arrivalTime;
    // int firsttime[n];
    // dynamic allocation
    int *firsttime = (int *)calloc(n, sizeof(int));
    // memset(firsttime,0,sizeof(firsttime));
    // int vis[n];
    // dynamic allocation
    int *vis= (int *)calloc(n, sizeof(int));
    // memset(vis, 0, sizeof(vis));
    vis[shortestJob] = 1;
    int totalVis = 0;
    fprintf(outputFile, "%s %.3f ", processes[shortestJob].pid, currentTime);

    float *completionTime = (float *)calloc(n, sizeof(float));
    float *responseTime = (float *)calloc(n, sizeof(float));

    if (completionTime == NULL || responseTime == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        // Handle memory allocation error
        exit(1);
    }

    while (totalVis < n) {
        if (shortestJob == -1) {
            int ind = -1;
            for (int i = 0; i < n; i++) {
                if ((vis[i] == 1 && processes[i].completed == 0) && (ind == -1 || remainingTime[i] < remainingTime[ind])) {
                    ind = i;
                }
            }
            shortestJob = ind;
            if (shortestJob == -1) {
                for (int i = 0; i < n; i++) {
                    if (vis[i] == 0) {
                        currentTime = processes[i].arrivalTime;
                        vis[i] = 1;
                        shortestJob = i;
                        break;
                    }
                }
            }
            fprintf(outputFile, "%s %.3f ", processes[shortestJob].pid, currentTime);
            if(firsttime[shortestJob]==0){
                responseTime[shortestJob] = currentTime - processes[shortestJob].arrivalTime;
                firsttime[shortestJob]=1;
            }
        }

        if (shortestJob == n - 1) {
            totalVis++;
            processes[shortestJob].completed = 1;
            vis[shortestJob] = 2;
            currentTime += remainingTime[shortestJob];
            completionTime[shortestJob] = currentTime;
            if(firsttime[shortestJob]==0){
                responseTime[shortestJob] = currentTime - processes[shortestJob].arrivalTime;
                firsttime[shortestJob]=1;
            }
            fprintf(outputFile, "%.3f ", currentTime);
            shortestJob = -1;
            continue;
        }

        for (int i = shortestJob + 1; i < n; i++) {
            if (vis[i] != 0 && i != n - 1) continue;
            else if (vis[i] != 0) {
                totalVis++;
                processes[shortestJob].completed = 1;
                vis[shortestJob] = 2;
                currentTime += remainingTime[shortestJob];
                completionTime[shortestJob] = currentTime;
                shortestJob = -1;
                fprintf(outputFile, "%.3f ", currentTime);//crt
                break;
            }

            if (currentTime + remainingTime[shortestJob] > processes[i].arrivalTime + remainingTime[i]) {
                vis[i] = 1;
                remainingTime[shortestJob] -= (processes[i].arrivalTime) - currentTime;
                
                currentTime = processes[i].arrivalTime;
                fprintf(outputFile, "%.3f ", currentTime);//crt
                shortestJob = i;
                if(firsttime[shortestJob]==0){
                    responseTime[shortestJob] = currentTime - processes[shortestJob].arrivalTime;
                    firsttime[shortestJob]=1;
                }
                fprintf(outputFile, "%s %.3f ", processes[shortestJob].pid, currentTime);//crt
            } else {
                if (processes[i].arrivalTime > currentTime + remainingTime[shortestJob] || i == n - 1) {
                    if (i == n - 1 && (processes[i].arrivalTime <= currentTime + remainingTime[shortestJob])) vis[i] = 1;
                    totalVis++;
                    processes[shortestJob].completed = 1;
                    vis[shortestJob] = 2;
                    currentTime += remainingTime[shortestJob];
                    completionTime[shortestJob] = currentTime;
                    shortestJob = -1;
                    fprintf(outputFile, "%.3f ", currentTime);
                    break;
                } else vis[i] = 1;
            }
        }
    }

    float avgTurnaroundTime = calculateAvgTurnaroundTime(processes, n, completionTime);
    float avgResponseTime = calculateAvgResponseTime(responseTime, n);

    fprintf(outputFile, "\n%.3f ", avgTurnaroundTime);
    fprintf(outputFile, "%.3f\n", avgResponseTime);
    free(completionTime);
    free(responseTime);
    free(remainingTime);
    free(vis);
    free(firsttime);
}

float max(float a, float b) {
    return (a > b) ? a : b;
}

float min(float a, float b) {
    return (a < b) ? a : b;
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

void rr(struct Process processes[], int n, FILE *outputFile, float timeSlice){
    float currentTime = 0;
    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;
    int completedProcesses = 0;
    // float turnaroundTime[n];
    // float responseTime[n];
    // create dynamic arrays
    float *turnaroundTime = (float *)malloc(n * sizeof(float));
    float *responseTime = (float *)malloc(n * sizeof(float));
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    
    // float remainingTime[n];
    float *remainingTime = (float *)malloc(n * sizeof(float));
    for(int i = 0; i < n; i++) {
        remainingTime[i] = processes[i].burstTime;
        processes[i].id = i;
        // fprintf(outputFile,"\n%s %.3f %.3f\n",processes[i].pid, remainingTime[i], processes[i].burstTime);
    }

    initializeQueue(queue);

    int processIndex = 0;
    
    while (!isEmpty(queue) || processIndex < n) {
        if(isEmpty(queue)) {
            currentTime = max(currentTime, processes[processIndex].arrivalTime);
        }
        for(; processIndex < n && processes[processIndex].arrivalTime <= currentTime + timeSlice; processIndex++) {
            enqueue(queue, &processes[processIndex]);
        }
        
        struct Process *currentProcess = dequeue(queue);
        int id = currentProcess->id;
        currentTime=max(currentTime,currentProcess->arrivalTime);

        if(currentProcess->burstTime == remainingTime[id]) { // First time running
            responseTime[id] = currentTime - currentProcess->arrivalTime;
            totalResponseTime += responseTime[id];
        }

        if(remainingTime[id] <= timeSlice || completedProcesses == n-1) { // Process finished or last process, so no need to divide into slices
            // if(id==1)fprintf(outputFile,"\n%s %.3f %.3f\n",currentProcess->pid, remainingTime[id], currentTime);
            currentTime += remainingTime[id];
            turnaroundTime[id] = currentTime - currentProcess->arrivalTime;
            totalTurnaroundTime += turnaroundTime[id];

            fprintf(outputFile,"%s %.3f %.3f ", currentProcess->pid, currentTime - remainingTime[id], currentTime);
            remainingTime[id] = 0;
            completedProcesses++;
        } else {  
            remainingTime[id] -= timeSlice;
            currentTime += timeSlice;
            enqueue(queue, currentProcess);

             fprintf(outputFile,"%s %.3f %.3f ", currentProcess->pid, currentTime - timeSlice, currentTime);
        }
    }


    double avgTurnaroundTime = totalTurnaroundTime / n;
    double avgResponseTime = totalResponseTime / n;

    fprintf(outputFile,"\n%.3f %.3f\n", avgTurnaroundTime, avgResponseTime);
    free(turnaroundTime);
    free(responseTime);
    free(remainingTime);
    
}

void mlfq(struct Process processes[],int n,FILE *outputFile, double ts_mlfq1, double ts_mlfq2, double ts_mlfq3, double ts_bmlfq)
{
    struct Queue queues[3]; // Three priority queues
    for (int i = 0; i < 3; i++)
    {
        initializeQueue(&queues[i]);
    }
    for(int i = 0; i < n; i++) {
        // remainingTime[i] = processes[i].burstTime;
        processes[i].id = i;
    }

    struct Queue jobs;
    initializeQueue(&jobs);
    for(int i=0;i<n;i++){
        // struct Node *node=(struct Node *)malloc(sizeof(struct Node));
        // node->data=&processes[i];
        enqueue(&jobs,&processes[i]);
        // node->
    }
    // int n = readFile(filePath, &jobs);

    // float *completionTime = (float *)calloc(n, sizeof(float));
    double currentTime = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;
    double timeSlice = ts_bmlfq;
    double timeQuantum[] = {ts_mlfq1, ts_mlfq2, ts_mlfq3};
    double nextboost=ts_bmlfq;

    int *firstvis = (int *)calloc(n, sizeof(int));
    // int firstvis[n];
    // memset(firstvis,0,sizeof(firstvis));
    int total = n;
    int n1 = 0;
    int lastjobid = -1;
    double lastjobtime=-1;
    while (n > 0)
    {
        // Check each queue in priority order

        if (isEmpty(&queues[0]) && isEmpty(&queues[1]) && isEmpty(&queues[2]) && currentTime < jobs.front->data->arrivalTime)
            currentTime = jobs.front->data->arrivalTime;

        for (int i = 0; i < 3; i++)
        {
            while (!isEmpty(&jobs) && jobs.front->data->arrivalTime <= currentTime)
            {
                struct Process *currentJob = dequeue(&jobs);
                enqueue(&queues[0], currentJob);
            }

            struct Process *currentJob;
            if(queues[i].front!=NULL)currentJob=queues[i].front->data;
            else currentJob=NULL;
            // currentJob-
            if (currentJob != NULL)
            {
                if(currentJob->burstTime==0){
                    dequeue(&queues[i]);
                    n--;
                    continue;
                }
                // print lastjobtime if current job id is different from last job id
                // if(currentJob->id==4)printf("\n%s %.3f %.3f\n", currentJob->pid, currentTime, currentJob->burstTime);
                if(lastjobtime!=-1 && lastjobid!=currentJob->id)fprintf(outputFile,"%.3f ", lastjobtime);

                if (firstvis[currentJob->id] ==0)
                {
                    // currentJob->startTime = currentTime;
                    totalResponseTime += currentTime - currentJob->arrivalTime; // Response time is the time the job first gets the CPU
                    firstvis[currentJob->id]=1;

                }

                if((int)((currentTime+min(currentJob->burstTime,timeQuantum[i])) / timeSlice) > n1){

                    // fprintf(outputFile,"boosted\n");
                    // fprintf(outputFile,"\n%s %.3f %dkk\n", currentJob->pid, currentTime,i+1);
                    currentJob->burstTime-=((n1+1)*timeSlice)-currentTime;
                    // if(currentJob->burstTime==0){
                        // completionTime[currentJob->id]=currentTime;
                    //     totalTurnaroundTime += currentTime - currentJob->arrivalTime;
                    // }
                    // if(currentJob->id==4)fprintf(outputFile,"\n%s %.3f %.3f\n", currentJob->pid, currentTime, currentJob->burstTime);
                    // currentTime=nextboost;
                    if(lastjobid!=currentJob->id)fprintf(outputFile,"%s %.3f ", currentJob->pid, currentTime);
                    currentTime=(n1+1)*timeSlice;
                    lastjobtime=currentTime;
                    lastjobid=currentJob->id;
                    if(currentJob->burstTime==0){
                        // completionTime[currentJob->id]=currentTime;
                        totalTurnaroundTime += currentTime - currentJob->arrivalTime;
                    }

                    // if (i == 2) enqueue(&queues[i], currentJob);
                    // else enqueue(&queues[i + 1], currentJob);

                    break;
                }
                dequeue(&queues[i]);
                if(lastjobid!=currentJob->id)fprintf(outputFile,"%s %.3f ", currentJob->pid, currentTime);
                lastjobid = currentJob->id;
                

                // fprintf(outputFile,"%s %.3lf ", currentJob->name, currentTime);
                if (currentJob->burstTime <= timeQuantum[i])
                {   
                    // if(currentJob->id==4)fprintf(outputFile,"\n k %s %.3f %.3f\n", currentJob->pid, currentTime, currentJob->burstTime);
                    currentTime += currentJob->burstTime;
                    // currentJob->completionTime = currentTime;
                    lastjobtime=currentTime;
                    // if(n==1)
                    // completionTime[currentJob->id]=currentTime;
                    totalTurnaroundTime += currentTime - currentJob->arrivalTime;
                    currentJob->burstTime = 0;

                    // fprintf(outputFile,"%.3f %d\n", currentTime,n);
                    n--;
                    // cout<<n<<endl;
                    // fprintf(outputFile,"\n%s %d\n", currentJob->pid,n);
                }

                else
                {
                    // Job needs more time, so it goes to the next lower-priority queue
                    currentTime += timeQuantum[i];
                    // fprintf(outputFile,"%.3f ", currentTime);
                    lastjobtime=currentTime;
                    currentJob->burstTime -= timeQuantum[i];
                    if (i == 2)
                        enqueue(&queues[i], currentJob);

                    else
                        enqueue(&queues[i + 1], currentJob);
                }

                // fprintf(outputFile,"%.3lf ", currentTime);
                
                break; // Process only one job from the current queue
            }
            
        }

        if ((int)(currentTime / timeSlice) > n1)
        {
             while (!isEmpty(&jobs) && jobs.front->data->arrivalTime <= currentTime)
            {
                struct Process *currentJob = dequeue(&jobs);
                enqueue(&queues[0], currentJob);
            }
            while (!isEmpty(&queues[1]))
            {
                struct Process *currentJob = dequeue(&queues[1]);
                enqueue(&queues[0], currentJob);
            }

            while (!isEmpty(&queues[2]))
            {
                struct Process *currentJob = dequeue(&queues[2]);
                enqueue(&queues[0], currentJob);
            }
            n1++;
        }
        
    }
    fprintf(outputFile,"%.3f ", currentTime);

    double avgTurnaroundTime = (double)totalTurnaroundTime / total;
    double avgResponseTime = (double)totalResponseTime / total;

    fprintf(outputFile,"\n%.3lf %.3lf", avgTurnaroundTime, avgResponseTime);
    free(firstvis);
}

// Custom comparison function to compare two processes
int compareProcesses(const void* a, const void* b) {
    const struct Process* processA = (const struct Process*)a;
    const struct Process* processB = (const struct Process*)b;

    if (processA->arrivalTime != processB->arrivalTime) {
        return processA->arrivalTime - processB->arrivalTime;
    } else {
        return processA->burstTime - processB->burstTime;
    }
}

// Merge Sort implementation
void merge(struct Process* arr, int left, int middle, int right) {
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 = right - middle;

    // Create temporary arrays
    // struct Process L[n1], R[n2];
    // create dynamic arrays
    struct Process *L = (struct Process *)malloc(n1 * sizeof(struct Process));
    struct Process *R = (struct Process *)malloc(n2 * sizeof(struct Process));

    // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (compareProcesses(&L[i], &R[j]) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Merge Sort function
void mergeSort(struct Process* arr, int left, int right) {
    if (left < right) {
        // Same as (left+right)/2, but avoids overflow for large left and right
        int middle = left + (right - left) / 2;

        // Sort first and second halves
        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);

        // Merge the sorted halves
        merge(arr, left, middle, right);
    }
}
int main(int argc, char *argv[]) {
    if (argc != 8) {
        printf("Usage: %s input.txt output.txt TsRR TsMLFQ1 TsMLFQ2 TsMLFQ3 BMLFQ\n", argv[0]);
        return 1;
    }

    // Extract command line arguments
    char *inputFilePath = argv[1];
    char *outputFilePath = argv[2];
    float timeQuantumRR = atof(argv[3]);
    float timeQuantumMLFQ1 = atof(argv[4]);
    float timeQuantumMLFQ2 = atof(argv[5]);
    float timeQuantumMLFQ3 = atof(argv[6]);
    float boostParameterMLFQ = atof(argv[7]);

    // Open input file
    FILE *inputFile = fopen(inputFilePath, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Open output file
    FILE *outputFile = fopen(outputFilePath, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    // Read input and process data
    int n = 0;
    struct Process processes[(1<<16)]; // Assuming a maximum of 100 processes
    while (fscanf(inputFile, "%s %f %f", processes[n].pid, &processes[n].arrivalTime, &processes[n].burstTime) == 3) {
        n++;
    }

    // Sort processes by arrival time
    mergeSort(processes, 0, n - 1);

    // Call FCFS scheduling function
    fcfs(processes, n, outputFile);

    // Call RR scheduling function with specified time quantum
    rr(processes, n, outputFile, timeQuantumRR);
    
    // Call SJF scheduling function
    sjf(processes, n, outputFile);

    // Call SRTF scheduling function
    srtf(processes, n, outputFile);

    // Call MLFQ scheduling function with specified time slices and boost parameter
    mlfq(processes, n, outputFile, timeQuantumMLFQ1, timeQuantumMLFQ2, timeQuantumMLFQ3, boostParameterMLFQ);

    // fcfs(processes, n, outputFile);

    // Close files
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}