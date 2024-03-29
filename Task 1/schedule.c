#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// Structure to represent a process
struct Process {
    int pid;            // Process ID
    int arrival_time;   // Arrival time
    int burst_time;     // Burst time
    int remaining_time; // Remaining time (used for SRJF)
};

// Test processes
struct Process processes[] = {
    // {1, 0, 10, 10},
    // {2, 2 ,5 , 5},
    // {3, 4, 8, 8}
    {1, 0, 8, 8},
    {2, 1, 5, 5},
    {3, 2, 10, 10},
    {4, 3, 3, 3},
    {5, 4, 6, 6},
    {6, 5, 7, 7},
    {7, 6, 4, 4},
    {8, 7, 9, 9},
    {9, 8, 2, 2},
    {10, 9, 5, 5},
    {11, 10, 6, 6},
    {12, 11, 8, 8},
    {13, 12, 4, 4},
    {14, 13, 7, 7},
    {15, 14, 3, 3},
    {16, 15, 6, 6},
    {17, 16, 5, 5},
    {18, 17, 4, 4},
    {19, 18, 7, 7},
    {20, 19, 9, 9}
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

    // Free the dynamically allocated memory
    free(is_first_response);
    free(process_execution_started);
}

// Main function
int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    printf("--- FIFO Scheduling ---\n");
    schedule_processes_FIFO(processes, num_processes);

    printf("\n--- SJF Scheduling ---\n");
    schedule_processes_SJF(processes, num_processes);

    printf("\n--- SRJF Scheduling ---\n");
    schedule_processes_SRJF(processes, num_processes);

    return 0;
}
