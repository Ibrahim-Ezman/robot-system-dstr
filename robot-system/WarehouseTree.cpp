#include "WarehouseTree.hpp"
#include "CSVLoader.hpp"
#include <fstream>
#include <sstream>

//Step 2: constructor
// i set root to NULL because the tree starts empty
// this is the same idea as setting head = NULL in linked list
WarehouseTree::WarehouseTree()
{
    root = NULL;
}

//Step 3: destructor
// without this, every WarehouseNode created with new would stay in
// memory even after the program finishes using the tree. i call
// destroyTree which recursively deletes all nodes starting from the root
WarehouseTree::~WarehouseTree()
{
    destroyTree(root);
}

//Step 4: destroy the tree recursively
// i use post-order traversal (children first, then node) because i
// need to delete children before deleting the parent. if i deleted
// the parent first, i would lose the pointers to the children and
// they would become memory leaks. post-order ensures that when i
// delete a node, all its children are already deleted. this is the
// same pattern as the tree destruction in our lecture slides
void WarehouseTree::destroyTree(WarehouseNode* node)
{
    //4.1 check if node is NULL (base case for recursion)
    if (node != NULL)
    {
        //4.2 recursively destroy all children
        for (int i = 0; i < node->childCount; i++)
        {
            destroyTree(node->children[i]);
        }
        
        //4.3 delete this node
        delete node;
    }
}

//Step 5: build the warehouse tree from CSV file
// this reads the warehouse layout from a CSV file and constructs
// the N-ary tree. i do this in two passes: first pass creates all
// nodes, second pass establishes parent-child relationships. i use
// two passes because when reading a child node, its parent might
// not be created yet if the parent appears later in the file. with
// two passes, all nodes exist before i start linking them. this is
// a common pattern for building trees from flat data
void WarehouseTree::buildFromCSV(const char* filename)
{
    //5.1 open the CSV file
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    //5.2 skip the header line
    string line;
    getline(file, line);
    
    //5.3 create temporary arrays to hold node data
    // i use fixed size arrays because the warehouse has at most 100 nodes
    WarehouseNode* nodes[100];
    string nodeIDs[100];
    string nodeNames[100];
    string nodeTypes[100];
    string parentIDs[100];
    int nodeCount = 0;
    
    //5.4 first pass: create all nodes
    while (getline(file, line) && nodeCount < 100)
    {
        //5.4.1 parse the CSV line
        stringstream ss(line);
        string nodeID, nodeName, nodeType, parentID;
        
        getline(ss, nodeID, ',');
        getline(ss, nodeName, ',');
        getline(ss, nodeType, ',');
        getline(ss, parentID, ',');
        
        //5.4.2 trim whitespace from all fields
        nodeID = trim(nodeID);
        nodeName = trim(nodeName);
        nodeType = trim(nodeType);
        parentID = trim(parentID);
        
        //5.4.3 store data for second pass
        nodeIDs[nodeCount] = nodeID;
        nodeNames[nodeCount] = nodeName;
        nodeTypes[nodeCount] = nodeType;
        parentIDs[nodeCount] = parentID;
        
        //5.4.4 create a new node and fill it with data
        WarehouseNode* newnode = new WarehouseNode;
        newnode->nodeID = nodeID;
        newnode->nodeName = nodeName;
        newnode->nodeType = nodeType;
        newnode->childCount = 0;
        
        //5.4.5 initialize all child pointers to NULL
        for (int i = 0; i < MAX_CHILDREN; i++)
        {
            newnode->children[i] = NULL;
        }
        
        //5.4.6 store the node in the array
        nodes[nodeCount] = newnode;
        
        //5.4.7 if this is the root node, set it as root
        if (nodeType == "ROOT")
        {
            root = newnode;
        }
        
        //5.4.8 increment node count
        nodeCount++;
    }
    
    //5.5 close the file
    file.close();
    
    //5.6 second pass: establish parent-child relationships
    for (int i = 0; i < nodeCount; i++)
    {
        //5.6.1 skip root node (it has no parent)
        if (parentIDs[i].empty() || nodeTypes[i] == "ROOT")
        {
            continue;
        }
        
        //5.6.2 find the parent node by ID
        for (int j = 0; j < nodeCount; j++)
        {
            if (nodeIDs[j] == parentIDs[i])
            {
                //5.6.2.1 add current node as child of parent
                // check if parent has room for more children
                if (nodes[j]->childCount < MAX_CHILDREN)
                {
                    nodes[j]->children[nodes[j]->childCount] = nodes[i];
                    nodes[j]->childCount++;
                }
                break;
            }
        }
    }
    
    //5.7 confirm success
    cout << "Warehouse layout tree built successfully from " << filename << endl;
    cout << "Total nodes loaded: " << nodeCount << endl;
}

