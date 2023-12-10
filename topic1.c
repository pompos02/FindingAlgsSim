#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#define MAX_PATH_LENGTH 100000 
#define MAX_STATE 100000  // Define this according to your state space size
bool visitedStates[MAX_STATE] = {false};
void writeSolutionToFile(const char *filename, const char *path, unsigned long totalCost) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }
    unsigned long cc=0;
    // Count the number of instructions
    int numInstructions = 0;
    char pathCopy[MAX_PATH_LENGTH];
    strcpy(pathCopy, path);
    char *token = strtok(pathCopy, " ");
    while (token != NULL) {
        
        numInstructions++; // Count each instruction
        char *instruction = token;
        token = strtok(NULL, " "); // Skip the value
        if (token == NULL) break; // Safety check
        unsigned long number = strtoul(token, NULL, 10);
         if (strcmp(instruction, "increase") == 0) {
            cc += 2; 
        } else if (strcmp(instruction, "decrease") == 0) {
            cc += 2; 
        } else if (strcmp(instruction, "double") == 0) {
            cc += number / 2 + 1; 
        } else if (strcmp(instruction, "half") == 0) {
            cc += number / 4 + 1; 
        } else if (strcmp(instruction, "square") == 0) {
            cc += (number * number - number) / 4 + 1;
        } else if (strcmp(instruction, "root") == 0) {
            unsigned long root = sqrt(number);
            cc += (number - root) / 4 + 1;
        }
        token = strtok(NULL, " "); // Next instruction
    }
     
    fprintf(file, "%d, %lu\n", numInstructions, cc); // N, C

    // Reset strtok for actual parsing
    strcpy(pathCopy, path);
    token = strtok(pathCopy, " ");

    while (token != NULL) {
        char *instruction = token;
        token = strtok(NULL, " ");
        if (token == NULL) break; // Safety check

        unsigned long number = strtoul(token, NULL, 10);
        unsigned long cost = 0;

        // Calculate cost for each operation
        if (strcmp(instruction, "increase") == 0) {
            cost = 2; 
        } else if (strcmp(instruction, "decrease") == 0) {
            cost = 2; 
        } else if (strcmp(instruction, "double") == 0) {
            cost = number / 2 + 1; 
        } else if (strcmp(instruction, "half") == 0) {
            cost = number / 4 + 1; 
        } else if (strcmp(instruction, "square") == 0) {
            cost = (number * number - number) / 4 + 1;
        } else if (strcmp(instruction, "root") == 0) {
            unsigned long root = sqrt(number);
            cost = (number - root) / 4 + 1;
        }

        fprintf(file, "%s %lu %lu\n", instruction, number, cost); // instruction number cost

        token = strtok(NULL, " "); // Next instruction
    }

    fclose(file);
}


// Define a struct to represent each state
typedef struct {
    unsigned long value;
    unsigned long cost;
    char path[MAX_PATH_LENGTH];
} State;

// Node structure for BFS and DFS
typedef struct Node {
    State state;
    struct Node *next;
} Node;

// Function to create a new node
Node* createNode(State state) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->state = state;
    newNode->next = NULL;
    return newNode;
}

// Stack push function for DFS
void push(Node **head, State state) {
    Node *newNode = createNode(state);
    newNode->next = *head;
    *head = newNode;
}

// Queue enqueue function for BFS
void enqueue(Node **head, Node **tail, State state) {
    Node *newNode = createNode(state);
    if (*tail) {
        (*tail)->next = newNode;
    }
    *tail = newNode;
    if (!*head) {
        *head = *tail;
    }
}

// Stack pop function for DFS
State pop(Node **head) {
    if (*head == NULL) {
        State emptyState = {0, 0, ""};
        return emptyState;
    }
    Node *temp = *head;
    *head = (*head)->next;
    State poppedState = temp->state;
    free(temp);
    return poppedState;
}

// Queue dequeue function for BFS
State dequeue(Node **head, Node **tail) {
    return pop(head);
}

