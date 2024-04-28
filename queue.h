#ifndef QUEUE_H
#define QUEUE_H


// Node structure for the linked list
typedef struct Node {
    void* data;
    struct Node* next;
} Node;

// Queue structure
typedef struct Queue {
    Node* front;
    Node* rear;
} Queue, Stack;

Queue* createQueue();

Stack* createStack();

int isEmpty(Queue* queue);

void enqueue(Queue* queue, void* data);

void* dequeue(Queue* queue);

void* front(Queue* queue);

void* back(Queue* queue);

void* peek(Queue* queue, int index);

void* peekNext(Queue* queue);

void* pop(Stack* queue);

void push(Stack* queue, void* data);

void freeQueue(Queue* queue);

void freeStack(Stack* stack);

#endif /* QUEUE_H */