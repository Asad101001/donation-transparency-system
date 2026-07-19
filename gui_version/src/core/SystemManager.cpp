#include "SystemManager.h"
#include "Utils.h"
#include <iostream>

using namespace std;


SystemManager* createSystemManager() {
    SystemManager* sys = new SystemManager;
    sys->donorDB = createHashTable(100);
    sys->donationRecords = createDoublyLinkedList();
    sys->projects = createAVLTree();
    sys->allocationQueue = createQueue();
    sys->expenses = createSinglyLinkedList();
    sys->maxDonations = createMaxHeap(1000);
    sys->minDonations = createMinHeap(1000);
    
    sys->nextDonorID = 1;
    sys->nextDonationID = 100;
    sys->nextProjectID = 10;
    sys->nextExpenseID = 500;
    
    return sys;
}

void registerDonor(SystemManager* sys, const char* name, const char* cnic) {
    Donor d;
    d.donorID = sys->nextDonorID++;
    my_strcpy(d.name, name);
    my_strcpy(d.cnic, cnic);
    
    insertDonor(sys->donorDB, d);
    cout << "[SUCCESS] Donor registered! ID: " << d.donorID << "\n";
}

void makeDonation(SystemManager* sys, int donorID, double amount, const char* date) {
    if (searchDonor(sys->donorDB, donorID) == NULL) {
        cout << "[ERROR] Donor ID not found.\n";
        return;
    }
    
    Donation d;
    d.donationID = sys->nextDonationID++;
    d.donorID = donorID;
    d.amount = amount;
    d.remainingAmount = amount;
    my_strcpy(d.date, date);
    d.historyTop = NULL;
    d.next = NULL;
    d.prev = NULL;
    
    pushStatus(&d, STATUS_RECEIVED);
    insertDonation(sys->donationRecords, d);
    
    // Add to Allocation Queue
    Allocation a;
    a.donationID = d.donationID;
    a.projectID = -1; // Not yet allocated
    a.allocatedAmount = amount;
    enqueue(sys->allocationQueue, a);
    
    // Add to Heaps
    insertMaxHeap(sys->maxDonations, amount);
    insertMinHeap(sys->minDonations, amount);
    
    cout << "[SUCCESS] Donation received! Donation ID: " << d.donationID << "\n";
}

void viewDonors(SystemManager* sys) {
    cout << "--- Registered Donors ---\n";
    printAllDonors(sys->donorDB);
}

void createProject(SystemManager* sys, const char* name, const char* desc) {
    Project p;
    p.projectID = sys->nextProjectID++;
    my_strcpy(p.name, name);
    my_strcpy(p.description, desc);
    p.left = NULL;
    p.right = NULL;
    p.height = 1;
    
    sys->projects->root = insertProject(sys->projects->root, p);
    cout << "[SUCCESS] Project Created! ID: " << p.projectID << "\n";
}

void allocateDonation(SystemManager* sys, int projectID) {
    if (searchProject(sys->projects->root, projectID) == NULL) {
        cout << "[ERROR] Project ID not found.\n";
        return;
    }
    
    if (isQueueEmpty(sys->allocationQueue)) {
        cout << "[INFO] No pending donations to allocate.\n";
        return;
    }
    
    Allocation alloc = dequeue(sys->allocationQueue);
    alloc.projectID = projectID;
    
    Donation* d = searchDonation(sys->donationRecords, alloc.donationID);
    if (d != NULL) {
        pushStatus(d, STATUS_ALLOCATED);
        cout << "[SUCCESS] Allocated Donation " << d->donationID << " to Project " << projectID << "\n";
    }
}

