#include "Graph.h"
#include <iostream>

using namespace std;


Graph* createGraph(int V) {
    Graph* graph = new Graph;
    graph->V = V;
    graph->array = new AdjList[V];
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
    AdjListNode* newNode = new AdjListNode;
    newNode->destProjectID = dest;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

void printGraph(Graph* graph) {
    for (int v = 0; v < graph->V; ++v) {
        AdjListNode* pCrawl = graph->array[v].head;
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        while (pCrawl) {
            cout << "-> " << pCrawl->destProjectID;
            pCrawl = pCrawl->next;
        }
        cout << "\n";
    }
}
