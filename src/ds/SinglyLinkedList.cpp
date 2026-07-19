#include "SinglyLinkedList.h"
#include <iostream>

using namespace std;


SinglyLinkedList* createSinglyLinkedList() {
    SinglyLinkedList* sll = new SinglyLinkedList;
    sll->head = NULL;
    return sll;
}

void insertExpense(SinglyLinkedList* sll, Expense e) {
    Expense* newNode = new Expense;
    *newNode = e;
    newNode->next = NULL;

    if (sll->head == NULL) {
        sll->head = newNode;
    } else {
        Expense* temp = sll->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void printExpensesForProject(SinglyLinkedList* sll, int projectID) {
    Expense* temp = sll->head;
    while (temp != NULL) {
        if (temp->projectID == projectID) {
            cout << "  - Expense ID: " << temp->expenseID 
                      << " | Amount: " << temp->amount 
                      << " | Desc: " << temp->description 
                      << " | Date: " << temp->date << "\n";
        }
        temp = temp->next;
    }
}
