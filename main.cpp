#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cstdio>
#include "OrderQueue.hpp"
#include "RobotCircularQueue.hpp"
#include "PathStack.hpp"
#include "ItemBST.hpp"
#include "WarehouseTree.hpp"
#include "CSVLoader.hpp"

using namespace std;

//===================================================================
// SECTION 1: CONSTANTS AND GLOBAL DECLARATIONS
//===================================================================

const int MAX_ORDERS = 100;
const int MAX_ITEMS = 200;

//===================================================================
// SECTION 2: FUNCTION PROTOTYPES
//===================================================================

void displayMainMenu();
void addNewOrder(OrderQueue& orderQueue);
void processNextOrder(OrderQueue& orderQueue, RobotCircularQueue& robotQueue, 
                      ItemBST& itemBST, WarehouseTree& warehouseTree);
void viewMovementLog();
void searchItemByName(ItemBST& itemBST);
void deleteItemFromBST(ItemBST& itemBST);
void displayAllItemsSorted(ItemBST& itemBST);
void demonstrateBFSTraversal(WarehouseTree& warehouseTree);
void demonstrateDFSTraversal(WarehouseTree& warehouseTree);
void findWarehouseNode(WarehouseTree& warehouseTree);
void demonstratePathStack();
void showQueueStatistics(OrderQueue& orderQueue);
void showRobotStatistics(RobotCircularQueue& robotQueue);
void findSpecificRobot(RobotCircularQueue& robotQueue);
string generateOrderID();
string getCurrentTimestamp();
void addNewItemToSystem(ItemBST& itemBST);
void updateItemInSystemUI(ItemBST& itemBST); // Прототип функции точечного обновления

//===================================================================
// SECTION 3: MAIN FUNCTION
//===================================================================

