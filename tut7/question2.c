#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
proc* pop(queue** head);
proc* delete_name(queue** head, char* name);
proc* delete_pid(queue** head, int pid);
void printProcess(proc* process);
void freeQueue(queue** head);

int main(void) {
    FILE* file;
    char line[512];
    queue* head = NULL;
    proc* tempProcess;

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

    // Perform deletions as specified
    tempProcess = delete_name(&head, "emacs");
    if (tempProcess != NULL) {
        printf("Deleted emacs: ");
        printProcess(tempProcess);
        free(tempProcess);
    } else {
        printf("Process 'emacs' not found.\n");
    }

    tempProcess = delete_pid(&head, 12235);
    if (tempProcess != NULL) {
        printf("Deleted PID 12235: ");
        printProcess(tempProcess);
        free(tempProcess);
    } else {
        printf("Process with PID 12235 not found.\n");
    }

    // Pop and print remaining processes
    printf("Remaining processes after deletions:\n");
    while ((tempProcess = pop(&head)) != NULL) {
        printProcess(tempProcess);
        free(tempProcess);
    }

    // Free any remaining memory, though the queue should be empty at this point
    freeQueue(&head);

    return 0;
}

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

proc* pop(queue** head) {
    if (*head == NULL) return NULL;

    queue* temp = *head;
    proc* removedProcess = malloc(sizeof(proc));
    *removedProcess = temp->process; // Copy the process data
    *head = temp->next;
    free(temp);

    return removedProcess;
}

proc* delete_name(queue** head, char* name) {
    queue* temp = *head, *prev = NULL;
    while (temp != NULL && strcmp(temp->process.name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return NULL;

    proc* deletedProcess = malloc(sizeof(proc));
    *deletedProcess = temp->process; // Copy the process data

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp);
    return deletedProcess;
}

proc* delete_pid(queue** head, int pid) {
    queue* temp = *head, *prev = NULL;
    while (temp != NULL && temp->process.pid != pid) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return NULL;

    proc* deletedProcess = malloc(sizeof(proc));
    *deletedProcess = temp->process; // Copy the process data

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp);
    return deletedProcess;
}

void printProcess(proc* process) {
    if (process != NULL) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", process->name, process->priority, process->pid, process->runtime);
    }
}

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
