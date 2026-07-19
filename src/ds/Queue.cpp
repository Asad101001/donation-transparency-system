#include "Queue.h"

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

    AllocationNode* cur = q->front;
    Allocation data = cur->data;

    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    delete cur;
    return data;
}

bool isQueueEmpty(Queue* q) {
    return q->front == NULL;
}
