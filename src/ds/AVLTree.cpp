#include "AVLTree.h"
#include <iostream>

using namespace std;

#ifndef NULL
#define NULL 0
#endif

AVLTree* createAVLTree() {
    AVLTree* tree = new AVLTree;
    tree->root = NULL;
    return tree;
}

int getHeight(Project* n) {
    if (n == NULL) return 0;
    return n->height;
}

int getBalance(Project* n) {
    if (n == NULL) return 0;
    return getHeight(n->left) - getHeight(n->right);
}

int maxVal(int a, int b) {
    return (a > b) ? a : b;
}

Project* rightRotate(Project* y) {
    Project* x = y->left;
    Project* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

Project* leftRotate(Project* x) {
    Project* y = x->right;
    Project* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

Project* insertProject(Project* node, Project p) {
    if (node == NULL) {
        Project* newNode = new Project;
        *newNode = p;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (p.projectID < node->projectID)
        node->left = insertProject(node->left, p);
    else if (p.projectID > node->projectID)
        node->right = insertProject(node->right, p);
    else
        return node;

    node->height = 1 + maxVal(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && p.projectID < node->left->projectID)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && p.projectID > node->right->projectID)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && p.projectID > node->left->projectID) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && p.projectID < node->right->projectID) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

Project* searchProject(Project* root, int projectID) {
    if (root == NULL || root->projectID == projectID)
        return root;
    
    if (root->projectID < projectID)
        return searchProject(root->right, projectID);
    
    return searchProject(root->left, projectID);
}

void inorderProjectTraversal(Project* root) {
    if (root != NULL) {
        inorderProjectTraversal(root->left);
        cout << "Project ID: " << root->projectID << " | Name: " << root->name << "\n";
        inorderProjectTraversal(root->right);
    }
}