void addExpense(SystemManager* sys, int projectID, double amount, const char* desc, const char* date) {
    if (searchProject(sys->projects->root, projectID) == NULL) {
        cout << "[ERROR] Project ID not found.\n";
        return;
    }
    
    Expense e;
    e.expenseID = sys->nextExpenseID++;
    e.projectID = projectID;
    e.amount = amount;
    my_strcpy(e.description, desc);
    my_strcpy(e.date, date);
    
    insertExpense(sys->expenses, e);
    cout << "[SUCCESS] Expense added to Project " << projectID << "\n";
    
    // Find an allocated donation and subtract remaining
    // For simplicity, we just find any donation allocated to this project
    // Real logic would be more complex, but we demonstrate finding and updating:
    Donation* cur = sys->donationRecords->head;
    while(cur != NULL) {
        if (cur->currentStatus == STATUS_ALLOCATED || cur->currentStatus == STATUS_IN_PROGRESS) {
            // we assume it is related if it has remaining amount > 0
            if (cur->remainingAmount >= amount) {
                cur->remainingAmount -= amount;
                pushStatus(cur, STATUS_IN_PROGRESS);
                if (cur->remainingAmount == 0) {
                    pushStatus(cur, STATUS_COMPLETED);
                }
                break;
            }
        }
        cur = cur->next;
    }
}

void searchDonation(SystemManager* sys, int donationID) {
    Donation* d = searchDonation(sys->donationRecords, donationID);
    if (d == NULL) {
        cout << "[ERROR] Donation not found.\n";
        return;
    }
    
    Donor* donor = searchDonor(sys->donorDB, d->donorID);
    
    cout << "\n--- Donation Details ---\n";
    cout << "Donation ID: " << d->donationID << "\n";
    cout << "Donor: " << (donor ? donor->name : "Unknown") << "\n";
    cout << "Total Amount: Rs. " << d->amount << "\n";
    cout << "Remaining Amount: Rs. " << d->remainingAmount << "\n";
    
    cout << "Current Status: ";
    switch(d->currentStatus) {
        case STATUS_RECEIVED: cout << "Received"; break;
        case STATUS_ALLOCATED: cout << "Allocated"; break;
        case STATUS_IN_PROGRESS: cout << "In Progress"; break;
        case STATUS_COMPLETED: cout << "Completed"; break;
    }
    cout << "\n";
    
    printStatusHistory(d);
}

void generateTransparencyReport(SystemManager* sys, int donationID) {
    Donation* d = searchDonation(sys->donationRecords, donationID);
    if (d == NULL) {
        cout << "[ERROR] Donation not found.\n";
        return;
    }
    
    cout << "\n=== TRANSPARENCY REPORT ===\n";
    cout << "Donation\n";
    cout << "Rs. " << d->amount << "\n";
    cout << "   |\n   v\n";
    
    if (d->currentStatus >= STATUS_ALLOCATED) {
        cout << "Allocated\n";
        cout << "   |\n   v\n";
        cout << "Expenses (Global or specific to project):\n";
        // To be precise we should link expense to donation directly,
        // but for now we list all expenses of allocated projects or global.
        cout << "   |\n   v\n";
        cout << "Remaining\n";
        cout << "Rs. " << d->remainingAmount << "\n";
    } else {
        cout << "Pending Allocation\n";
    }
    cout << "===========================\n";
}

void viewDonationHistory(SystemManager* sys) {
    cout << "\n--- All Donations ---\n";
    printAllDonations(sys->donationRecords);
}

void viewStatistics(SystemManager* sys) {
    cout << "\n--- Statistics ---\n";
    cout << "Largest Donation: Rs. " << getMax(sys->maxDonations) << "\n";
    cout << "Smallest Donation: Rs. " << getMin(sys->minDonations) << "\n";
    
    int count = 0;
    double total = 0;
    Donation* cur = sys->donationRecords->head;
    while(cur != NULL) {
        count++;
        total += cur->amount;
        cur = cur->next;
    }
    cout << "Total Donations Received: " << count << "\n";
    if (count > 0)
        cout << "Average Donation: Rs. " << (total / count) << "\n";
}
