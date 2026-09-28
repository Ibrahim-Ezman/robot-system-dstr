#ifndef ROBOTCIRCULARQUEUE_HPP
#define ROBOTCIRCULARQUEUE_HPP

#include "Robot.hpp"
#include <iostream>

const int MAX_ROBOTS = 20;

//step 1.0 define the circular queue class for robot assignment
// i use a circular queue instead of a regular queue because robots
// need to be assigned in round-robin fashion. when a robot finishes
// a task, it goes back to the pool and the next robot in line gets
// the next task. with circular queue, when i reach the end of the
// array i wrap around to the beginning automatically using modulo.
// if i used a regular queue, i would need to dequeue and enqueue
// robots constantly which wastes time. with circular queue i just
// move the currentIndex forward and it wraps around naturally.
// i use array based queue instead of linked list because the number
// of robots is fixed (at most 20) and known in advance. array is
// simpler and faster than linked list for fixed size data
class RobotCircularQueue
{
private:
    Robot robots[MAX_ROBOTS];
    int capacity;
    int count;
    int currentIndex;

public:
    RobotCircularQueue();
    
    //step 1.1 core operations
    void loadRobots(Robot robotArray[], int robotCount);
    Robot* assignNext();
    void setStatus(string robotID, string status, string orderID);
    void displayAll();
    bool allBusy();
    int getCount();
    
    //step 1.2 helper function to find a robot by ID
    Robot* findRobot(string robotID);
};

#endif
