#pragma once
struct AdjListNode {
    int destProjectID;
    AdjListNode* next;
};

struct AdjList {
    AdjListNode* head;
};

struct Graph {
    int V;
    AdjList* array;
};

Graph* createGraph(int V);
void addEdge(Graph* graph, int src, int dest);
void printGraph(Graph* graph);
