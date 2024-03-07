#include <stdio.h>
#include <stdlib.h>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority; // Priority level (higher value indicates higher priority)
};

//Globals implemented to store turnaround/response times for final avergae calculation
float globalAverageTurnAroundTime;
float globalAverageResponseTime;

// Example test processes
struct Process processes[] = {
    {11, 10, 6, 6,  1},
    {12, 11, 8, 8, 3},
    {13, 12, 4, 4, 2},
    {14, 13, 7, 7, 1},
    {15, 14, 3, 3, 3},
    {16, 15, 6, 6, 2},
    {17, 16, 5, 5, 1},
    {18, 17, 4, 4, 3},
    {19, 18, 7, 7, 2},
    {20, 19, 9, 9, 1}
};

// Function to implement FIFO scheduling
void schedule_processes_FIFO(struct Process processes[], int num_processes) {
    int current_time = 0;       // Initialize current time
    int total_turnaround_time = 0;  // Total turnaround time
    int total_response_time = 0;    // Total response time
    printf("Process Execution Order (FIFO):\n");

    for (int i = 0; i < num_processes; ++i) {
        // Wait until the process arrives (if needed)
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\n", processes[i].pid, processes[i].burst_time);
        total_response_time += current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;

        // Calculate turnaround time
        int turnaround_time = current_time - processes[i].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[i].pid, turnaround_time);
    }

    // Print average turnaround time and response time
    printf("Average Turnaround Time for FIFO: %.2f\n", (float)total_turnaround_time / num_processes);
    printf("Average Response Time for FIFO: %.2f\n", (float)total_response_time / num_processes);

    globalAverageTurnAroundTime += ((float)total_turnaround_time / num_processes);
    globalAverageResponseTime += ((float)total_response_time / num_processes);
}

// Function to implement SJF scheduling
void schedule_processes_SJF(struct Process processes[], int num_processes) {
    int current_time = 0;  // Initialize current time
    int total_turnaround_time = 0;  // Total turnaround time
    int total_response_time = 0;  // Total response time
    int *process_finished = (int *)malloc(num_processes * sizeof(int));  // To track completion of processes

    if (process_finished == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    memset(process_finished, 0, num_processes * sizeof(int));  // Initialize completion status array

    printf("Process Execution Order (SJF):\n");

    while (1) {
        int shortest_burst_index = -1;  // Index of process with shortest burst time
        int shortest_burst_time = INT_MAX;  // Shortest burst time

        for (int i = 0; i < num_processes; ++i) {
            if (!process_finished[i] && processes[i].arrival_time <= current_time && processes[i].burst_time < shortest_burst_time) {
                shortest_burst_index = i;
                shortest_burst_time = processes[i].burst_time;
            }
        }

        if (shortest_burst_index == -1) {
            // Check if all processes are completed to break the loop
            int all_finished = 1;
            for (int i = 0; i < num_processes; ++i) {
                if (!process_finished[i]) {
                    all_finished = 0;
                    break;
                }
            }
            if (all_finished) {
                break;
            }

            current_time++;  // Advance time if no process is ready to execute
            continue;
        }

        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\n", processes[shortest_burst_index].pid, processes[shortest_burst_index].burst_time);
        total_response_time += current_time - processes[shortest_burst_index].arrival_time;
        current_time += processes[shortest_burst_index].burst_time;
        process_finished[shortest_burst_index] = 1;  // Mark the process as finished

        // Calculate turnaround time
        int turnaround_time = current_time - processes[shortest_burst_index].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[shortest_burst_index].pid, turnaround_time);
    }

    // Print average turnaround time and response time
    printf("Average Turnaround Time for SJF: %.2f\n", (float)total_turnaround_time / num_processes);
    printf("Average Response Time for SJF: %.2f\n", (float)total_response_time / num_processes);

    globalAverageTurnAroundTime += ((float)total_turnaround_time / num_processes);
    globalAverageResponseTime += ((float)total_response_time / num_processes);

    free(process_finished);  // Free the dynamically allocated memory
}

