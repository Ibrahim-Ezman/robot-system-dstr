#include "ItemBST.hpp"
#include "CSVLoader.hpp"
#include <cctype>
#include <algorithm>
#include <fstream>

ItemBST::ItemBST()
{
    root = NULL;
}

ItemBST::~ItemBST()
{
    destroyTree(root);
}

void ItemBST::destroyTree(ItemNode* node)
{
    if (node != NULL)
    {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void ItemBST::insert(Item item)
{
    if (search(item.itemID) != NULL)
    {
        cout << "Error: Item " << item.itemID << " already exists" << endl;
        return;
    }
    
    root = insertHelper(root, item);
}

ItemNode* ItemBST::insertHelper(ItemNode* node, Item item)
{
    if (node == NULL)
    {
        ItemNode* newnode = new ItemNode;
        newnode->data = item;
        newnode->left = NULL;
        newnode->right = NULL;
        return newnode;
    }
    
    if (item.itemID < node->data.itemID)
    {
        node->left = insertHelper(node->left, item);
    }
    else if (item.itemID > node->data.itemID)
    {
        node->right = insertHelper(node->right, item);
    }
    return node;
}

Item* ItemBST::search(string itemID)
{
    ItemNode* result = searchHelper(root, itemID);
    
    if (result != NULL)
    {
        return &(result->data);
    }
    return NULL;
}

ItemNode* ItemBST::searchHelper(ItemNode* node, string itemID)
{
    if (node == NULL)
    {
        return NULL;
    }
    
    if (node->data.itemID == itemID)
    {
        return node;
    }
    
    if (itemID < node->data.itemID)
    {
        return searchHelper(node->left, itemID);
    }
    else
    {
        return searchHelper(node->right, itemID);
    }
}

Item* ItemBST::searchByName(string name)
{
    Item* result = NULL;
    searchByNameHelper(root, name, result);
    return result;
}

void ItemBST::searchByNameHelper(ItemNode* node, string name, Item*& result)
{
    if (node == NULL || result != NULL)
    {
        return;
    }
    
    string nodeName = node->data.name;
    transform(nodeName.begin(), nodeName.end(), nodeName.begin(), ::tolower);
    
    string searchName = name;
    transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
    
    if (nodeName.find(searchName) != string::npos)
    {
        result = &(node->data);
        return;
    }
    
    searchByNameHelper(node->left, name, result);
    searchByNameHelper(node->right, name, result);
}

void ItemBST::deleteItem(string itemID)
{
    if (isEmpty())
    {
        cout << "Error: Tree is empty" << endl;
        return;
    }
    
    if (search(itemID) == NULL)
    {
        cout << "Error: Item " << itemID << " not found" << endl;
        return;
    }
    
    root = deleteHelper(root, itemID);
    cout << "Item " << itemID << " deleted successfully" << endl;
}

ItemNode* ItemBST::deleteHelper(ItemNode* node, string itemID)
{
    if (node == NULL)
    {
        return NULL;
    }
    
    if (itemID < node->data.itemID)
    {
        node->left = deleteHelper(node->left, itemID);
    }
    else if (itemID > node->data.itemID)
    {
        node->right = deleteHelper(node->right, itemID);
    }
    else
    {
        if (node->left == NULL && node->right == NULL)
        {
            delete node;
            return NULL;
        }
        
        if (node->left == NULL)
        {
            ItemNode* temp = node->right;
            delete node;
            return temp;
        }
        
        if (node->right == NULL)
        {
            ItemNode* temp = node->left;
            delete node;
            return temp;
        }
        
        ItemNode* successor = findMin(node->right);
        node->data = successor->data;
        node->right = deleteHelper(node->right, successor->data.itemID);
    }
    
    return node;
}

ItemNode* ItemBST::findMin(ItemNode* node)
{
    while (node->left != NULL)
    {
        node = node->left;
    }
    
    return node;
}

void ItemBST::inOrderDisplay()
{
    if (isEmpty())
    {
        cout << "Item database is empty" << endl;
        return;
    }
    
    cout << "\n=== ITEM DATABASE (BST In-Order) ===" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    inOrderHelper(root);
    
    cout << "------------------------------------------------------------" << endl;
}

void ItemBST::inOrderHelper(ItemNode* node)
{
    if (node != NULL)
    {
        inOrderHelper(node->left);
        
        cout << "ID: " << node->data.itemID 
             << " | Name: " << node->data.name
             << " | Zone: " << node->data.zoneID
             << " | Aisle: " << node->data.aisleID
             << " | Shelf: " << node->data.shelfID
             << " | Qty: " << node->data.quantity
             << " | Weight: " << node->data.weight_kg << "kg" << endl;
        
        inOrderHelper(node->right);
    }
}

ItemLocation ItemBST::getLocation(string itemID)
{
    Item* item = search(itemID);
    
    if (item == NULL)
    {
        cout << "Item " << itemID << " not found" << endl;
        ItemLocation emptyLocation;
        emptyLocation.zoneID = "";
        emptyLocation.aisleID = "";
        emptyLocation.shelfID = "";
        emptyLocation.found = false;
        return emptyLocation;
    }
    
    ItemLocation location;
    location.zoneID = item->zoneID;
    location.aisleID = item->aisleID;
    location.shelfID = item->shelfID;
    location.found = true;
    return location;
}

bool ItemBST::isEmpty()
{
    return root == NULL;
}

void ItemBST::addItemWithSave(const string& filename)
{ 
    cout << "ADD NEW ITEM TO WAREHOUSE" << endl;

    string id;
    while (true)
    {
        cout << "Enter Item ID (e.g., ITEM_011): ";
        if (cin.peek() == '\n') cin.ignore(); 
        getline(cin, id);

        id.erase(0, id.find_first_not_of(" \t\r\n"));
        id.erase(id.find_last_not_of(" \t\r\n") + 1);

        if (id.empty())
        {
            cout << "[ERROR] Item ID cannot be empty\n" << endl;
            continue;
        }
        if (search(id) != NULL)
        {
            cout << "[ERROR] Item with ID " << id << " already exists in the system\n" << endl;
            return;
        }
        break;
    }

    Item newItem;
    newItem.itemID = id;

    while (true)
    {
        cout << "Enter Item Name: ";
        getline(cin, newItem.name);
        if (newItem.name.empty()) {
            cout << "[ERROR] Item Name cannot be empty\n" << endl;
            continue;
        }
        break;
    }

    while (true)
    {
        cout << "Enter Zone ID (e.g., A, B, C): ";
        getline(cin, newItem.zoneID);
        if (newItem.zoneID.empty()) {
            cout << "[ERROR] Zone ID cannot be empty\n" << endl;
            continue;
        }
        break;
    }

    while (true)
    {
        cout << "Enter Aisle ID (e.g., 1, 2): ";
        getline(cin, newItem.aisleID);
        if (newItem.aisleID.empty()) {
            cout << "[ERROR] Aisle ID cannot be empty\n" << endl;
            continue;
        }
        break;
    }

    while (true)
    {
        cout << "Enter Shelf ID (e.g., 3, 4): ";
        getline(cin, newItem.shelfID);
        if (newItem.shelfID.empty()) {
            cout << "[ERROR] Shelf ID cannot be empty\n" << endl;
            continue;
        }
        break;
    }

    string inputBuf;

    while (true)
    {
        cout << "Enter Quantity in Stock (Must be > 0): ";
        getline(cin, inputBuf);
        try {
            int q = stoi(inputBuf);
            if (q <= 0) {
                cout << "[ERROR] Quantity must be greater than 0\n" << endl;
                continue;
            }
            newItem.quantity = q;
            break;
        }
        catch (...) {
            cout << "[ERROR] Invalid input\n" << endl;
        }
    }

    while (true)
    {
        cout << "Enter Weight (kg) (Must be > 0): ";
        getline(cin, inputBuf);
        try {
            double w = stod(inputBuf);
            if (w <= 0) {
                cout << "[ERROR] Weight must be greater than 0\n" << endl;
                continue;
            }
            newItem.weight_kg = w;
            break;
        }
        catch (...) {
            cout << "[ERROR] Invalid input! Please enter a valid floating-point number\n" << endl;
        }
    }

    insert(newItem);

    ofstream fileOut(filename.c_str(), ios::app);
    if (!fileOut.is_open())
    {
        cout << "[ERROR] Could not open file " << filename << " to save the item permanently" << endl;
        cout << "Item is only saved in volatile memory (BST) for this session" << endl;
        return;
    }

    fileOut << newItem.itemID << ","
            << newItem.name << ","
            << newItem.zoneID << ","
            << newItem.aisleID << ","
            << newItem.shelfID << ","
            << newItem.quantity << ","
            << newItem.weight_kg << "\n";

    fileOut.close();

    cout << "\n[SUCCESS] Item " << id << " successfully added to BST and saved to " << filename << "!" << endl;
}

void ItemBST::updateItemQuantityAndLocation(string itemID, int newQty, string newZone, string newAisle, string newShelf, const char* csvFilename) 
{
    Item* item = search(itemID);
    
    if (item == NULL) 
    {
        cout << "\nError: Item " << itemID << " not found in BST system" << endl;
        return;
    }
    
    if (newQty >= 0) 
    {
        item->quantity = newQty;
    }
    
    if (!newZone.empty())  item->zoneID = newZone;
    if (!newAisle.empty()) item->aisleID = newAisle;
    if (!newShelf.empty()) item->shelfID = newShelf;
    
    updateItemInCSV(csvFilename, itemID, item->quantity, item->zoneID, item->aisleID, item->shelfID);
    
    cout << "\nItem " << itemID << " successfully updated in memory (BST) and logged to " << csvFilename << endl;
}