int main()
{
    //3.1 display welcome banner
    cout << "\n============================================" << endl;
    cout << "  WAREHOUSE ROBOT NAVIGATION SYSTEM" << endl;
    cout << "  CT077-3-2-DSTR Lab Evaluation Work #2" << endl;
    cout << "============================================\n" << endl;
    
    //3.2 initialize data structures
    OrderQueue orderQueue;
    RobotCircularQueue robotQueue;
    ItemBST itemBST;
    WarehouseTree warehouseTree;
    
    //3.3 load all CSV files at startup
    cout << "Loading system data from CSV files..." << endl;
    cout << "--------------------------------------------" << endl;
    
    //3.3.1 load orders
    Order orders[MAX_ORDERS];
    int orderCount = readOrders("orders.csv", orders, MAX_ORDERS);
    cout << "✓ Loaded " << orderCount << " orders from orders.csv" << endl;
    
    //3.3.2 enqueue only PENDING orders
    int pendingCount = 0;
    for (int i = 0; i < orderCount; i++)
    {
        if (orders[i].status == "PENDING")
        {
            orderQueue.enqueue(orders[i]);
            pendingCount++;
        }
    }
    cout << "  → " << pendingCount << " PENDING orders added to queue" << endl;
    
    //3.3.3 load robots
    Robot robots[MAX_ROBOTS];
    int robotCount = readRobots("robots.csv", robots, MAX_ROBOTS);
    cout << "✓ Loaded " << robotCount << " robots from robots.csv" << endl;
    robotQueue.loadRobots(robots, robotCount);
    
    //3.3.4 load items into BST
    Item items[MAX_ITEMS];
    int itemCount = readItems("items.csv", items, MAX_ITEMS);
    cout << "✓ Loaded " << itemCount << " items from items.csv" << endl;
    for (int i = 0; i < itemCount; i++)
    {
        itemBST.insert(items[i]);
    }
    cout << "  → All items inserted into BST" << endl;
    
    //3.3.5 load warehouse layout
    warehouseTree.buildFromCSV("warehouse_layout.csv");
    
    cout << "--------------------------------------------" << endl;
    cout << "System initialization complete!\n" << endl;
    
    //3.4 main menu loop
    int choice;
    bool running = true;
    
    while (running)
    {
        displayMainMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        
        cout << endl;
        
        switch (choice)
        {
            case 1:
                addNewOrder(orderQueue);
                break;
                
            case 2:
                processNextOrder(orderQueue, robotQueue, itemBST, warehouseTree);
                break;
                
            case 3:
                orderQueue.displayAll();
                break;
                
            case 4:
                showQueueStatistics(orderQueue);
                break;
                
            case 5:
                robotQueue.displayAll();
                break;
                
            case 6:
                showRobotStatistics(robotQueue);
                break;
                
            case 7:
                findSpecificRobot(robotQueue);
                break;
                
            case 8:
            {
                string itemID;
                cout << "Enter Item ID to search: ";
                getline(cin, itemID);
                
                Item* item = itemBST.search(itemID);
                if (item != NULL)
                {
                    cout << "\n=== ITEM FOUND ===" << endl;
                    cout << "ID: " << item->itemID << endl;
                    cout << "Name: " << item->name << endl;
                    cout << "Location: Zone " << item->zoneID 
                         << ", Aisle " << item->aisleID 
                         << ", Shelf " << item->shelfID << endl;
                    cout << "Quantity: " << item->quantity << endl;
                    cout << "Weight: " << item->weight_kg << " kg" << endl;
                }
                else
                {
                    cout << "Item not found." << endl;
                }
                break;
            }
                
            case 9:
                searchItemByName(itemBST);
                break;
                
            case 10:
                displayAllItemsSorted(itemBST);
                break;
                
            case 11:
                deleteItemFromBST(itemBST);
                break;

            case 12: 
                addNewItemToSystem(itemBST);
                break;

            case 13: // Наш новый кейс для точечного апдейта
                updateItemInSystemUI(itemBST);
                break;
                
            case 14: // Сдвинут с 13
                warehouseTree.displayTree();
                break;
                
            case 15: // Сдвинут с 14
                demonstrateBFSTraversal(warehouseTree);
                break;
                
            case 16: // Сдвинут с 15
                demonstrateDFSTraversal(warehouseTree);
                break;
                
            case 17: // Сдвинут с 16
                findWarehouseNode(warehouseTree);
                break;
                
            case 18: // Сдвинут с 17
                demonstratePathStack();
                break;
                
            case 19: // Сдвинут с 18
                viewMovementLog();
                break;
                
            case 0:
                cout << "Goodbye. All data has been saved." << endl;
                running = false;
                break;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        
        if (running)
        {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
    
    return 0;
}

//===================================================================
// SECTION 4: MENU AND USER INTERFACE FUNCTIONS
//===================================================================

void displayMainMenu()
{
    cout << "\n============================================" << endl;
    cout << "  WAREHOUSE ROBOT NAVIGATION SYSTEM" << endl;
    cout << "============================================" << endl;
    cout << "  ORDER MANAGEMENT:" << endl;
    cout << "  [1] Add New Order" << endl;
    cout << "  [2] Process Next Order (Full Pipeline)" << endl;
    cout << "  [3] View Pending Orders" << endl;
    cout << "  [4] Show Queue Statistics" << endl;
    cout << "\n  ROBOT MANAGEMENT:" << endl;
    cout << "  [5] View Robot Status" << endl;
    cout << "  [6] Show Robot Statistics" << endl;
    cout << "  [7] Find Specific Robot by ID" << endl;
    cout << "\n  ITEM MANAGEMENT (BST):" << endl;
    cout << "  [8] Search Item by ID" << endl;
    cout << "  [9] Search Item by Name" << endl;
    cout << "  [10] Display All Items (Sorted)" << endl;
    cout << "  [11] Delete Item from BST" << endl;
    cout << "  [12] Add New Item (BST & CSV)" << endl;
    cout << "  [13] Update Existing Item (BST & CSV)" << endl; // Добавлен пункт меню
    cout << "\n  WAREHOUSE LAYOUT (N-ary Tree):" << endl;
    cout << "  [14] Display Warehouse Tree" << endl;
    cout << "  [15] BFS Traversal" << endl;
    cout << "  [16] DFS Traversal" << endl;
    cout << "  [17] Find Warehouse Node by ID" << endl;
    cout << "\n  PATH TRACKING (Stack):" << endl;
    cout << "  [18] Demonstrate Path Stack" << endl;
    cout << "\n  LOGS & REPORTS:" << endl;
    cout << "  [19] View Movement Log" << endl;
    cout << "\n  [0] Exit" << endl;
    cout << "============================================" << endl;
}

void addNewOrder(OrderQueue& orderQueue)
{
    cout << "=== ADD NEW ORDER ===" << endl;
    
    string itemID, customerName, priority;
    
    cout << "Enter Item ID: ";
    getline(cin, itemID);
    
    cout << "Enter Customer Name: ";
    getline(cin, customerName);
    
    cout << "Enter Priority (NORMAL/HIGH): ";
    getline(cin, priority);
    
    //4.1 generate order ID and timestamp
    string orderID = generateOrderID();
    string timestamp = getCurrentTimestamp();
    
    //4.2 create order struct and fill it
    Order newOrder;
    newOrder.orderID = orderID;
    newOrder.itemID = itemID;
    newOrder.customerName = customerName;
    newOrder.priority = priority;
    newOrder.status = "PENDING";
    newOrder.timestamp = timestamp;
    
    //4.3 enqueue and append to CSV
    orderQueue.enqueue(newOrder);
    appendOrder("orders.csv", newOrder);
    
    cout << "\n✓ Order " << orderID << " added successfully!" << endl;
}

//===================================================================
// SECTION 5: ORDER PROCESSING PIPELINE
//===================================================================

void processNextOrder(OrderQueue& orderQueue, RobotCircularQueue& robotQueue, 
                      ItemBST& itemBST, WarehouseTree& warehouseTree)
{
    cout << "=== PROCESSING NEXT ORDER (FULL PIPELINE) ===" << endl;
    cout << "--------------------------------------------" << endl;
    
    //5.1 check if queue is empty
    if (orderQueue.isEmpty())
    {
        cout << "No pending orders in queue." << endl;
        return;
    }
    
    //5.2 peek at front order
    Order currentOrder = orderQueue.peek();
    cout << "Step 1: Next order in queue:" << endl;
    cout << "  Order ID: " << currentOrder.orderID << endl;
    cout << "  Item ID: " << currentOrder.itemID << endl;
    cout << "  Customer: " << currentOrder.customerName << endl;
    cout << "  Priority: " << currentOrder.priority << endl;
    cout << endl;
    
    //5.3 assign robot
    cout << "Step 2: Assigning robot..." << endl;
    Robot* assignedRobot = robotQueue.assignNext();
    
    if (assignedRobot == NULL)
    {
        cout << "  ✗ No robots available. Order remains in queue." << endl;
        return;
    }
    
    cout << "  ✓ Robot " << assignedRobot->robotID << " (" << assignedRobot->name 
         << ") assigned!" << endl;
    assignedRobot->currentOrderID = currentOrder.orderID;
    cout << endl;
    
    //5.4 dequeue order
    orderQueue.dequeue();
    updateOrderStatus("orders.csv", currentOrder.orderID, "PROCESSING");
    updateRobotStatus("robots.csv", assignedRobot->robotID, "BUSY", currentOrder.orderID);
    cout << "Step 3: Order dequeued and status updated to PROCESSING" << endl;
    cout << endl;
    
    //5.5 search item location
    cout << "Step 4: Searching for item location..." << endl;
    Item* item = itemBST.search(currentOrder.itemID);
    
    if (item == NULL)
    {
        cout << "  ✗ Item not found in database. Aborting." << endl;
        robotQueue.setStatus(assignedRobot->robotID, "AVAILABLE", "");
        updateRobotStatus("robots.csv", assignedRobot->robotID, "AVAILABLE", "");
        return;
    }
    
    cout << "  ✓ Item found: " << item->name << endl;
    cout << "  Location: Zone " << item->zoneID << ", Aisle " << item->aisleID 
         << ", Shelf " << item->shelfID << endl;
    cout << endl;
    
    //5.6 generate route
    cout << "Step 5: Generating navigation route..." << endl;
    string route[MAX_ROUTE_STEPS];
    int stepCount = warehouseTree.generateRoute(item->zoneID, item->aisleID, 
                                                item->shelfID, route, MAX_ROUTE_STEPS);
    
    if (stepCount == 0)
    {
        cout << "  ✗ Could not generate route. Aborting." << endl;
        robotQueue.setStatus(assignedRobot->robotID, "AVAILABLE", "");
        updateRobotStatus("robots.csv", assignedRobot->robotID, "AVAILABLE", "");
        return;
    }
    
    cout << "  ✓ Route generated with " << stepCount << " steps" << endl;
    cout << endl;
    
    //5.7 push steps onto stack and simulate forward movement
    cout << "Step 6: Robot moving forward..." << endl;
    PathStack pathStack;
    
    for (int i = 0; i < stepCount; i++)
    {
        pathStack.push(route[i]);
        cout << "  → " << route[i] << endl;
    }
    
    cout << "\n=== DESTINATION REACHED ===" << endl;
    cout << "Robot " << assignedRobot->robotID << " picked up item: " << item->name << endl;
    cout << endl;
    
    //5.8 return journey (reverse path)
    cout << "Step 7: Robot returning to base..." << endl;
    pathStack.reverseReturn();
    cout << endl;
    
    //5.9 write to movement log
    appendMovementLog("movement_log.csv", assignedRobot->robotID, currentOrder.orderID,
                      route, stepCount, "FORWARD");
    
    //5.9.1 create reversed steps for log
    string returnSteps[MAX_ROUTE_STEPS];
    for (int i = 0; i < stepCount; i++)
    {
        returnSteps[i] = route[stepCount - 1 - i];
    }
    appendMovementLog("movement_log.csv", assignedRobot->robotID, currentOrder.orderID,
                      returnSteps, stepCount, "RETURN");
    
    //5.10 update statuses
    cout << "Step 8: Updating system status..." << endl;
    updateOrderStatus("orders.csv", currentOrder.orderID, "COMPLETED");
    cout << "  ✓ Order " << currentOrder.orderID << " marked as COMPLETED" << endl;
    
    robotQueue.setStatus(assignedRobot->robotID, "AVAILABLE", "");
    updateRobotStatus("robots.csv", assignedRobot->robotID, "AVAILABLE", "");
    cout << "  ✓ Robot " << assignedRobot->robotID << " marked as AVAILABLE" << endl;
    
    cout << "\n=== ORDER PROCESSING COMPLETE ===" << endl;
    cout << "--------------------------------------------" << endl;
}

//===================================================================
// SECTION 6: UTILITY FUNCTIONS
//===================================================================

void viewMovementLog()
{
    ifstream file("movement_log.csv");
    if (!file.is_open())
    {
        cout << "Error: Cannot open movement_log.csv" << endl;
        return;
    }
    
    cout << "\n=== MOVEMENT LOG ===" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    string line;
    getline(file, line);
    cout << "LogID | RobotID | OrderID | Step | Direction | Phase" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    while (getline(file, line))
    {
        stringstream ss(line);
        string logID, robotID, orderID, stepNo, direction, timestamp, phase;
        
        getline(ss, logID, ',');
        getline(ss, robotID, ',');
        getline(ss, orderID, ',');
        getline(ss, stepNo, ',');
        getline(ss, direction, ',');
        getline(ss, timestamp, ',');
        getline(ss, phase, ',');
        
        cout << trim(logID) << " | " << trim(robotID) << " | " << trim(orderID) 
             << " | " << trim(stepNo) << " | " << trim(direction) 
             << " | " << trim(phase) << endl;
    }
    
    cout << "------------------------------------------------------------" << endl;
    file.close();
}

string generateOrderID()
{
    static int counter = 100;
    counter++;
    return "ORD" + string(3 - to_string(counter).length(), '0') + to_string(counter);
}

string getCurrentTimestamp()
{
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d",
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
             ltm->tm_hour, ltm->tm_min);
    
    return string(buffer);
}

//===================================================================
// SECTION 7: ITEM BST SEARCH AND MANAGEMENT FUNCTIONS
//===================================================================

void searchItemByName(ItemBST& itemBST)
{
    cout << "=== SEARCH ITEM BY NAME ===" << endl;
    
    string name;
    cout << "Enter item name (or part of name): ";
    getline(cin, name);
    
    cout << "\nSearching for items containing: \"" << name << "\"..." << endl;
    
    Item* item = itemBST.searchByName(name);
    
    if (item != NULL)
    {
        cout << "\n=== ITEM FOUND ===" << endl;
        cout << "ID: " << item->itemID << endl;
        cout << "Name: " << item->name << endl;
        cout << "Location: Zone " << item->zoneID 
             << ", Aisle " << item->aisleID 
             << ", Shelf " << item->shelfID << endl;
        cout << "Quantity: " << item->quantity << endl;
        cout << "Weight: " << item->weight_kg << " kg" << endl;
    }
    else
    {
        cout << "\nNo item found with name containing: \"" << name << "\"" << endl;
    }
}

void deleteItemFromBST(ItemBST& itemBST)
{
    cout << "=== DELETE ITEM FROM BST ===" << endl;
    
    string itemID;
    cout << "Enter Item ID to delete: ";
    getline(cin, itemID);
    
    cout << "\nAttempting to delete item: " << itemID << "..." << endl;
    
    itemBST.deleteItem(itemID);
}

void displayAllItemsSorted(ItemBST& itemBST)
{
    cout << "=== DISPLAY ALL ITEMS (IN-ORDER TRAVERSAL) ===" << endl;
    cout << "Items are displayed in sorted order by Item ID" << endl;
    cout << "This demonstrates BST in-order traversal (O(n))" << endl;
    
    itemBST.inOrderDisplay();
}

void addNewItemToSystem(ItemBST& itemBST)
{
    itemBST.addItemWithSave("items.csv");
}

// Новая интерфейсная функция сбора ввода для точечного обновления
// Обновленная интерфейсная функция сбора ввода с защитой от пустых значений
void updateItemInSystemUI(ItemBST& itemBST)
{
    cout << "=== UPDATE EXISTING ITEM ===" << endl;
    
    string itemID;
    cout << "Enter Item ID to update: ";
    getline(cin, itemID);
    
    // Проверяем существование товара в дереве
    Item* item = itemBST.search(itemID);
    if (item == NULL)
    {
        cout << "✗ Error: Item with ID " << itemID << " does not exist." << endl;
        return;
    }
    
    cout << "\n--- Current Details ---" << endl;
    cout << "Name: " << item->name << endl;
    cout << "Location: Zone " << item->zoneID << ", Aisle " << item->aisleID << ", Shelf " << item->shelfID << endl;
    cout << "Quantity: " << item->quantity << "\n-----------------------" << endl;
    cout << "(Leave field blank and press Enter to keep current value)\n" << endl;
    
    string newZone, newAisle, newShelf, inputBuf;
    int newQty = item->quantity; // По умолчанию оставляем текущее количество
    
    cout << "Enter New Zone ID [" << item->zoneID << "]: ";
    getline(cin, newZone);
    if (newZone.empty()) {
        newZone = item->zoneID; // Если пусто, сохраняем старое
    }
    
    cout << "Enter New Aisle ID [" << item->aisleID << "]: ";
    getline(cin, newAisle);
    if (newAisle.empty()) {
        newAisle = item->aisleID;
    }
    
    cout << "Enter New Shelf ID [" << item->shelfID << "]: ";
    getline(cin, newShelf);
    if (newShelf.empty()) {
        newShelf = item->shelfID;
    }
    
    // Безопасный ввод количества с защитой от stoi() crash
    while (true) {
        cout << "Enter New Quantity [" << item->quantity << "]: ";
        getline(cin, inputBuf);
        
        if (inputBuf.empty()) {
            // Пользователь нажал Enter, оставляем старое значение
            newQty = item->quantity;
            break;
        }
        
        try {
            newQty = stoi(inputBuf);
            if (newQty < 0) {
                cout << "[ERROR] Quantity cannot be negative. Please try again." << endl;
                continue;
            }
            break; // Ввод корректен, выходим из цикла
        }
        catch (const invalid_argument& e) {
            cout << "[ERROR] Invalid input! Please enter a valid integer number or leave blank." << endl;
        }
        catch (const out_of_range& e) {
            cout << "[ERROR] Number is too large. Please try again." << endl;
        }
    }
    
    // Вызываем точечное связывающее обновление памяти и диска
    itemBST.updateItemQuantityAndLocation(itemID, newQty, newZone, newAisle, newShelf, "items.csv");
}

//===================================================================
// SECTION 8: WAREHOUSE TREE TRAVERSAL FUNCTIONS
//===================================================================

void demonstrateBFSTraversal(WarehouseTree& warehouseTree)
{
    cout << "=== BREADTH-FIRST SEARCH (BFS) TRAVERSAL ===" << endl;
    cout << "BFS visits nodes level by level:" << endl;
    cout << "1. Root (Warehouse)" << endl;
    cout << "2. All Zones" << endl;
    cout << "3. All Aisles" << endl;
    cout << "4. All Shelves" << endl;
    cout << "\nThis uses a QUEUE (FIFO) data structure." << endl;
    
    warehouseTree.bfsTraversal();
    
    cout << "\nBFS is useful for finding shortest paths in unweighted trees." << endl;
}

void demonstrateDFSTraversal(WarehouseTree& warehouseTree)
{
    cout << "=== DEPTH-FIRST SEARCH (DFS) TRAVERSAL ===" << endl;
    cout << "DFS visits nodes by going as deep as possible first:" << endl;
    cout << "1. Root -> First Zone -> First Aisle -> All Shelves" << endl;
    cout << "2. Then backtrack to next Aisle, etc." << endl;
    cout << "\nThis uses RECURSION (implicit STACK)." << endl;
    
    warehouseTree.dfsTraversal();
    
    cout << "\nDFS is useful for exploring all paths in a tree." << endl;
}

void findWarehouseNode(WarehouseTree& warehouseTree)
{
    cout << "=== FIND WAREHOUSE NODE BY ID ===" << endl;
    cout << "Node ID format examples:" << endl;
    cout << "  - Zone: Z_A, Z_B, Z_C" << endl;
    cout << "  - Aisle: A_A1, A_A2, A_B1" << endl;
    cout << "  - Shelf: S_A1_1, S_A1_2, S_B1_1" << endl;
    
    string nodeID;
    cout << "\nEnter Node ID: ";
    getline(cin, nodeID);
    
    cout << "\nSearching for node: " << nodeID << "..." << endl;
    
    WarehouseNode* node = warehouseTree.findNode(nodeID);
    
    if (node != NULL)
    {
        cout << "\n=== NODE FOUND ===" << endl;
        cout << "Node ID: " << node->nodeID << endl;
        cout << "Node Name: " << node->nodeName << endl;
        cout << "Node Type: " << node->nodeType << endl;
        cout << "Number of Children: " << node->childCount << endl;
        
        if (node->childCount > 0)
        {
            cout << "\nChildren:" << endl;
            for (int i = 0; i < node->childCount; i++)
            {
                cout << "  " << (i+1) << ". " << node->children[i]->nodeName 
                     << " [" << node->children[i]->nodeID << "]" << endl;
            }
        }
    }
    else
    {
        cout << "\nNode not found: " << nodeID << endl;
    }
}

//===================================================================
// SECTION 9: PATH STACK DEMONSTRATION
//===================================================================

void demonstratePathStack()
{
    cout << "=== PATH STACK DEMONSTRATION ===" << endl;
    cout << "This demonstrates how the robot tracks its path using a STACK (LIFO)." << endl;
    cout << "\nCreating a sample path..." << endl;
    
    PathStack demoStack;
    
    cout << "\nPushing steps onto stack:" << endl;
    demoStack.push("ENTER_ZONE_A");
    cout << "  1. ENTER_ZONE_A" << endl;
    
    demoStack.push("MOVE_TO_AISLE_1");
    cout << "  2. MOVE_TO_AISLE_1" << endl;
    
    demoStack.push("MOVE_TO_SHELF_3");
    cout << "  3. MOVE_TO_SHELF_3" << endl;
    
    cout << "\nStack size: " << demoStack.getSize() << " steps" << endl;
    
    cout << "\nPeeking at top of stack (last step): " << demoStack.peek() << endl;
    
    cout << "\n--- FORWARD PATH ---" << endl;
    demoStack.displayForwardPath();
    
    cout << "\n--- RETURN PATH (REVERSED) ---" << endl;
    cout << "Popping from stack and reversing directions:" << endl;
    demoStack.reverseReturn();
    
    cout << "\nStack is now empty. Size: " << demoStack.getSize() << endl;
    
    cout << "\nThis demonstrates:" << endl;
    cout << "  - LIFO (Last In First Out) behavior" << endl;
    cout << "  - Automatic path reversal for return journey" << endl;
    cout << "  - O(1) push and pop operations" << endl;
}

//===================================================================
// SECTION 10: QUEUE AND ROBOT STATISTICS
//===================================================================

void showQueueStatistics(OrderQueue& orderQueue)
{
    cout << "=== ORDER QUEUE STATISTICS ===" << endl;
    
    int queueSize = orderQueue.getSize();
    bool empty = orderQueue.isEmpty();
    
    cout << "Total pending orders: " << queueSize << endl;
    cout << "Queue status: " << (empty ? "EMPTY" : "HAS ORDERS") << endl;
    
    if (!empty)
    {
        Order frontOrder = orderQueue.peek();
        cout << "\nNext order to be processed:" << endl;
        cout << "  Order ID: " << frontOrder.orderID << endl;
        cout << "  Item ID: " << frontOrder.itemID << endl;
        cout << "  Customer: " << frontOrder.customerName << endl;
        cout << "  Priority: " << frontOrder.priority << endl;
        cout << "  Timestamp: " << frontOrder.timestamp << endl;
    }
    
    cout << "\nQueue uses FIFO (First In First Out) principle." << endl;
    cout << "Oldest order is always processed first for fairness." << endl;
}

void showRobotStatistics(RobotCircularQueue& robotQueue)
{
    cout << "=== ROBOT CIRCULAR QUEUE STATISTICS ===" << endl;
    
    int totalRobots = robotQueue.getCount();
    bool allBusy = robotQueue.allBusy();
    
    cout << "Total robots in system: " << totalRobots << endl;
    cout << "All robots busy: " << (allBusy ? "YES" : "NO") << endl;
    
    cout << "\nCircular Queue uses round-robin assignment:" << endl;
    cout << "  - Each robot gets tasks in turn" << endl;
    cout << "  - Fair distribution of workload" << endl;
    cout << "  - O(1) assignment time" << endl;
    
    cout << "\nDetailed robot status:" << endl;
    robotQueue.displayAll();
}

void findSpecificRobot(RobotCircularQueue& robotQueue)
{
    cout << "=== FIND SPECIFIC ROBOT ===" << endl;
    
    string robotID;
    cout << "Enter Robot ID (e.g., R001, R002): ";
    getline(cin, robotID);
    
    cout << "\nSearching for robot: " << robotID << "..." << endl;
    
    Robot* robot = robotQueue.findRobot(robotID);
    
    if (robot != NULL)
    {
        cout << "\n=== ROBOT FOUND ===" << endl;
        cout << "Robot ID: " << robot->robotID << endl;
        cout << "Name: " << robot->name << endl;
        cout << "Status: " << robot->status << endl;
        cout << "Current Order: " << (robot->currentOrderID.empty() ? "None" : robot->currentOrderID) << endl;
        cout << "Total Tasks Completed: " << robot->totalTasksDone << endl;
    }
    else
    {
        cout << "\nRobot not found: " << robotID << endl;
    }
}