// Function to implement SRJF scheduling
void schedule_processes_SRJF(struct Process processes[], int num_processes) {
    int current_time = 0;  // Initialize current time
    int total_turnaround_time = 0;  // Total turnaround time
    int total_response_time = 0;  // Total response time

    // Dynamically allocate memory to track first response and start times for each process
    int *is_first_response = (int *)malloc(num_processes * sizeof(int));
    int *process_execution_started = (int *)malloc(num_processes * sizeof(int));

    if (is_first_response == NULL || process_execution_started == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    memset(is_first_response, 0, num_processes * sizeof(int));
    memset(process_execution_started, 0, num_processes * sizeof(int));

    printf("Process Execution Order (SRJF):\n");

    int processes_completed = 0;
    while (processes_completed < num_processes) {
        int shortest_burst_index = -1;
        int shortest_burst_time = INT_MAX;

        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time < shortest_burst_time && processes[i].remaining_time > 0) {
                shortest_burst_index = i;
                shortest_burst_time = processes[i].remaining_time;
            }
        }

        if (shortest_burst_index == -1) {
            current_time++;  // Increment time if no process is ready to execute
            continue;
        }

        // Track the process being executed
        struct Process *current_process = &processes[shortest_burst_index];

        if (is_first_response[shortest_burst_index] == 0) {
            is_first_response[shortest_burst_index] = 1;
            process_execution_started[shortest_burst_index] = current_time;
        }

        // Execute the process
        current_process->remaining_time--;
        current_time++;  // Increment current time as process has executed for a time unit

        if (current_process->remaining_time == 0) {
            processes_completed++;
            int turnaround_time = current_time - current_process->arrival_time;
            total_turnaround_time += turnaround_time;
            total_response_time += process_execution_started[shortest_burst_index] - current_process->arrival_time;

            printf("Executing Process %d (Burst Time: %d)\n", current_process->pid, current_process->burst_time);
            printf("Turnaround Time for Process %d: %d\n", current_process->pid, turnaround_time);
        }
    }

    printf("Average Turnaround Time for SRJF: %.2f\n", (float)total_turnaround_time / num_processes);
    printf("Average Response Time for SRJF: %.2f\n", (float)total_response_time / num_processes);

    globalAverageTurnAroundTime += ((float)total_turnaround_time / num_processes);
    globalAverageResponseTime += ((float)total_response_time / num_processes);

    // Free the dynamically allocated memory
    free(is_first_response);
    free(process_execution_started);
}

// Function to implement MLQ scheduling
void schedule_processes(struct Process processes[], int num_processes) {
    // Separate queues for different priority levels
    struct Process *high_priority_queue = malloc(num_processes * sizeof(struct Process));
    struct Process *medium_priority_queue = malloc(num_processes * sizeof(struct Process));
    struct Process *low_priority_queue = malloc(num_processes * sizeof(struct Process));
    int high_size = 0, medium_size = 0, low_size = 0;

    // Distribute processes to respective queues based on priority
    for (int i = 0; i < num_processes; ++i) {
        switch (processes[i].priority) {
            case 3:
                high_priority_queue[high_size++] = processes[i];
                break;
            case 2:
                medium_priority_queue[medium_size++] = processes[i];
                break;
            case 1:
                low_priority_queue[low_size++] = processes[i];
                break;
        }
    }

    // Execute processes from each queue using appropriate scheduling algorithms
    if (high_size > 0) {
        printf("Process Execution Order (High Priority - SRJF):\n");
        schedule_processes_SRJF(high_priority_queue, high_size);
    }
    if (medium_size > 0) {
        printf("\nProcess Execution Order (Medium Priority - SJF):\n");
        schedule_processes_SJF(medium_priority_queue, medium_size);
    }
    if (low_size > 0) {
        printf("\nProcess Execution Order (Low Priority - FIFO):\n");
        schedule_processes_FIFO(low_priority_queue, low_size);
    }

    // Free the dynamically allocated memory
    free(high_priority_queue);
    free(medium_priority_queue);
    free(low_priority_queue);
}

int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    schedule_processes(processes, num_processes);
    printf("\nAverage Total Turnaround time:  %.2f\n", globalAverageTurnAroundTime / 3);
    printf("Average Total Response time:  %.2f\n", globalAverageResponseTime / 3);
    return 0;
}