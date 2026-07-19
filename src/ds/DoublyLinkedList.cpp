#include "DoublyLinkedList.h"
#include <iostream>

using namespace std;
#include <cstring>


DoublyLinkedList* createDoublyLinkedList() {
    DoublyLinkedList* dll = new DoublyLinkedList;
    dll->head = NULL;
    dll->tail = NULL;
    return dll;
}

void insertDonation(DoublyLinkedList* dll, Donation d) {
    Donation* temp = new Donation;
    *temp = d;
    temp->next = NULL;
    temp->prev = NULL;

    if(dll->head == NULL) {
        dll->head = temp;
        dll->tail = temp;
    } else {
        dll->tail->next = temp;
        temp->prev = dll->tail;
        dll->tail = temp;
    }
}

Donation* searchDonation(DoublyLinkedList* dll, int donationID) {
    Donation* cur = dll->head;
    while(cur != NULL) {
        if(cur->donationID == donationID) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

void printAllDonations(DoublyLinkedList* dll) {
    Donation* cur = dll->head;
    while(cur != NULL) {
        cout << "Donation ID: " << cur->donationID 
                  << " | Donor ID: " << cur->donorID 
                  << " | Amount: " << cur->amount 
                  << " | Status: " << cur->currentStatus << "\n";
        cur = cur->next;
    }
}
