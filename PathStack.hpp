#ifndef PATHSTACK_HPP
#define PATHSTACK_HPP




#include <string>
#include <iostream>



using namespace std;

//step 1.0 define the node structure for the stack
// i chose a linked list based stack instead of array based stack
// because with array i would need to set a fixed size like arr[100]
// and if the robot path is longer than 100 steps the stack would overflow
// with linked list every push just creates one new node on the heap
// so the stack can grow to any size the robot needs
// each node holds one movement step like FORWARD, LEFT, ENTER_ZONE_A
// and a pointer to the node below it in the stack


struct PathNode
{
    string step;
    PathNode* nextAddress;
};

//step 1.1 define the statistics structure for path analysis
// i use a separate struct to group all statistics fields together
// instead of having many individual variables scattered in the class.
// this makes the code more organized and easier to understand.
// no constructor is used because we follow the coding style rule
// that structs should only have data fields. all fields are initialized
// manually in the PathStack constructor
struct PathStats
{
    int forwardSteps;
    int returnSteps;
    int turns;
    int straightMoves;
    int zoneTransitions;
    int aisleTransitions;
    int shelfTransitions;
    int longestStraightRun;
    int currentStraightRun;
};

//step 2.0 define the stack class for robot path tracking
// i use stack because the robot needs to return using the
// reverse path LIFO, if the robot went FORWARD then LEFT then FORWARD
// the return should be BACKWARD then RIGHT then BACKWARD
// stack naturally gives this reverse order - the last step pushed
// is the first step popped. if i used a queue instead,
// the robot would try to return using the same forward order
// which would not bring it back to the starting point because of FIFO concept


class PathStack
{
private:
    PathNode* top;
    int size;
    
    //step 2.1 robot position tracking variables
    // these track where the robot currently is in the warehouse
    // so we can draw the map correctly. they are updated every time
    // the robot moves (in push) and when it returns (in reverseReturn)
    string currentZone;
    string currentAisle;
    string currentShelf;
    
    //step 2.2 statistics tracking
    // this struct holds all the statistics about the robot's journey
    // such as how many steps taken, how many turns made, efficiency, etc.
    PathStats stats;

    //step 2.3 helper function to reverse a direction string
    string reverseDirection(string direction);
    
    //step 2.4 visualization helper function
    // this draws the ASCII map of the warehouse with the robot position
    // marked as [R]. it is called after every step during both forward
    // and return phases so the user can see the robot moving in real time
    void drawMap(string currentZone, string currentAisle, 
                 string currentShelf, string direction, 
                 string phase, int stepNum, int totalSteps);
    
    //step 2.5 position parsing helper function
    // this reads a direction string like ENTER_ZONE_A or MOVE_TO_AISLE_2
    // and updates the currentZone, currentAisle, currentShelf variables
    // accordingly. it is called in both push and reverseReturn
    void parseStepInfo(string direction);
    
    //step 2.6 statistics update helper function
    // this updates the statistics counters based on the direction taken
    // and whether it is forward or return phase. it counts turns,
    // straight moves, zone transitions, etc.
    void updateStats(string direction, string phase);
    
    //step 2.7 statistics reset helper function
    // this resets all statistics counters to 0 when starting a new journey
    // it is called in clearStack so each new path starts with clean stats
    void resetStats();

public:
    PathStack();
    ~PathStack();

    //step 2.8 core stack operations
    void push(string step);
    string pop();
    string peek();
    bool isEmpty();
    int getSize();
    
    //step 2.9 path display and reversal functions
    void displayForwardPath();
    void reverseReturn();
    void clearStack();
    
    //step 2.10 statistics display function
    // this prints a detailed report of the robot's journey statistics
    // after the robot returns to base. it shows efficiency, turns,
    // transitions, and other metrics to analyze the path quality
    void displayStats();
};

#endif