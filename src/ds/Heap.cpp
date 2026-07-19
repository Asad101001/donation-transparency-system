#include "Heap.h"


void swap(double& a, double& b) {
    double temp = a;
    a = b;
    b = temp;
}

// Max Heap
MaxHeap* createMaxHeap(int capacity) {
    MaxHeap* h = new MaxHeap;
    h->capacity = capacity;
    h->size = 0;
    h->array = new double[capacity];
    return h;
}

void insertMaxHeap(MaxHeap* h, double val) {
    if (h->size == h->capacity) return; // overflow
    h->size++;
    int i = h->size - 1;
    h->array[i] = val;

    while (i != 0 && h->array[(i - 1) / 2] < h->array[i]) {
        swap(h->array[i], h->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

double getMax(MaxHeap* h) {
    if (h->size <= 0) return 0.0;
    return h->array[0];
}

// Min Heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* h = new MinHeap;
    h->capacity = capacity;
    h->size = 0;
    h->array = new double[capacity];
    return h;
}

void insertMinHeap(MinHeap* h, double val) {
    if (h->size == h->capacity) return; // overflow
    h->size++;
    int i = h->size - 1;
    h->array[i] = val;

    while (i != 0 && h->array[(i - 1) / 2] > h->array[i]) {
        swap(h->array[i], h->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

double getMin(MinHeap* h) {
    if (h->size <= 0) return 0.0;
    return h->array[0];
}
