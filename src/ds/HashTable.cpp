#include "HashTable.h"
#include <iostream>

using namespace std;


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
    HashNode* temp = new HashNode;
    temp->donor = d;
    temp->next = ht->table[index];
    ht->table[index] = temp;
}

Donor* searchDonor(HashTable* ht, int donorID) {
    int index = hashFunction(ht, donorID);
    HashNode* cur = ht->table[index];
    while(cur != NULL) {
        if(cur->donor.donorID == donorID) {
            return &(cur->donor);
        }
        cur = cur->next;
    }
    return NULL;
}

void printAllDonors(HashTable* ht) {
    for(int i = 0; i < ht->size; i++) {
        HashNode* cur = ht->table[i];
        while(cur != NULL) {
            cout << "Donor ID: " << cur->donor.donorID 
                      << " | Name: " << cur->donor.name 
                      << " | CNIC: " << cur->donor.cnic << "\n";
            cur = cur->next;
        }
    }
}
