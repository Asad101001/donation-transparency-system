#include "DoublyLinkedList.h"
#include <iostream>

using namespace std;
#include <cstring>

#ifndef NULL
#define NULL 0
#endif

DoublyLinkedList* createDoublyLinkedList() {
    DoublyLinkedList* dll = new DoublyLinkedList;
    dll->head = NULL;
    dll->tail = NULL;
    return dll;
}

void insertDonation(DoublyLinkedList* dll, Donation d) {
    Donation* newNode = new Donation;
    *newNode = d;
    newNode->next = NULL;
    newNode->prev = NULL;

    if(dll->head == NULL) {
        dll->head = newNode;
        dll->tail = newNode;
    } else {
        dll->tail->next = newNode;
        newNode->prev = dll->tail;
        dll->tail = newNode;
    }
}

Donation* searchDonation(DoublyLinkedList* dll, int donationID) {
    Donation* temp = dll->head;
    while(temp != NULL) {
        if(temp->donationID == donationID) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printAllDonations(DoublyLinkedList* dll) {
    Donation* temp = dll->head;
    while(temp != NULL) {
        cout << "Donation ID: " << temp->donationID 
                  << " | Donor ID: " << temp->donorID 
                  << " | Amount: " << temp->amount 
                  << " | Status: " << temp->currentStatus << "\n";
        temp = temp->next;
    }
}
