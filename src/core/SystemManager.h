#pragma once
#include "../ds/HashTable.h"
#include "../ds/DoublyLinkedList.h"
#include "../ds/AVLTree.h"
#include "../ds/Queue.h"
#include "../ds/SinglyLinkedList.h"
#include "../ds/Stack.h"
#include "../ds/Heap.h"

struct SystemManager {
    HashTable* donorDB;
    DoublyLinkedList* donationRecords;
    AVLTree* projects;
    Queue* allocationQueue;
    SinglyLinkedList* expenses;
    MaxHeap* maxDonations;
    MinHeap* minDonations;
    
    int nextDonorID;
    int nextDonationID;
    int nextProjectID;
    int nextExpenseID;
};

SystemManager* createSystemManager();

// Core Operations
void registerDonor(SystemManager* sys, const char* name, const char* cnic);
void makeDonation(SystemManager* sys, int donorID, double amount, const char* date);
void viewDonors(SystemManager* sys);

void createProject(SystemManager* sys, const char* name, const char* desc);
void allocateDonation(SystemManager* sys, int projectID);
void addExpense(SystemManager* sys, int projectID, double amount, const char* desc, const char* date);

void searchDonation(SystemManager* sys, int donationID);
void generateTransparencyReport(SystemManager* sys, int donationID);
void viewDonationHistory(SystemManager* sys);
void viewStatistics(SystemManager* sys);