//Step 6: find a node by its ID
// this searches the tree for a node with the given ID. i call the
// recursive helper function which does a depth-first search through
// the tree. if the node is found, the helper returns a pointer to it.
// if not found, the helper returns NULL
WarehouseNode* WarehouseTree::findNode(string nodeID)
{
    return findNodeHelper(root, nodeID);
}

//Step 7: recursive helper for finding a node
// this does a depth-first search through the N-ary tree. i check
// if the current node matches the search ID. if yes, i return it.
// if no, i recursively search all children. if any child search
// returns a non-NULL result, i return that result immediately.
// if all children return NULL, the node is not in this subtree.
// this is O(n) because i might need to check every node in the
// worst case. this is the same pattern as DFS in our lecture slides
WarehouseNode* WarehouseTree::findNodeHelper(WarehouseNode* node, string nodeID)
{
    //7.1 if node is NULL, not found (base case)
    if (node == NULL)
    {
        return NULL;
    }
    
    //7.2 if this node matches, return it
    if (node->nodeID == nodeID)
    {
        return node;
    }
    
    //7.3 recursively search all children
    for (int i = 0; i < node->childCount; i++)
    {
        WarehouseNode* result = findNodeHelper(node->children[i], nodeID);
        
        //7.3.1 if found in this child's subtree, return it
        if (result != NULL)
        {
            return result;
        }
    }
    
    //7.4 not found in this subtree
    return NULL;
}

//Step 8: display the tree with indentation
// i use indentation to show the tree structure visually. each level
// of the tree is indented more than its parent. this makes it easy
// to see the hierarchy: warehouse -> zones -> aisles -> shelves.
// i call the recursive helper which does a pre-order traversal
// (node first, then children) to print the tree top-down
void WarehouseTree::displayTree()
{
    //8.1 check if tree is empty
    if (isEmpty())
    {
        cout << "Warehouse tree is empty." << endl;
        return;
    }
    
    //8.2 print header
    cout << "\n=== WAREHOUSE LAYOUT TREE ===" << endl;
    
    //8.3 call recursive helper to print the tree
    displayTreeHelper(root, 0);
    
    //8.4 print footer
    cout << "=============================" << endl;
}

//Step 9: recursive helper for displaying the tree
// this does a pre-order traversal (node first, then children) and
// prints each node with indentation based on its level. level 0
// is the root (no indentation), level 1 is zones (2 spaces), level 2
// is aisles (4 spaces), level 3 is shelves (6 spaces). this visual
// structure helps the user understand the warehouse hierarchy
void WarehouseTree::displayTreeHelper(WarehouseNode* node, int level)
{
    //9.1 if node is NULL, return (base case)
    if (node == NULL)
    {
        return;
    }
    
    //9.2 print indentation based on level
    for (int i = 0; i < level; i++)
    {
        cout << "  ";
    }
    
    //9.3 print tree branch symbol for non-root nodes
    if (level > 0)
    {
        cout << "├── ";
    }
    
    //9.4 print node name and ID
    cout << node->nodeName << " [" << node->nodeID << "]" << endl;
    
    //9.5 recursively print all children with increased level
    for (int i = 0; i < node->childCount; i++)
    {
        displayTreeHelper(node->children[i], level + 1);
    }
}

