#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the proc structure
typedef struct proc {
    char name[256];
    int priority;
    int pid;
    int runtime;
} proc;

// Define the queue structure
typedef struct queue {
    proc process;
    struct queue* next;
} queue;

// Function prototypes
void push(queue** head, proc process);
void printQueue(queue* head);
void freeQueue(queue** head);

int main(void) {
    FILE* file;
    char line[512];
    queue* head = NULL;

    // Open the processes.txt file
    file = fopen("processes.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        proc newProcess;
        // Parse the line into the proc structure
        if (sscanf(line, "%255[^,],%d,%d,%d", newProcess.name, &newProcess.priority, &newProcess.pid, &newProcess.runtime) == 4) {
            // Add the process to the queue
            push(&head, newProcess);
        }
    }

    fclose(file);

    // Print the processes in the queue
    printQueue(head);

    // Free the allocated memory
    freeQueue(&head);

    return 0;
}

// Function to add a process to the queue
void push(queue** head, proc process) {
    queue* newNode = (queue*)malloc(sizeof(queue));
    if (newNode == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    newNode->process = process;
    newNode->next = *head;
    *head = newNode;
}

// Function to print the queue
void printQueue(queue* head) {
    queue* current = head;
    while (current != NULL) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", current->process.name, current->process.priority, current->process.pid, current->process.runtime);
        current = current->next;
    }
}

// Function to free the allocated memory for the queue
void freeQueue(queue** head) {
    queue* current = *head;
    queue* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}