// Check if stack/queue is empty
bool isEmpty(Node *head) {
    return head == NULL;
}

bool isCycle(char *path, unsigned long value) {
    char tempPath[MAX_PATH_LENGTH];
    strcpy(tempPath, path);
    char *token = strtok(tempPath, " ");
    while (token != NULL) {
        // Skip the operation name
        token = strtok(NULL, " "); 
        if (token == NULL) {
            break;
        }

        // Convert the next token (which should be a value) to a number
        unsigned long pathValue = strtoul(token, NULL, 10);
        if (pathValue == value) {
            return true;
        }

        // Move to the next operation
        token = strtok(NULL, " ");
    }
    return false;
}

unsigned long heuristic(unsigned long currentValue, unsigned long targetValue) {
    if (currentValue == targetValue) {
        return 0;
    }

    unsigned long estimatedCost = 0;
    while (currentValue != targetValue) {
        if (currentValue < targetValue) {
            if (currentValue * 2 <= targetValue && currentValue <= 500000000) {
                // Use double if it doesn't overshoot and within safe range
                currentValue *= 2;
                estimatedCost += currentValue / 2 + 1;
            } else {
                // Otherwise, use increase
                currentValue++;
                estimatedCost += 2;
            }
        } else {
            if (currentValue / 2 >= targetValue && currentValue > 1) {
                // Use half if it doesn't undershoot and is valid
                currentValue /= 2;
                estimatedCost += currentValue / 4 + 1;
            } else {
                // Otherwise, use decrease
                currentValue--;
                estimatedCost += 2;
            }
        }
    }

    return estimatedCost;
}



Node *extractMin(Node **head, unsigned long target) {
    if (*head == NULL) {
        return NULL;
    }

    Node *minNode = *head;
    Node *prev = NULL;
    Node *current = *head;
    Node *prevToMin = NULL;

    while (current != NULL) {
        unsigned long currentPriority = current->state.cost + heuristic(current->state.value, target);
        unsigned long minPriority = minNode->state.cost + heuristic(current->state.value, target);
        if (currentPriority < minPriority) {
            minNode = current;
            prevToMin = prev;
        }
        prev = current;
        current = current->next;
    }

    // Remove minNode from the list
    if (minNode == *head) {
        *head = minNode->next;
    } else {
        prevToMin->next = minNode->next;
    }

    return minNode;
}

