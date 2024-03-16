#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Structure Definitions
typedef struct proc {
    char name[256];
    int priority;
    int pid;
    int runtime;
} proc;

typedef struct queue {
    proc process;
    struct queue* next;
} queue;

// Function Prototypes
void push(queue** head, proc process);
proc* pop(queue** head);
proc* delete_name(queue** head, char* name);
proc* delete_pid(queue** head, int pid);
void printProcess(proc* process);
void freeQueue(queue** head);

// Main function
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

    // Read and process each line from the file
    while (fgets(line, sizeof(line), file)) {
        proc newProcess;
        if (sscanf(line, "%255[^,],%d,%d,%d", newProcess.name, &newProcess.priority, &newProcess.pid, &newProcess.runtime) == 4) {
            push(&head, newProcess);
        }
    }
    fclose(file);

    // Delete specified processes
    tempProcess = delete_name(&head, "emacs");
    if (tempProcess) {
        printf("Deleted emacs: ");
        printProcess(tempProcess);
        free(tempProcess);
    } else {
        printf("Process 'emacs' not found.\n");
    }

    tempProcess = delete_pid(&head, 12235);
    if (tempProcess) {
        printf("Deleted PID 12235: ");
        printProcess(tempProcess);
        free(tempProcess);
    } else {
        printf("Process with PID 12235 not found.\n");
    }

    // Pop and print the remaining processes
    printf("Remaining processes after deletions:\n");
    while ((tempProcess = pop(&head)) != NULL) {
        printProcess(tempProcess);
        free(tempProcess);
    }

    // Free any remaining queue memory
    freeQueue(&head);
    return 0;
}

// Function implementations
void push(queue** head, proc process) {
    queue* newNode = (queue*)malloc(sizeof(queue));
    if (!newNode) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    newNode->process = process;
    newNode->next = *head;
    *head = newNode;
}

proc* pop(queue** head) {
    if (!*head) return NULL;

    queue* temp = *head;
    proc* removedProcess = malloc(sizeof(proc));
    *removedProcess = temp->process;
    *head = temp->next;
    free(temp);
    return removedProcess;
}

proc* delete_name(queue** head, char* name) {
    queue* temp = *head, *prev = NULL;
    while (temp && strcmp(temp->process.name, name)) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return NULL;

    proc* deletedProcess = malloc(sizeof(proc));
    *deletedProcess = temp->process;
    
    if (!prev) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp);
    return deletedProcess;
}

proc* delete_pid(queue** head, int pid) {
    queue* temp = *head, *prev = NULL;
    while (temp && temp->process.pid != pid) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return NULL;

    proc* deletedProcess = malloc(sizeof(proc));
    *deletedProcess = temp->process;

    if (!prev) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp);
    return deletedProcess;
}

void printProcess(proc* process) {
    if (process) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", process->name, process->priority, process->pid, process->runtime);
    }
}

void freeQueue(queue** head) {
    while (*head) {
        queue* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}
