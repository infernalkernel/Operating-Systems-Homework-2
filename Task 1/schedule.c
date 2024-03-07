#include <stdio.h>
#include <limits.h>

// Structure to represent a process
struct Process {
    int pid;            // Process ID
    int arrival_time;   // Arrival time
    int burst_time;     // Burst time
    int remaining_time; // Remaining time (used for SRJF)
};

// Test processes
struct Process processes[] = {
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
    int current_time = 0;       // Initialize current time
    int total_turnaround_time = 0;  // Total turnaround time
    int total_response_time = 0;    // Total response time
    printf("Process Execution Order (SJF):\n");
    for (int i = 0; i < num_processes; ++i) {
        int shortest_burst_index = -1;  // Index of process with shortest burst time
        int shortest_burst_time = INT_MAX;  // Shortest burst time
        // Find the shortest burst time among the arrived processes
        for (int j = 0; j < num_processes; ++j) {
            if (processes[j].arrival_time <= current_time && processes[j].burst_time < shortest_burst_time && processes[j].burst_time > 0) {
                shortest_burst_time = processes[j].burst_time;
                shortest_burst_index = j;
            }
        }
        // If no process with burst time shorter than current time, move to the next arrival time
        if (shortest_burst_index == -1) {
            current_time = processes[i].arrival_time;
            --i;  // Decrement i to process the current arrival time in the next iteration
            continue;
        }
        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\n", processes[shortest_burst_index].pid, processes[shortest_burst_index].burst_time);
        total_response_time += current_time - processes[shortest_burst_index].arrival_time;
        current_time += processes[shortest_burst_index].burst_time;
        // Calculate turnaround time
        int turnaround_time = current_time - processes[shortest_burst_index].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[shortest_burst_index].pid, turnaround_time);
        // Mark the process as completed
        processes[shortest_burst_index].burst_time = INT_MAX;
    }
    // Print average turnaround time and response time
    printf("Average Turnaround Time for SJF: %.2f\n", (float)total_turnaround_time / num_processes);
    printf("Average Response Time for SJF: %.2f\n", (float)total_response_time / num_processes);
}

// Function to implement SRJF scheduling
void schedule_processes_SRJF(struct Process processes[], int num_processes) {
    int current_time = 0;       // Initialize current time
    int total_turnaround_time = 0;  // Total turnaround time
    int total_response_time = 0;    // Total response time
    printf("Process Execution Order (SRJF):\n");
    while (1) {
        int shortest_burst_index = -1;  // Index of process with shortest remaining burst time
        int shortest_burst_time = INT_MAX;  // Shortest remaining burst time
        // Find the process with the shortest remaining burst time
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time < shortest_burst_time && processes[i].remaining_time > 0) {
                shortest_burst_time = processes[i].remaining_time;
                shortest_burst_index = i;
            }
        }
        // If no process with remaining burst time shorter than current time, break the loop
        if (shortest_burst_index == -1) {
            break;
        }
        // Execute the process
        printf("Executing Process %d (Remaining Time: %d)\n", processes[shortest_burst_index].pid, processes[shortest_burst_index].remaining_time);
        total_response_time += current_time - processes[shortest_burst_index].arrival_time;
        current_time += processes[shortest_burst_index].remaining_time;
        // Calculate turnaround time
        int turnaround_time = current_time - processes[shortest_burst_index].arrival_time;
        total_turnaround_time += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[shortest_burst_index].pid, turnaround_time);
        // Update remaining time for other processes
        processes[shortest_burst_index].remaining_time = INT_MAX;
    }
    // Print average turnaround time and response time
    printf("Average Turnaround Time for SRJF: %.2f\n", (float)total_turnaround_time / num_processes);
    printf("Average Response Time for SRJF: %.2f\n", (float)total_response_time / num_processes);
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