void sortedInsertForAStar(Node **head, State state, unsigned long target) {
    Node *newNode = createNode(state);
    unsigned long newPriority = state.cost + heuristic(state.value, target); // A* priority

    Node *current = *head;
    Node *previous = NULL;
    while (current != NULL && (current->state.cost + heuristic(current->state.value, target)) < newPriority) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        newNode->next = *head;
        *head = newNode;
    } else {
        newNode->next = previous->next;
        previous->next = newNode;
    }
}
// Function to insert a state into the priority queue in sorted order
void sortedInsert(Node **head, State state, unsigned long target) {
    Node *newNode = createNode(state);
    unsigned long newHeuristic = heuristic(state.value, target);

    // Find the correct position to insert the new node
    Node *current = *head;
    Node *previous = NULL;
    while (current != NULL && heuristic(current->state.value, target) < newHeuristic) {
        previous = current;
        current = current->next;
    }

    // Insert the new node
    if (previous == NULL) {
        newNode->next = *head;
        *head = newNode;
    } else {
        newNode->next = previous->next;
        previous->next = newNode;
    }
}
void resetVisitedStates() {
    for (int i = 0; i < MAX_STATE; i++) {
        visitedStates[i] = false;
    }
}
void applyOperationsAndAddStates2(Node **head, State currentState, unsigned long target, bool isAStar) {
    State newState;

    // Example for one operation (increase):
    if (currentState.value < 1000000000) {
        newState = currentState;
        newState.value += 1;
        newState.cost += 2;
        // Append operation to path
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " increase %lu", newState.value);
        strcat(newState.path, operation);
        // Insert new state in sorted order
        if (newState.value < MAX_STATE && !visitedStates[newState.value]) {
            visitedStates[newState.value] = true;  // Mark as visited
            if (isAStar) {
                sortedInsert(head, newState, target);
            } else {
                sortedInsert(head, newState, target);
            }
        }
    }

    // Decrease
    if (currentState.value > 0) {
        newState = currentState;
        newState.value -= 1;
        newState.cost += 2;
        snprintf(newState.path, sizeof(newState.path), "%s decrease %lu", currentState.path, newState.value);
        if (isAStar) {
            sortedInsertForAStar(head, newState, target);
        } else {
            sortedInsert(head, newState, target);
        }
    }

    // Double
    if (currentState.value > 0 && currentState.value <= 500000000) { // Adjusted to prevent overflow
        newState = currentState;
        newState.value *= 2;
        newState.cost += currentState.value / 2 + 1;
        snprintf(newState.path, sizeof(newState.path), "%s double %lu", currentState.path, newState.value);
        if (isAStar) {
            sortedInsertForAStar(head, newState, target);
        } else {
            sortedInsert(head, newState, target);
        }
    }

    // Half
    if (currentState.value > 0) {
        newState = currentState;
        newState.value /= 2;
        newState.cost += currentState.value / 4 + 1;
        snprintf(newState.path, sizeof(newState.path), "%s half %lu", currentState.path, newState.value);
        if (isAStar) {
            sortedInsertForAStar(head, newState, target);
        } else {
            sortedInsert(head, newState, target);
        }
    }

    // Square
    if (currentState.value <= 31622) { // sqrt(10^9) to prevent overflow
        newState = currentState;
        newState.value *= newState.value;
        newState.cost += (newState.value - currentState.value) / 4 + 1;
        snprintf(newState.path, sizeof(newState.path), "%s square %lu", currentState.path, newState.value);
        if (isAStar) {
            sortedInsertForAStar(head, newState, target);
        } else {
            sortedInsert(head, newState, target);
        }
    }

    // Root
    if (currentState.value > 1 && sqrt(currentState.value) == floor(sqrt(currentState.value))) {
        newState = currentState;
        newState.value = sqrt(newState.value);
        newState.cost += (currentState.value - newState.value) / 4 + 1;
        snprintf(newState.path, sizeof(newState.path), "%s root %lu", currentState.path, newState.value);
        if (isAStar) {
            sortedInsertForAStar(head, newState, target);
        } else {
            sortedInsert(head, newState, target);
        }
    }
    // Similar implementation for other operations (decrease, double, half, square, root)
}
// Function to apply operations and add new states to the stack/queue
void applyOperationsAndAddStates(Node **head, Node **tail, State currentState, bool isBFS) {
    State newState;

    // Increase
    if (currentState.value < 1000000000) {
        newState = currentState;
        newState.value += 1;
        newState.cost += 2;
         // Convert the new value to a string and append it along with the operation name
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " increase %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){
            if (isBFS) enqueue(head, tail, newState); else push(head, newState);
         }
        
    }

    // Decrease
    if (currentState.value > 0) {
        newState = currentState;
        newState.value -= 1;
        newState.cost += 2;
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " decrease %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){

         }
        if (isBFS) enqueue(head, tail, newState); else push(head, newState);
    }

    // Double
    if (currentState.value > 0 && currentState.value <= 1000000000) {
        newState = currentState;
        newState.value *= 2;
        newState.cost += currentState.value / 2 + 1;
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " double %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){
            if (isBFS) enqueue(head, tail, newState); else push(head, newState);
         }
        
    }

    // Half
    if (currentState.value > 0) {
        newState = currentState;
        newState.value /= 2;
        newState.cost += currentState.value / 4 + 1;
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " half %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){
            if (isBFS) enqueue(head, tail, newState); else push(head, newState);
         }
        
    }

    // Square
    if (currentState.value * currentState.value <= 1000000000) {
        newState = currentState;
        newState.value *= newState.value;
        newState.cost += (newState.value - currentState.value) / 4 + 1;
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " square %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){
            if (isBFS) enqueue(head, tail, newState); else push(head, newState);
         }
        
    }

    // Root
    if (currentState.value > 1 && sqrt(currentState.value) == floor(sqrt(currentState.value))) {
        newState = currentState;
        newState.value = sqrt(newState.value);
        newState.cost += (currentState.value - newState.value) / 4 + 1;
        char operation[50]; // Size should be enough to hold the operation and number
        snprintf(operation, sizeof(operation), " root %lu", newState.value);
        strcat(newState.path, operation);
         if (!isCycle(currentState.path, newState.value)){
            if (isBFS) enqueue(head, tail, newState); else push(head, newState);
         }
        
    }
}

