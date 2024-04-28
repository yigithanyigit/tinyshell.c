#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "mem.h"
#include "tokens.h"

Queue* createQueue() {
    Queue* queue = shell_calloc(1, sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

Stack *createStack()
{
    return createQueue();
}

int isEmpty(Queue* queue) {
    return (queue->front == NULL);
}

void enqueue(Queue* queue, void* data) {
    Node* newNode = shell_calloc(1, sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

void* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return NULL;
    }

    Node* temp = queue->front;
    void* data = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    shell_free(temp);
    return data;
}

void* pop(Stack* stack)
{
    return dequeue(stack);
}

void push(Stack* stack, void* data)
{
    enqueue(stack, data);
}

void* front(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return NULL;
    }

    return queue->front->data;
}


void freeQueue(Queue* queue) {
    Node* temp = queue->front;
    while (temp != NULL) {
        Node* next = temp->next;
        shell_free(temp);
        temp = next;
    }
    shell_free(queue);
}

void freeStack(Stack* stack)
{
    freeQueue(stack);
}

void* peek(Queue* queue, int index) {
    Node* temp = queue->front;
    for (int i = 0; i < index; i++) {
        if (temp == NULL) {
            return NULL;
        }
        temp = temp->next;
    }

    return temp->data;
}

void* peekNext(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return NULL;
    }

    return queue->front->next->data;
}