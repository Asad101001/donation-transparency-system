#include "SinglyLinkedList.h"
#include <iostream>

using namespace std;

SinglyLinkedList* createSinglyLinkedList() {
    SinglyLinkedList* sll = new SinglyLinkedList;
    sll->head = NULL;
    return sll;
}

void insertExpense(SinglyLinkedList* sll, Expense e) {
    Expense* temp = new Expense;
    *temp = e;
    temp->next = NULL;

    if (sll->head == NULL) {
        sll->head = temp;
    } else {
        Expense* cur = sll->head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = temp;
    }
}

void printExpensesForProject(SinglyLinkedList* sll, int projectID) {
    Expense* cur = sll->head;
    while (cur != NULL) {
        if (cur->projectID == projectID) {
            cout << "  - Expense ID: " << cur->expenseID 
                      << " | Amount: " << cur->amount 
                      << " | Desc: " << cur->description 
                      << " | Date: " << cur->date << "\n";
        }
        cur = cur->next;
    }
}
