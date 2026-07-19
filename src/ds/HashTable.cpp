#include "HashTable.h"
#include <iostream>

using namespace std;

#ifndef NULL
#define NULL 0
#endif

HashTable* createHashTable(int size) {
    HashTable* ht = new HashTable;
    ht->size = size;
    ht->table = new HashNode*[size];
    for(int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

int hashFunction(HashTable* ht, int donorID) {
    return donorID % ht->size;
}

void insertDonor(HashTable* ht, Donor d) {
    int index = hashFunction(ht, d.donorID);
    HashNode* newNode = new HashNode;
    newNode->donor = d;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

Donor* searchDonor(HashTable* ht, int donorID) {
    int index = hashFunction(ht, donorID);
    HashNode* temp = ht->table[index];
    while(temp != NULL) {
        if(temp->donor.donorID == donorID) {
            return &(temp->donor);
        }
        temp = temp->next;
    }
    return NULL;
}

void printAllDonors(HashTable* ht) {
    for(int i = 0; i < ht->size; i++) {
        HashNode* temp = ht->table[i];
        while(temp != NULL) {
            cout << "Donor ID: " << temp->donor.donorID 
                      << " | Name: " << temp->donor.name 
                      << " | CNIC: " << temp->donor.cnic << "\n";
            temp = temp->next;
        }
    }
}
