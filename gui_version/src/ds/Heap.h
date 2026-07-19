#pragma once
struct MaxHeap {
    double* array;
    int capacity;
    int size;
};

struct MinHeap {
    double* array;
    int capacity;
    int size;
};

MaxHeap* createMaxHeap(int capacity);
void insertMaxHeap(MaxHeap* h, double val);
double getMax(MaxHeap* h);

MinHeap* createMinHeap(int capacity);
void insertMinHeap(MinHeap* h, double val);
double getMin(MinHeap* h);