// Breadth-First Search implementation
void breadthFirstSearch(unsigned long start, unsigned long target, const char *filename) {
    Node *queueHead = NULL, *queueTail = NULL;
    State initialState = {start, 0, ""};
    enqueue(&queueHead, &queueTail, initialState);

    while (!isEmpty(queueHead)) {
        State currentState = dequeue(&queueHead, &queueTail);
        if (currentState.value == target) {
            writeSolutionToFile(filename, currentState.path, currentState.cost);
            break;
        }
        applyOperationsAndAddStates(&queueHead, &queueTail, currentState, true);
    }
}


// Depth-First Search implementation
void depthFirstSearch(unsigned long start, unsigned long target, const char *filename) {
    Node *stackHead = NULL;
    State initialState = {start, 0, ""};
    
    push(&stackHead, initialState);

    while (!isEmpty(stackHead)) {
        State currentState = pop(&stackHead);
        if (currentState.value == target) {
            writeSolutionToFile(filename, currentState.path, currentState.cost);
            break;
        }
        applyOperationsAndAddStates(&stackHead, NULL, currentState, false);
    }
}

void bestFirstSearch(unsigned long start, unsigned long target, const char *filename) {
    Node *priorityQueue = NULL;
    State initialState = {start, 0, ""};
    sortedInsert(&priorityQueue, initialState, target);

    while (!isEmpty(priorityQueue)) {
        State currentState = pop(&priorityQueue);
        if (currentState.value == target) {
            writeSolutionToFile(filename, currentState.path, currentState.cost);
            break;
        }
        applyOperationsAndAddStates2(&priorityQueue,currentState, target,false);
    }
}

// A* Search implementation
void aStarSearch(unsigned long start, unsigned long target, const char *filename) {
    resetVisitedStates();
    Node *priorityQueue = NULL;
    State initialState = {start, 0, ""};
    applyOperationsAndAddStates2(&priorityQueue, initialState, target, true); // Add initial state

    while (!isEmpty(priorityQueue)) {
        Node *node = extractMin(&priorityQueue, target); // Extract state with the lowest cost + heuristic
        State currentState = node->state;
        free(node); // Free the extracted node

        if (currentState.value == target) {
            writeSolutionToFile(filename, currentState.path, currentState.cost);
            break;
        }

        applyOperationsAndAddStates2(&priorityQueue, currentState, target, true);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s [breadth|depth] start_value target_value output_file\n", argv[0]);
        return 1;
    }

    unsigned long start = strtoul(argv[2], NULL, 10);
    unsigned long target = strtoul(argv[3], NULL, 10);
    const char *method = argv[1];
    const char *filename = argv[4];
    clock_t begin = clock();
    if (strcmp(method, "breadth") == 0) {
        breadthFirstSearch(start, target, filename);
    } else if (strcmp(method, "depth") == 0) {
        depthFirstSearch(start, target, filename);
    } else if (strcmp(method, "astar") == 0) {
        aStarSearch(start, target, filename);
    } else if (strcmp(method, "best") == 0) {
        bestFirstSearch(start, target, filename);
    } else {
        printf("Invalid search method. Choose 'breadth' or 'depth'.\n");
        return 1;
    }
    clock_t end = clock();
    double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Time taken: %f seconds\n", timeSpent);
    return 0;
}