//Step 10: generate a route from base to target shelf
// this creates a sequence of movement steps for the robot to reach
// a specific shelf. the route is: enter zone -> move to aisle ->
// move to shelf. i first find the zone, aisle, and shelf nodes in
// the tree to verify they exist. if any node is not found, i return
// 0 steps. if all nodes exist, i generate the route steps and store
// them in the route array. the caller can then push these steps onto
// the PathStack for the robot to follow
int WarehouseTree::generateRoute(string targetZone, string targetAisle, 
                                  string targetShelf, string route[], int maxSize)
{
    //10.1 initialize step counter
    int stepCount = 0;
    
    //10.2 build target node IDs based on naming convention
    // the CSV file uses a specific naming pattern for node IDs
    string zoneNodeID = "Z_" + targetZone;
    string aisleNodeID = "A_" + targetZone + targetAisle;
    string shelfNodeID = "S_" + targetZone + targetAisle + "_" + targetShelf;
    
    //10.3 find the zone node
    WarehouseNode* zoneNode = findNode(zoneNodeID);
    if (zoneNode == NULL)
    {
        cout << "Error: Zone " << targetZone << " not found in warehouse layout." << endl;
        return 0;
    }
    
    //10.4 find the aisle node
    WarehouseNode* aisleNode = findNode(aisleNodeID);
    if (aisleNode == NULL)
    {
        cout << "Error: Aisle " << targetAisle << " in Zone " << targetZone << " not found." << endl;
        return 0;
    }
    
    //10.5 find the shelf node
    WarehouseNode* shelfNode = findNode(shelfNodeID);
    if (shelfNode == NULL)
    {
        cout << "Error: Shelf " << targetShelf << " in Aisle " << targetAisle 
             << " Zone " << targetZone << " not found." << endl;
        return 0;
    }
    
    //10.6 generate route steps
    // check array bounds before adding each step
    if (stepCount < maxSize)
    {
        route[stepCount] = "ENTER_ZONE_" + targetZone;
        stepCount++;
    }
    
    if (stepCount < maxSize)
    {
        route[stepCount] = "MOVE_TO_AISLE_" + targetAisle;
        stepCount++;
    }
    
    if (stepCount < maxSize)
    {
        route[stepCount] = "MOVE_TO_SHELF_" + targetShelf;
        stepCount++;
    }
    
    //10.7 return the number of steps generated
    return stepCount;
}

//Step 11: breadth-first search traversal
// i use BFS to visit nodes level by level: first the root, then all
// zones, then all aisles, then all shelves. BFS uses a queue (FIFO).
// i enqueue the root, then repeatedly dequeue a node, print it, and
// enqueue all its children. this continues until the queue is empty.
// i use a simple array-based queue because the tree has at most 100
// nodes. BFS is useful for finding the shortest path in an unweighted
// tree. this is the same pattern as BFS in our lecture slides
void WarehouseTree::bfsTraversal()
{
    //11.1 check if tree is empty
    if (isEmpty())
    {
        cout << "Tree is empty." << endl;
        return;
    }
    
    //11.2 print header
    cout << "\n=== BFS TRAVERSAL ===" << endl;
    
    //11.3 create a simple queue using array
    WarehouseNode* queue[100];
    int front = 0;
    int rear = 0;
    
    //11.4 enqueue the root
    queue[rear] = root;
    rear++;
    
    //11.5 process queue until empty
    while (front < rear)
    {
        //11.5.1 dequeue a node
        WarehouseNode* current = queue[front];
        front++;
        
        //11.5.2 print the node
        cout << current->nodeName << " [" << current->nodeType << "] ";
        
        //11.5.3 enqueue all children
        for (int i = 0; i < current->childCount; i++)
        {
            if (rear < 100)
            {
                queue[rear] = current->children[i];
                rear++;
            }
        }
    }
    
    //11.6 print newline
    cout << endl;
}

//Step 12: depth-first search traversal
// i use DFS to visit nodes by going as deep as possible before
// backtracking. DFS visits: root, first zone and all its aisles
// and shelves, then second zone and all its aisles and shelves, etc.
// i call the recursive helper which does a pre-order traversal
// (node first, then children). DFS is useful for exploring all paths
// in a tree. this is the same pattern as DFS in our lecture slides
void WarehouseTree::dfsTraversal()
{
    //12.1 check if tree is empty
    if (isEmpty())
    {
        cout << "Tree is empty." << endl;
        return;
    }
    
    //12.2 print header
    cout << "\n=== DFS TRAVERSAL ===" << endl;
    
    //12.3 call recursive helper
    dfsHelper(root);
    
    //12.4 print newline
    cout << endl;
}

//Step 13: recursive helper for DFS
// this does a pre-order traversal (node first, then children) and
// prints each node. i print the current node, then recursively
// visit all children from left to right. this explores the tree
// depth-first, going all the way down one branch before moving to
// the next branch. this is O(n) because i visit every node exactly once
void WarehouseTree::dfsHelper(WarehouseNode* node)
{
    //13.1 if node is NULL, return (base case)
    if (node == NULL)
    {
        return;
    }
    
    //13.2 print this node
    cout << node->nodeName << " [" << node->nodeType << "] ";
    
    //13.3 recursively visit all children
    for (int i = 0; i < node->childCount; i++)
    {
        dfsHelper(node->children[i]);
    }
}

//Step 14: check if the tree is empty
// i check root == NULL instead of counting nodes. if root is NULL,
// the tree has no nodes. this is O(1) constant time
bool WarehouseTree::isEmpty()
{
    return root == NULL;
}
