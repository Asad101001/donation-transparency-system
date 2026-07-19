#include "Stack.h"
#include <iostream>

using namespace std;

void pushStatus(Donation* d, DonationStatus newStatus) {
    StatusNode* temp = new StatusNode;
    temp->status = newStatus;
    temp->next = d->historyTop;
    d->historyTop = temp;
    d->currentStatus = newStatus; // update current status
}

DonationStatus popStatus(Donation* d) {
    if (d->historyTop == NULL) {
        return STATUS_RECEIVED; // default
    }
    StatusNode* cur = d->historyTop;
    DonationStatus popped = cur->status;
    d->historyTop = d->historyTop->next;
    delete cur;
    
    if (d->historyTop != NULL) {
        d->currentStatus = d->historyTop->status;
    }
    
    return popped;
}

void printStatusHistory(Donation* d) {
    StatusNode* cur = d->historyTop;
    cout << "  Status History (Latest First):\n";
    while (cur != NULL) {
        cout << "    - ";
        switch (cur->status) {
            case STATUS_RECEIVED: cout << "Received"; break;
            case STATUS_ALLOCATED: cout << "Allocated"; break;
            case STATUS_IN_PROGRESS: cout << "In Progress"; break;
            case STATUS_COMPLETED: cout << "Completed"; break;
        }
        cout << "\n";
        cur = cur->next;
    }
}
