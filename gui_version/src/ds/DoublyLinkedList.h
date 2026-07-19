#pragma once
#include "../models/Models.h"

struct DoublyLinkedList {
    Donation* head;
    Donation* tail;
};

DoublyLinkedList* createDoublyLinkedList();
void insertDonation(DoublyLinkedList* dll, Donation d);
Donation* searchDonation(DoublyLinkedList* dll, int donationID);
void printAllDonations(DoublyLinkedList* dll);
