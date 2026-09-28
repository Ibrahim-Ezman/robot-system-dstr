#ifndef ITEMBST_HPP
#define ITEMBST_HPP

#include "Item.hpp"
#include <iostream>

struct ItemNode
{
    Item data;
    ItemNode* left;
    ItemNode* right;
};

class ItemBST
{
private:
    ItemNode* root;
    
    ItemNode* insertHelper(ItemNode* node, Item item);
    ItemNode* searchHelper(ItemNode* node, string itemID);
    void searchByNameHelper(ItemNode* node, string name, Item*& result);
    ItemNode* deleteHelper(ItemNode* node, string itemID);
    ItemNode* findMin(ItemNode* node);
    void inOrderHelper(ItemNode* node);
    void destroyTree(ItemNode* node);

public:
    ItemBST();
    ~ItemBST();
    
    void insert(Item item);
    Item* search(string itemID);
    Item* searchByName(string name);
    void deleteItem(string itemID);
    void inOrderDisplay();
    ItemLocation getLocation(string itemID);
    bool isEmpty();
    void addItemWithSave(const string& filename);
void updateItemQuantityAndLocation(string itemID, int newQty, string newZone, 
    string newAisle, string newShelf, const char* csvFilename);
};

#endif