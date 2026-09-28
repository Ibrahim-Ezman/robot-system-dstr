#ifndef ROBOT_HPP
#define ROBOT_HPP

#include <string>
using namespace std;

//step 1.0 define the robot structure
// i use a simple struct with only data fields and no constructor
// because robot data comes from CSV file and is set manually
// after creating the struct. each robot has an ID, name, status
// (AVAILABLE, BUSY, MAINTENANCE), current order assignment, and
// a counter for total tasks completed. with plain struct i can
// create an empty robot and fill fields one by one which is clearer
// than passing 5 parameters to a constructor
struct Robot
{
    string robotID;
    string name;
    string status;
    string currentOrderID;
    int totalTasksDone;
};

#endif
