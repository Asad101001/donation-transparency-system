#pragma once
#include "../models/Models.h"

struct HashNode {
    Donor donor;
    HashNode* next;
};

struct HashTable {
    int size;
    HashNode** table;
};

HashTable* createHashTable(int size);
int hashFunction(HashTable* ht, int donorID);
void insertDonor(HashTable* ht, Donor d);
Donor* searchDonor(HashTable* ht, int donorID);
void printAllDonors(HashTable* ht);
