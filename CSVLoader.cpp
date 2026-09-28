#include "CSVLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>

//Step 1: trim whitespace from string
// this removes leading and trailing spaces, tabs, and newlines from
// a string. i use this when reading CSV files because CSV fields
// often have extra whitespace that should be removed. i find the
// first non-whitespace character and the last non-whitespace character,
// then extract the substring between them. if the string is all
// whitespace, i return an empty string
string trim(string str)
{
    //1.1 find first non-whitespace character
    size_t first = str.find_first_not_of(" \t\r\n");
    
    //1.2 if string is all whitespace, return empty string
    if (first == string::npos)
    {
        return "";
    }
    
    //1.3 find last non-whitespace character
    size_t last = str.find_last_not_of(" \t\r\n");
    
    //1.4 extract and return the trimmed substring
    return str.substr(first, (last - first + 1));
}

//Step 2: read orders from CSV file
// this reads all orders from a CSV file and stores them in an array.
// i skip the header line, then read each line and parse it into
// fields separated by commas. i create an Order struct for each line
// and store it in the output array. i return the number of orders
// read. this is used at program startup to load all pending orders
int readOrders(const char* filename, Order outArray[], int maxSize)
{
    //2.1 open the CSV file
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return 0;
    }
    
    //2.2 skip the header line
    string line;
    getline(file, line);
    
    //2.3 read each line and parse it
    int count = 0;
    while (getline(file, line) && count < maxSize)
    {
        //2.3.1 create a stringstream to parse the line
        stringstream ss(line);
        string orderID, itemID, customerName, priority, status, timestamp;
        
        //2.3.2 extract each field separated by comma
        getline(ss, orderID, ',');
        getline(ss, itemID, ',');
        getline(ss, customerName, ',');
        getline(ss, priority, ',');
        getline(ss, status, ',');
        getline(ss, timestamp, ',');
        
        //2.3.3 create an Order struct and fill it with trimmed data
        outArray[count].orderID = trim(orderID);
        outArray[count].itemID = trim(itemID);
        outArray[count].customerName = trim(customerName);
        outArray[count].priority = trim(priority);
        outArray[count].status = trim(status);
        outArray[count].timestamp = trim(timestamp);
        
        //2.3.4 increment count
        count++;
    }
    
    //2.4 close the file
    file.close();
    
    //2.5 return the number of orders read
    return count;
}

//Step 3: read robots from CSV file
// this reads all robots from a CSV file and stores them in an array.
// i skip the header line, then read each line and parse it into
// fields separated by commas. i create a Robot struct for each line
// and store it in the output array. i return the number of robots
// read. this is used at program startup to load all available robots
int readRobots(const char* filename, Robot outArray[], int maxSize)
{
    //3.1 open the CSV file
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return 0;
    }
    
    //3.2 skip the header line
    string line;
    getline(file, line);
    
    //3.3 read each line and parse it
    int count = 0;
    while (getline(file, line) && count < maxSize)
    {
        //3.3.1 create a stringstream to parse the line
        stringstream ss(line);
        string robotID, name, status, currentOrderID, tasksStr;
        
        //3.3.2 extract each field separated by comma
        getline(ss, robotID, ',');
        getline(ss, name, ',');
        getline(ss, status, ',');
        getline(ss, currentOrderID, ',');
        getline(ss, tasksStr, ',');
        
        //3.3.3 convert tasks string to integer
        int tasks = 0;
        if (!tasksStr.empty())
        {
            tasks = stoi(trim(tasksStr));
        }
        
        //3.3.4 create a Robot struct and fill it with trimmed data
        outArray[count].robotID = trim(robotID);
        outArray[count].name = trim(name);
        outArray[count].status = trim(status);
        outArray[count].currentOrderID = trim(currentOrderID);
        outArray[count].totalTasksDone = tasks;
        
        //3.3.5 increment count
        count++;
    }
    
    //3.4 close the file
    file.close();
    
    //3.5 return the number of robots read
    return count;
}

//Step 4: read items from CSV file
// this reads all items from a CSV file and stores them in an array.
// i skip the header line, then read each line and parse it into
// fields separated by commas. i create an Item struct for each line
// and store it in the output array. i return the number of items
// read. this is used at program startup to load all inventory items
int readItems(const char* filename, Item outArray[], int maxSize)
{
    //4.1 open the CSV file
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return 0;
    }
    
    //4.2 skip the header line
    string line;
    getline(file, line);
    
    //4.3 read each line and parse it
    int count = 0;
    while (getline(file, line) && count < maxSize)
    {
        //4.3.1 create a stringstream to parse the line
        stringstream ss(line);
        string itemID, name, zoneID, aisleID, shelfID, qtyStr, weightStr;
        
        //4.3.2 extract each field separated by comma
        getline(ss, itemID, ',');
        getline(ss, name, ',');
        getline(ss, zoneID, ',');
        getline(ss, aisleID, ',');
        getline(ss, shelfID, ',');
        getline(ss, qtyStr, ',');
        getline(ss, weightStr, ',');
        
        //4.3.3 convert quantity string to integer
        int qty = 0;
        if (!qtyStr.empty())
        {
            qty = stoi(trim(qtyStr));
        }
        
        //4.3.4 convert weight string to double
        double weight = 0.0;
        if (!weightStr.empty())
        {
            weight = stod(trim(weightStr));
        }
        
        //4.3.5 create an Item struct and fill it with trimmed data
        outArray[count].itemID = trim(itemID);
        outArray[count].name = trim(name);
        outArray[count].zoneID = trim(zoneID);
        outArray[count].aisleID = trim(aisleID);
        outArray[count].shelfID = trim(shelfID);
        outArray[count].quantity = qty;
        outArray[count].weight_kg = weight;
        
        //4.3.6 increment count
        count++;
    }
    
    //4.4 close the file
    file.close();
    
    //4.5 return the number of items read
    return count;
}

