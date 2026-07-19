#include "Stack.h"
#include <iostream>

using namespace std;

#ifndef NULL
#define NULL 0
#endif

void pushStatus(Donation* d, DonationStatus newStatus) {
    StatusNode* newNode = new StatusNode;
    newNode->status = newStatus;
    newNode->next = d->historyTop;
    d->historyTop = newNode;
    d->currentStatus = newStatus; // update current status
}

DonationStatus popStatus(Donation* d) {
    if (d->historyTop == NULL) {
        return STATUS_RECEIVED; // default
    }
    StatusNode* temp = d->historyTop;
    DonationStatus popped = temp->status;
    d->historyTop = d->historyTop->next;
    delete temp;
    
    if (d->historyTop != NULL) {
        d->currentStatus = d->historyTop->status;
    }
    
    return popped;
}

void printStatusHistory(Donation* d) {
    StatusNode* temp = d->historyTop;
    cout << "  Status History (Latest First):\n";
    while (temp != NULL) {
        cout << "    - ";
        switch (temp->status) {
            case STATUS_RECEIVED: cout << "Received"; break;
            case STATUS_ALLOCATED: cout << "Allocated"; break;
            case STATUS_IN_PROGRESS: cout << "In Progress"; break;
            case STATUS_COMPLETED: cout << "Completed"; break;
        }
        cout << "\n";
        temp = temp->next;
    }
}
