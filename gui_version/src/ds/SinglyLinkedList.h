#pragma once
#include "../models/Models.h"

struct SinglyLinkedList {
    Expense* head;
};

SinglyLinkedList* createSinglyLinkedList();
void insertExpense(SinglyLinkedList* sll, Expense e);
void printExpensesForProject(SinglyLinkedList* sll, int projectID);
