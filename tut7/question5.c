#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

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

// Function prototypes
void push(queue** head, proc process);
proc* pop(queue** head);
proc* delete_name(queue** head, char* name, bool freeNode);
void executeProcess(proc process);
void freeQueue(queue** head);
void readFileAndFillQueue(const char* filename, queue** head);
void printProcessDetails(const proc* process);

int main(void) {
    queue* head = NULL;
    readFileAndFillQueue("processes_q5.txt", &head);

    // Execute processes with priority 0 first
    queue* current = head;
    while (current != NULL) {
        if (current->process.priority == 0) {
            proc* process = delete_name(&head, current->process.name, false);
            if (process != NULL) {
                executeProcess(*process);
                free(process);
            }
        }
        current = current->next;
    }

    // Execute the remaining processes
    proc* process;
    while ((process = pop(&head)) != NULL) {
        executeProcess(*process);
        free(process);
    }

    // Clean up
    freeQueue(&head);
    return 0;
}

void push(queue** head, proc process) {
    queue* newNode = (queue*)malloc(sizeof(queue));
    if (newNode == NULL) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->process = process;
    newNode->next = *head;
    *head = newNode;
}

proc* pop(queue** head) {
    if (*head == NULL) {
        return NULL;
    }
    queue* nextNode = (*head)->next;
    proc* retProcess = malloc(sizeof(proc));
    *retProcess = (*head)->process;
    free(*head);
    *head = nextNode;
    return retProcess;
}

proc* delete_name(queue** head, char* name, bool freeNode) {
    queue* temp = *head, *prev = NULL;
    while (temp != NULL) {
        if (strcmp(temp->process.name, name) == 0) {
            if (prev == NULL) {
                *head = temp->next;
            } else {
                prev->next = temp->next;
            }
            proc* retProcess = malloc(sizeof(proc));
            *retProcess = temp->process;
            if (freeNode) {
                free(temp);
            }
            return retProcess;
        }
        prev = temp;
        temp = temp->next;
    }
    return NULL;
}

void executeProcess(proc process) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execl("./process", "./process", NULL);
        // If execl returns, it must have failed
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        process.pid = pid;
        sleep(process.runtime);
        kill(pid, SIGINT);

        int status;
        waitpid(pid, &status, 0); // Wait for the process to exit

        printProcessDetails(&process);
    }
}

void freeQueue(queue** head) {
    while (*head != NULL) {
        queue* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

void readFileAndFillQueue(const char* filename, queue** head) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        proc newProcess;
        if (sscanf(line, "%[^,],%d,%d", newProcess.name, &newProcess.priority, &newProcess.runtime) == 3) {
            newProcess.pid = 0; // Initialize pid to 0
            push(head, newProcess);
        }
    }

    fclose(file);
}

void printProcessDetails(const proc* process) {
    printf("Process: %s, Priority: %d, PID: %d, Runtime: %d\n",
           process->name, process->priority, process->pid, process->runtime);
}
