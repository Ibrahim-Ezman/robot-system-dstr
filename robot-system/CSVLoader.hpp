#ifndef CSVLOADER_HPP
#define CSVLOADER_HPP

#include "Order.hpp"
#include "Robot.hpp"
#include "Item.hpp"
#include <string>
using namespace std;

//step 1.0 utility functions for CSV file operations

//step 1.1 read functions
int readOrders(const char* filename, Order outArray[], int maxSize);
int readRobots(const char* filename, Robot outArray[], int maxSize);
int readItems(const char* filename, Item outArray[], int maxSize);

//step 1.2 update functions
void updateOrderStatus(const char* filename, string orderID, string newStatus);
void updateRobotStatus(const char* filename, string robotID, string newStatus, 
                       string orderID);
void updateItemInCSV(const char* filename, string targetItemID, int newQty, 
    string newZone, string newAisle, string newShelf);

//step 1.3 append functions
void appendOrder(const char* filename, Order order);
void appendMovementLog(const char* filename, string robotID, string orderID,
                       string steps[], int stepCount, string phase);

//step 1.4 utility function to trim whitespace
string trim(string str);

#endif
