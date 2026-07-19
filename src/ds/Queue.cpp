#include "Queue.h"

#ifndef NULL
#define NULL 0
#endif

Queue* createQueue() {
    Queue* q = new Queue;
    q->front = NULL;
    q->rear = NULL;
    return q;
}

void enqueue(Queue* q, Allocation a) {
    AllocationNode* temp = new AllocationNode;
    temp->data = a;
    temp->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;
}

Allocation dequeue(Queue* q) {
    if (q->front == NULL) {
        Allocation emptyAlloc;
        emptyAlloc.donationID = -1;
        return emptyAlloc;
    }

    AllocationNode* temp = q->front;
    Allocation data = temp->data;

    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    delete temp;
    return data;
}

bool isQueueEmpty(Queue* q) {
    return q->front == NULL;
}