//Step 5: update order status in CSV file
// this finds an order by ID in the CSV file and updates its status.
// i read all lines into memory, find the matching order, update its
// status field, then write all lines back to the file. i use this
// approach because CSV files do not support in-place updates - i
// must rewrite the entire file. this is called when an order is
// assigned to a robot (status becomes PROCESSING) or completed
void updateOrderStatus(const char* filename, string orderID, string newStatus)
{
    //5.1 open the CSV file for reading
    ifstream fileIn(filename);
    if (!fileIn.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    //5.2 create an array to hold all lines
    string lines[200];
    int lineCount = 0;
    string line;
    
    //5.3 read header line
    getline(fileIn, line);
    lines[lineCount] = line;
    lineCount++;
    
    //5.4 read and update matching order
    while (getline(fileIn, line) && lineCount < 200)
    {
        //5.4.1 parse the line
        stringstream ss(line);
        string oid, itemID, customerName, priority, status, timestamp;
        
        getline(ss, oid, ',');
        getline(ss, itemID, ',');
        getline(ss, customerName, ',');
        getline(ss, priority, ',');
        getline(ss, status, ',');
        getline(ss, timestamp, ',');
        
        //5.4.2 check if this is the order to update
        if (trim(oid) == orderID)
        {
            //5.4.2.1 rebuild the line with new status
            lines[lineCount] = trim(oid) + "," + trim(itemID) + "," + 
                              trim(customerName) + "," + trim(priority) + "," + 
                              newStatus + "," + trim(timestamp);
        }
        else
        {
            //5.4.2.2 keep the line unchanged
            lines[lineCount] = line;
        }
        
        //5.4.3 increment line count
        lineCount++;
    }
    
    //5.5 close input file
    fileIn.close();
    
    //5.6 open the file for writing
    ofstream fileOut(filename);
    
    //5.7 write all lines back to file
    for (int i = 0; i < lineCount; i++)
    {
        fileOut << lines[i] << endl;
    }
    
    //5.8 close output file
    fileOut.close();
}

//Step 6: update robot status in CSV file
// this finds a robot by ID in the CSV file and updates its status
// and current order assignment. i read all lines into memory, find
// the matching robot, update its fields, then write all lines back
// to the file. if the robot is completing a task (status becomes
// AVAILABLE and orderID is empty), i also increment its task counter.
// this is called when a robot is assigned to an order or completes an order
void updateRobotStatus(const char* filename, string robotID, 
                       string newStatus, string orderID)
{
    //6.1 open the CSV file for reading
    ifstream fileIn(filename);
    if (!fileIn.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    //6.2 create an array to hold all lines
    string lines[50];
    int lineCount = 0;
    string line;
    
    //6.3 read header line
    getline(fileIn, line);
    lines[lineCount] = line;
    lineCount++;
    
    //6.4 read and update matching robot
    while (getline(fileIn, line) && lineCount < 50)
    {
        //6.4.1 parse the line
        stringstream ss(line);
        string rid, name, status, currentOrderID, tasksStr;
        
        getline(ss, rid, ',');
        getline(ss, name, ',');
        getline(ss, status, ',');
        getline(ss, currentOrderID, ',');
        getline(ss, tasksStr, ',');
        
        //6.4.2 check if this is the robot to update
        if (trim(rid) == robotID)
        {
            //6.4.2.1 convert tasks string to integer
            int tasks = 0;
            if (!tasksStr.empty())
            {
                tasks = stoi(trim(tasksStr));
            }
            
            //6.4.2.2 increment tasks if completing a task
            if (newStatus == "AVAILABLE" && orderID.empty())
            {
                tasks++;
            }
            
            //6.4.2.3 set new order ID (empty if not provided)
            string newOrderID = "";
            if (!orderID.empty())
            {
                newOrderID = orderID;
            }
            
            //6.4.2.4 rebuild the line with new data
            lines[lineCount] = trim(rid) + "," + trim(name) + "," + 
                              newStatus + "," + newOrderID + "," + to_string(tasks);
        }
        else
        {
            //6.4.2.5 keep the line unchanged
            lines[lineCount] = line;
        }
        
        //6.4.3 increment line count
        lineCount++;
    }
    
    //6.5 close input file
    fileIn.close();
    
    //6.6 open the file for writing
    ofstream fileOut(filename);
    
    //6.7 write all lines back to file
    for (int i = 0; i < lineCount; i++)
    {
        fileOut << lines[i] << endl;
    }
    
    //6.8 close output file
    fileOut.close();
}

//Step 7: append a new order to CSV file
// this adds a new order to the end of the orders CSV file. i open
// the file in append mode (ios::app) which positions the write
// pointer at the end of the file. i then write the order fields
// separated by commas. this is used when a new order arrives and
// needs to be added to the system
void appendOrder(const char* filename, Order order)
{
    //7.1 open the file in append mode
    ofstream file(filename, ios::app);
    if (!file.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    //7.2 write the order fields separated by commas
    file << order.orderID << "," << order.itemID << "," << order.customerName << ","
         << order.priority << "," << order.status << "," << order.timestamp << endl;
    
    //7.3 close the file
    file.close();
}

//Step 8: append movement log entries to CSV file
// this adds multiple movement steps to the movement log CSV file.
// each step is a separate line with a unique log ID, robot ID, order ID,
// step number, step description, timestamp, and phase (FORWARD or RETURN).
// i first read the file to find the highest existing log ID, then
// append new entries with incrementing log IDs. this creates a complete
// audit trail of all robot movements
void appendMovementLog(const char* filename, string robotID, string orderID,
                       string steps[], int stepCount, string phase)
{
    //8.1 read existing log to get next log ID
    ifstream fileIn(filename);
    int lastLogID = 0;
    
    if (fileIn.is_open())
    {
        //8.1.1 skip header line
        string line;
        getline(fileIn, line);
        
        //8.1.2 read all lines and find highest log ID
        while (getline(fileIn, line))
        {
            stringstream ss(line);
            string logIDStr;
            getline(ss, logIDStr, ',');
            int logID = stoi(trim(logIDStr));
            
            if (logID > lastLogID)
            {
                lastLogID = logID;
            }
        }
        
        //8.1.3 close input file
        fileIn.close();
    }
    
    //8.2 open the file in append mode
    ofstream fileOut(filename, ios::app);
    if (!fileOut.is_open())
    {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    //8.3 append new entries for each step
    for (int i = 0; i < stepCount; i++)
    {
        //8.3.1 increment log ID
        lastLogID++;
        
        //8.3.2 write the log entry
        fileOut << lastLogID << "," << robotID << "," << orderID << ","
                << (i + 1) << "," << steps[i] << ",2026-05-21 10:00," << phase << endl;
    }
    
    //8.4 close output file
    fileOut.close();
}

void updateItemInCSV(const char* filename, string targetItemID, int newQty, string newZone, string newAisle, string newShelf)
{
    ifstream fileIn(filename);
    if (!fileIn.is_open()) 
    {
        cout << "Error: Cannot open " << filename << " for reading." << endl;
        return;
    }

    // Создаем временный файл
    ofstream fileOut("temp_items.csv");
    if (!fileOut.is_open()) 
    {
        cout << "Error: Cannot create temporary file." << endl;
        fileIn.close();
        return;
    }

    string line;
    // Копируем строку заголовков (ItemID,Name,ZoneID,AisleID,ShelfID,Quantity,Weight_kg)
    if (getline(fileIn, line)) 
    {
        fileOut << line << "\n";
    }

    bool found = false;

    // Читаем исходный CSV построчно
    while (getline(fileIn, line)) 
    {
        stringstream ss(line);
        string itemID, name, zoneID, aisleID, shelfID, qtyStr, weightStr;

        getline(ss, itemID, ',');
        getline(ss, name, ',');
        getline(ss, zoneID, ',');
        getline(ss, aisleID, ',');
        getline(ss, shelfID, ',');
        getline(ss, qtyStr, ',');
        getline(ss, weightStr, ',');

        // Сравниваем ID (убирая лишние пробелы через trim)
        if (trim(itemID) == trim(targetItemID)) 
        {
            // Записываем обновленные поля, сохраняя оригинальное имя и вес товара
            fileOut << trim(itemID) << ","
                    << trim(name) << ","
                    << trim(newZone) << ","
                    << trim(newAisle) << ","
                    << trim(newShelf) << ","
                    << newQty << ","
                    << trim(weightStr) << "\n";
            found = true;
        } 
        else 
        {
            // Если ID не совпал, просто переносим оригинальную строку
            fileOut << line << "\n";
        }
    }

    fileIn.close();
    fileOut.close();

    if (found) 
    {
        // Удаляем старый файл и переименовываем временный в оригинальное имя
        remove(filename);
        rename("temp_items.csv", filename);
    } 
    else 
    {
        // Если товар не нашли в CSV, убираем за собой временный файл
        remove("temp_items.csv");
        cout << "Warning: Item to update from CSV file not found." << endl;
    }
}