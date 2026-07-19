#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include "../models/Models.h"

struct DoublyLinkedList {
    Donation* head;
    Donation* tail;
};

DoublyLinkedList* createDoublyLinkedList();
void insertDonation(DoublyLinkedList* dll, Donation d);
Donation* searchDonation(DoublyLinkedList* dll, int donationID);
void printAllDonations(DoublyLinkedList* dll);

#endif // DOUBLYLINKEDLIST_H
