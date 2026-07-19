#pragma once

#ifndef NULL
#define NULL 0
#endif
#define MAX_STR 100

// Using C-style enums or string for status. Let's use enum for internal status tracking, but string is also fine.
enum DonationStatus {
    STATUS_RECEIVED,
    STATUS_ALLOCATED,
    STATUS_IN_PROGRESS,
    STATUS_COMPLETED
};

struct Donor {
    int donorID;
    char name[MAX_STR];
    char cnic[MAX_STR];
};

struct Expense {
    int expenseID;
    int projectID; // To know which project this belongs to
    double amount;
    char description[MAX_STR];
    char date[MAX_STR];
    Expense* next; // For Singly Linked List
};

struct StatusNode {
    DonationStatus status;
    StatusNode* next;
};

struct Allocation {
    int donationID;
    int projectID;
    double allocatedAmount;
};

struct Donation {
    int donationID;
    int donorID;
    double amount;
    double remainingAmount;
    char date[MAX_STR];
    DonationStatus currentStatus;
    
    StatusNode* historyTop; // Stack for status history
    
    Donation* prev;
    Donation* next;
};

struct Project {
    int projectID;
    char name[MAX_STR];
    char description[MAX_STR];
    
    // For AVL Tree
    Project* left;
    Project* right;
    int height;
};

// Queue node for Allocation
struct AllocationNode {
    Allocation data;
    AllocationNode* next;
};
