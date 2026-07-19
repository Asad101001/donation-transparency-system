#ifndef AVLTREE_H
#define AVLTREE_H

#include "../models/Models.h"

struct AVLTree {
    Project* root;
};

AVLTree* createAVLTree();
Project* insertProject(Project* node, Project p);
Project* searchProject(Project* root, int projectID);
void inorderProjectTraversal(Project* root);

#endif // AVLTREE_H
