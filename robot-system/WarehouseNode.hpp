#ifndef WAREHOUSENODE_HPP
#define WAREHOUSENODE_HPP

#include <string>
using namespace std;

const int MAX_CHILDREN = 20;

//step 1.0 define the warehouse node structure for N-ary tree
// i use a simple struct with only data fields and no constructor
// because warehouse nodes are created dynamically when building
// the tree from CSV file. each node has an ID, name, type (ROOT,
// ZONE, AISLE, SHELF), an array of child pointers, and a child count.
// i use a fixed size array children[MAX_CHILDREN] instead of dynamic
// array or linked list because the warehouse structure is known in
// advance - a zone has at most 20 aisles, an aisle has at most 20 shelves.
// fixed array is simpler and faster than dynamic allocation for each child.
// with plain struct i can create an empty node and fill fields one by one
// which is clearer than passing parameters to a constructor
struct WarehouseNode
{
    string nodeID;
    string nodeName;
    string nodeType;
    WarehouseNode* children[MAX_CHILDREN];
    int childCount;
};

#endif
