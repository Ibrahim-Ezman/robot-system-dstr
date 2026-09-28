#include "RobotCircularQueue.hpp"

//Step 2: constructor
// i set capacity to MAX_ROBOTS which is 20 because the warehouse
// can have at most 20 robots. count starts at 0 because no robots
// are loaded yet. currentIndex starts at 0 because when robots are
// loaded, the first robot to be assigned will be at index 0
RobotCircularQueue::RobotCircularQueue()
{
    capacity = MAX_ROBOTS;
    count = 0;
    currentIndex = 0;
}

//Step 3: load robots from an array into the circular queue
// this is called once at the start of the program after reading
// robots from CSV file. i copy each robot from the input array
// into the internal robots array. if the input has more than
// MAX_ROBOTS robots, i only take the first MAX_ROBOTS to avoid
// array overflow. this is the initialization step that fills the
// circular queue with all available robots
void RobotCircularQueue::loadRobots(Robot robotArray[], int robotCount)
{
    //3.1 set count to the smaller of robotCount and MAX_ROBOTS
    // this prevents array overflow if too many robots are provided
    if (robotCount > MAX_ROBOTS)
    {
        count = MAX_ROBOTS;
    }
    else
    {
        count = robotCount;
    }
    
    //3.2 copy each robot from input array to internal array
    for (int i = 0; i < count; i++)
    {
        robots[i] = robotArray[i];
    }
    
    //3.3 reset currentIndex to 0 to start from the beginning
    currentIndex = 0;
}

//Step 4: assign the next available robot using circular rotation
// this is the core function of the circular queue. it finds the next
// robot with status AVAILABLE and assigns it to a task by changing
// its status to BUSY. the circular rotation means that after checking
// robot at index i, i check robot at index (i+1) % count which wraps
// around to 0 when i reaches the end. this ensures fair distribution
// of tasks - every robot gets assigned in turn. if i used a regular
// queue, the first robot would always get assigned first which is
// unfair. with circular queue, the robot that finished its last task
// longest ago gets the next task. this is the same pattern as the
// circular queue implementation in our lecture slides
Robot* RobotCircularQueue::assignNext()
{
    //4.1 check if there are any robots in the system
    if (count == 0)
    {
        cout << "Error: No robots in the system." << endl;
        return NULL;
    }
    
    //4.2 check if all robots are busy or in maintenance
    if (allBusy())
    {
        cout << "Warning: All robots are currently BUSY or under MAINTENANCE." << endl;
        return NULL;
    }
    
    //4.3 rotate through queue to find next AVAILABLE robot
    // i try at most count times to avoid infinite loop
    int attempts = 0;
    while (attempts < count)
    {
        //4.3.1 get pointer to robot at current index
        Robot* current = &robots[currentIndex];
        
        //4.3.2 move to next position using circular wrap-around
        // modulo ensures that when currentIndex reaches count-1,
        // the next index wraps around to 0
        currentIndex = (currentIndex + 1) % count;
        
        //4.3.3 check if this robot is available
        if (current->status == "AVAILABLE")
        {
            //4.3.3.1 mark robot as busy
            current->status = "BUSY";
            
            //4.3.3.2 return pointer to this robot
            return current;
        }
        
        //4.3.4 increment attempts counter
        attempts++;
    }
    
    //4.4 if we tried all robots and none are available, return NULL
    return NULL;
}

//Step 5: update a robot's status and current order assignment
// this is called when a robot starts a task (status becomes BUSY)
// or finishes a task (status becomes AVAILABLE). when a robot
// finishes a task, i also increment its totalTasksDone counter
// to track productivity. i find the robot by ID first, then update
// its fields. if the robot is not found, nothing happens
void RobotCircularQueue::setStatus(string robotID, string status, string orderID)
{
    //5.1 find the robot with the given ID
    Robot* robot = findRobot(robotID);
    
    //5.2 check if robot was found
    if (robot != NULL)
    {
        //5.2.1 update status
        robot->status = status;
        
        //5.2.2 update current order assignment
        robot->currentOrderID = orderID;
        
        //5.2.3 if robot finished a task, increment task counter
        // a task is finished when status becomes AVAILABLE and
        // currentOrderID is empty (no longer assigned to an order)
        if (status == "AVAILABLE" && robot->currentOrderID.empty())
        {
            robot->totalTasksDone++;
        }
    }
}

//Step 6: display all robots and their current status
// i walk through the robots array from index 0 to count-1 and
// print each robot's details. i also mark which robot is at
// currentIndex because that is the next robot that will be
// checked for assignment. this helps the user see the state
// of the circular queue and understand the rotation pattern
void RobotCircularQueue::displayAll()
{
    //6.1 check if there are any robots
    if (count == 0)
    {
        cout << "No robots in the system." << endl;
        return;
    }
    
    //6.2 print header with total count and current index
    cout << "\n=== ROBOT STATUS (CIRCULAR QUEUE) ===" << endl;
    cout << "Total robots: " << count << " | Current index: " << currentIndex << endl;
    cout << "------------------------------------------------------------" << endl;
    
    //6.3 walk through all robots and print their details
    for (int i = 0; i < count; i++)
    {
        //6.3.1 print index number and mark the next robot
        cout << "[" << i << "] ";
        if (i == currentIndex)
        {
            cout << "[NEXT] ";
        }
        
        //6.3.2 print robot ID, name, and status
        cout << robots[i].robotID << " (" << robots[i].name << ") - "
             << "Status: " << robots[i].status;
        
        //6.3.3 if robot is handling an order, print the order ID
        if (!robots[i].currentOrderID.empty())
        {
            cout << " | Handling: " << robots[i].currentOrderID;
        }
        
        //6.3.4 print total tasks completed
        cout << " | Tasks Done: " << robots[i].totalTasksDone << endl;
    }
    
    //6.4 print footer
    cout << "------------------------------------------------------------" << endl;
}

//Step 7: check if all robots are busy or in maintenance
// i walk through all robots and check their status. if i find
// even one robot with status AVAILABLE, i return false immediately.
// if i reach the end without finding any available robot, i return
// true. this is used before assigning a new order - if all robots
// are busy, the order must wait in the queue
bool RobotCircularQueue::allBusy()
{
    //7.1 walk through all robots
    for (int i = 0; i < count; i++)
    {
        //7.1.1 check if this robot is available
        if (robots[i].status == "AVAILABLE")
        {
            return false;
        }
    }
    
    //7.2 if we reached here, no robot is available
    return true;
}

//Step 8: get the current number of robots in the queue
int RobotCircularQueue::getCount()
{
    return count;
}

//Step 9: find a robot by its ID
// i walk through the robots array and compare each robot's ID
// with the given ID. if i find a match, i return a pointer to
// that robot. if i reach the end without finding a match, i
// return NULL. this is a linear search which is O(n) but necessary
// because the robots array is not sorted by ID. with at most 20
// robots, linear search is fast enough
Robot* RobotCircularQueue::findRobot(string robotID)
{
    //9.1 walk through all robots
    for (int i = 0; i < count; i++)
    {
        //9.1.1 check if this robot's ID matches
        if (robots[i].robotID == robotID)
        {
            //9.1.1.1 return pointer to this robot
            return &robots[i];
        }
    }
    
    //9.2 if we reached here, the robot was not found
    return NULL;
}
