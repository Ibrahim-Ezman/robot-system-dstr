#ifndef WAREHOUSETREE_HPP
#define WAREHOUSETREE_HPP

#include "WarehouseNode.hpp"
#include <iostream>

const int MAX_ROUTE_STEPS = 50;

//step 1.0 define the N-ary tree class for warehouse layout
// i use an N-ary tree instead of binary tree because the warehouse
// has a hierarchical structure with variable number of children at
// each level. the root is the warehouse, each zone has multiple
// aisles, each aisle has multiple shelves. with binary tree i could
// only have 2 children per node which is not enough. with N-ary tree
// each node can have up to MAX_CHILDREN (20) children which fits
// the warehouse structure. this is the same pattern as the N-ary
// tree implementation in our lecture slides
class WarehouseTree
{
private:
    WarehouseNode* root;
    
    //step 1.1 helper functions
    void destroyTree(WarehouseNode* node);
    void displayTreeHelper(WarehouseNode* node, int level);
    WarehouseNode* findNodeHelper(WarehouseNode* node, string nodeID);
    void dfsHelper(WarehouseNode* node);

public:
    WarehouseTree();
    ~WarehouseTree();
    
    //step 1.2 core operations
    void buildFromCSV(const char* filename);
    WarehouseNode* findNode(string nodeID);
    void displayTree();
    int generateRoute(string targetZone, string targetAisle, 
                      string targetShelf, string route[], int maxSize);
    void bfsTraversal();
    void dfsTraversal();
    bool isEmpty();
};

#endif
