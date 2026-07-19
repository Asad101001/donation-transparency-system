#pragma once
#include "../models/Models.h"

struct AVLTree {
    Project* root;
};

AVLTree* createAVLTree();
Project* insertProject(Project* node, Project p);
Project* searchProject(Project* root, int projectID);
void inorderProjectTraversal(Project* root);
