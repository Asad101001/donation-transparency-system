#ifndef QUEUE_H
#define QUEUE_H

#include "../models/Models.h"

struct Queue {
    AllocationNode* front;
    AllocationNode* rear;
};

Queue* createQueue();
void enqueue(Queue* q, Allocation a);
Allocation dequeue(Queue* q);
bool isQueueEmpty(Queue* q);

#endif // QUEUE_H
