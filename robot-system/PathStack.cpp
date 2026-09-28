// Сначала подключаем системный заголовок Windows
#include <windows.h> 

// И только потом всё остальное стандартное и твои .hpp
#include <iostream>
#include <string>
#include "PathStack.hpp"

using namespace std;




//Step 3: constructor 
// i set top to NULL because the stack starts with no steps in it
// this is the same idea as setting head = NULL in linked list
// size starts at 0 because no steps. i also initialize the position
// tracking variables to empty strings because the robot starts at base
// (not in any zone). all statistics counters are set to 0 manually
// following the coding style rule of no initializer lists


PathStack::PathStack()
{
    top = NULL;
    size = 0;
    
    //3.1 initialize position tracking
    currentZone = "";
    currentAisle = "";
    currentShelf = "";
    
    //3.2 initialize all statistics fields to 0
    stats.forwardSteps = 0;
    stats.returnSteps = 0;
    stats.turns = 0;
    stats.straightMoves = 0;
    stats.zoneTransitions = 0;
    stats.aisleTransitions = 0;
    stats.shelfTransitions = 0;
    stats.longestStraightRun = 0;
    stats.currentStraightRun = 0;
}

//Step 4: destructors
// without this, every PathNode created with new would stay in memory
// even after the program finishes using the stack.


PathStack::~PathStack()
{
    clearStack();
}

//Step 5: push a new step onto the top of the stack
// this is how the robot records each movement. every time the robot
// moves (forward, left, right, enter zone, etc), we push that step
// onto the stack. the new step becomes the top of the stack.
// i link the new node to the current top first, then move top to
// the new node. this is the same pattern as InsertToFrontList in
// our lecture slides - inserting at the front of a linked list.
// i chose to insert at front (not end) because stack push should
// be O(1). if i inserted at end, i would need to walk through the
// entire list every time which would be O(n) and much slower.
// after pushing, i update the robot position, statistics, and draw
// the map so the user can see the robot moving in real time

void PathStack::push(string step)
{
    //5.1 create a new node and fill it with the step data
    PathNode* newnode = new PathNode;
    newnode->step = step;
    newnode->nextAddress = NULL;

    //5.2 link the new node to the current top
    // if the stack is empty, top is NULL so newnode->nextAddress
    // will be NULL which is correct - first node has nothing below it
    newnode->nextAddress = top;

    //5.3 move top pointer to the new node
    top = newnode;

    //5.4 increase size
    size++;
    
    //5.5 update robot position based on this step
    parseStepInfo(step);
    
    //5.6 update statistics for forward phase
    updateStats(step, "FORWARD");
    
    //5.7 draw the warehouse map with robot position
    drawMap(currentZone, currentAisle, currentShelf, step, "FORWARD", size, size);
}

//Step 6: pop the top step from the stack and return it
// this removes and returns the most recent step. because stack is
// LIFO, the last step the robot took is the first one we get back.
// this is exactly what we need for the return path - if the robot's
// last move was FORWARD, that is the first move we need to reverse.
// i save the step string before deleting the node because after
// delete the memory is freed and accessing it would be undefined behavior
string PathStack::pop()
{
    //6.1 check if the stack is empty first
    // if the robot already returned to base, there are no more steps
    if (isEmpty())
    {
        cout << "error: Stack is empty - robot at base!" << endl;
        return "";
    }

    //6.2 save the top node in a temp pointer so we can delete it later
    PathNode* temp = top;

    //6.3 save the step string before we delete the node
    string step = top->step;

    //6.4 move top down to the next node
    top = top->nextAddress;

    //6.5 delete the old top node to free memory
    delete temp;

    //6.6 decrease size 
    size--;

    //6.7 return the step we saved
    return step;
}

//Step 7: peek at the top step without removing it
// sometimes we need to see what the last step was without actually
// popping it. for example to check the current position of the robot
// before deciding the next move. peek returns the data but does not
// change the stack at all
string PathStack::peek()
{
    //7.1 check if the stack is empty
    if (isEmpty())
    {
        cout << "error: Stack is empty!" << endl;
        return "";
    }

    //7.2 return the step stored in the top node
    return top->step;
}

//Step 8: check if the stack is empty
// i check top == NULL instead of size == 0. both would work but
// checking the pointer is more direct - if top points to nothing
// then there are no nodes in the stack. this is the same approach
// as checking head == NULL in a linked list
bool PathStack::isEmpty()
{
    return top == NULL;
}

//Step 9: get the current number of steps in the stack
int PathStack::getSize()
{
    return size;
}

//Step 10: display the full forward path from start to destination
// the problem is that the stack stores steps in reverse order.
// the first step the robot took is at the bottom and the last step
// is at the top. if i just print from top to bottom i would see
// the path backwards. so i use a dynamic array to collect all steps
// and then print them in the correct order from index 0 to size-1.
// i use dynamic array (new string[size]) instead of fixed array
// because i do not know how many steps the robot will take.
// if i used a fixed array like string steps[100] and the path had
// more than 100 steps it would overflow
void PathStack::displayForwardPath()
{
    //10.1 check if the path is empty
    if (isEmpty())
    {
        cout << "Path is empty! Destination was start" << endl;
        return;
    }

    //10.2 create a dynamic array to hold steps in correct order
    string* steps = new string[size];

    //10.3 walk through the stack from top to bottom
    // top has the last step so i put it at the end of the array
    // bottom has the first step so it goes at index 0
    PathNode* current = top;
    int index = size - 1;

    while (current != NULL)
    {
        steps[index] = current->step;
        current = current->nextAddress;
        index--;
    }

    //10.4 print the steps in forward order
    cout << "\n ---FORWARD PATH---" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << "Step " << (i + 1) << ": " << steps[i] << endl;
    }
    cout << "[DESTINATION REACHED]" << endl;

    //10.5 free the dynamic array to avoid memory leak
    delete[] steps;
}

//Step 11: pop all steps and display the return path with reversed directions
// this is the main feature of the path tracking module. the robot
// finished its task and needs to go back to the starting point.
// because the stack is LIFO, popping gives us the steps in reverse
// order automatically. for each step, we also reverse the direction
// itself - FORWARD becomes BACKWARD, LEFT becomes RIGHT, etc.
// this way the robot follows the exact same route but in reverse.
// now with visualization, we also update position and draw the map
// after each step so the user can see the robot returning in real time
void PathStack::reverseReturn()
{
    //11.1 check if the stack is empty
    if (isEmpty())
    {
        cout << "Robot at base! No return path needed" << endl;
        return;
    }

    //11.2 save total steps for statistics
    int totalSteps = size;

    //11.3 display header for return path
    cout << "\n ---RETURN PATH (REVERSE)---" << endl;
    int stepNum = 1;

    //11.4 pop each step and print its reversed version
    // i pop until the stack is empty which means the robot has
    // retraced every step it took on the forward journey
    while (!isEmpty())
    {
        //11.4.1 pop the original step from the stack
        string originalStep = pop();

        //11.4.2 get the reversed direction for this step
        string reversedStep = reverseDirection(originalStep);

        //11.4.3 update robot position based on reversed step
        parseStepInfo(reversedStep);
        
        //11.4.4 update statistics for return phase
        updateStats(reversedStep, "RETURN");

        //11.4.5 display both original and reversed for the navigation log
        cout << "Step " << stepNum << ": " << reversedStep
             << " (was: " << originalStep << ")" << endl;
        
        //11.4.6 draw the warehouse map with robot position
        drawMap(currentZone, currentAisle, currentShelf, reversedStep, 
                "RETURN", stepNum, totalSteps);
        
        stepNum++;
    }

    //11.5 confirm robot has returned
    cout << "[ROBOT RETURNED TO BASE]" << endl;
    
    //11.6 display statistics report
    displayStats();
}

//Step 12: clear all nodes from the stack
// this is used by the destructor and can also be called manually
// if we need to reset the path for a new task. when clearing,
// we also reset all statistics so the next journey starts fresh
void PathStack::clearStack()
{
    while (!isEmpty())
    {
        pop();
    }
    
    //12.1 reset statistics for next journey
    resetStats();
}

//Step 13: reverse a direction string so the robot can go back
// i use simple if-else matching for basic directions (FORWARD,
// BACKWARD, LEFT, RIGHT) because there are only 4 possibilities.
// for zone, aisle and shelf movements i use string find() and substr()
// because the direction includes a variable part like ENTER_ZONE_A
// or MOVE_TO_AISLE_2. find() checks if the string starts with a
// known prefix. substr() extracts the variable part (like _A or _2)
// so i can attach it to the reversed prefix.
// for example ENTER_ZONE_A: find("ENTER_ZONE") finds it at position 0,
// substr(10) extracts "_A", then i combine "EXIT_ZONE" + "_A" = "EXIT_ZONE_A"
// if i did not use find/substr, i would need a separate if for every
// possible zone, aisle and shelf combination which is not scalable
string PathStack::reverseDirection(string direction)
{
    //13.1 reverse basic movement directions
    if (direction == "FORWARD") return "BACKWARD";
    if (direction == "BACKWARD") return "FORWARD";
    if (direction == "LEFT") return "RIGHT";
    if (direction == "RIGHT") return "LEFT";

    //13.2 reverse zone entry and exit
    // ENTER_ZONE has 10 characters, so substr(10) gets everything after it
    if (direction.find("ENTER_ZONE") != string::npos)
    {
        return "EXIT_ZONE" + direction.substr(10);
    }
    // EXIT_ZONE has 9 characters
    if (direction.find("EXIT_ZONE") != string::npos)
    {
        return "ENTER_ZONE" + direction.substr(9);
    }

    //13.3 reverse aisle movement
    // MOVE_TO_AISLE has 13 characters
    if (direction.find("MOVE_TO_AISLE") != string::npos)
    {
        return "RETURN_FROM_AISLE" + direction.substr(13);
    }
    // RETURN_FROM_AISLE has 17 characters
    if (direction.find("RETURN_FROM_AISLE") != string::npos)
    {
        return "MOVE_TO_AISLE" + direction.substr(17);
    }

    //13.4 reverse shelf movement
    // MOVE_TO_SHELF has 13 characters
    if (direction.find("MOVE_TO_SHELF") != string::npos)
    {
        return "RETURN_FROM_SHELF" + direction.substr(13);
    }
    // RETURN_FROM_SHELF has 17 characters
    if (direction.find("RETURN_FROM_SHELF") != string::npos)
    {
        return "MOVE_TO_SHELF" + direction.substr(17);
    }

    //13.5 if no match found, return the direction as it is
    // this handles any future direction types that are not yet defined
    return direction;
}

//Step 14: parse step information and update robot position
// this function reads a direction string and updates the currentZone,
// currentAisle, and currentShelf variables to track where the robot is.
// i use string find() to check if the direction contains specific keywords
// like ENTER_ZONE or MOVE_TO_AISLE. then i extract the zone/aisle/shelf
// identifier using substr(). this is more flexible than hardcoding every
// possible combination. for example, ENTER_ZONE_A sets currentZone to "A",
// MOVE_TO_AISLE_2 sets currentAisle to "2", etc. when the robot exits
// a zone or returns from an aisle, i clear the corresponding variable
void PathStack::parseStepInfo(string direction)
{
    //14.1 check for zone entry
    // ENTER_ZONE_A means robot enters zone A
    if (direction.find("ENTER_ZONE_") != string::npos)
    {
        //14.1.1 extract zone letter (last character)
        currentZone = direction.substr(11, 1);
        currentAisle = "";
        currentShelf = "";
    }
    //14.2 check for zone exit
    // EXIT_ZONE_A means robot leaves zone A and goes back to base area
    else if (direction.find("EXIT_ZONE_") != string::npos)
    {
        currentZone = "";
        currentAisle = "";
        currentShelf = "";
    }
    //14.3 check for aisle movement
    // MOVE_TO_AISLE_2 means robot moves to aisle 2 within current zone
    else if (direction.find("MOVE_TO_AISLE_") != string::npos)
    {
        //14.3.1 extract aisle number (last character)
        currentAisle = direction.substr(14, 1);
        currentShelf = "";
    }
    //14.4 check for aisle return
    // RETURN_FROM_AISLE_2 means robot leaves aisle 2
    else if (direction.find("RETURN_FROM_AISLE_") != string::npos)
    {
        currentAisle = "";
        currentShelf = "";
    }
    //14.5 check for shelf movement
    // MOVE_TO_SHELF_1 means robot reaches shelf 1 within current aisle
    else if (direction.find("MOVE_TO_SHELF_") != string::npos)
    {
        //14.5.1 extract shelf number (last character)
        currentShelf = direction.substr(14, 1);
    }
    //14.6 check for shelf return
    // RETURN_FROM_SHELF_1 means robot leaves shelf 1
    else if (direction.find("RETURN_FROM_SHELF_") != string::npos)
    {
        currentShelf = "";
    }
    //14.7 for FORWARD/BACKWARD/LEFT/RIGHT, position does not change
    // these are generic movements that do not affect zone/aisle/shelf
}

//Step 15: update statistics based on direction and phase
// this function counts different types of movements to generate
// statistics about the robot's journey. i track turns (LEFT/RIGHT),
// straight moves (FORWARD/BACKWARD), and transitions between zones,
// aisles, and shelves. i also track the longest consecutive run of
// straight moves to measure path efficiency. if the robot makes many
// turns, the path is less efficient than a path with long straight runs
void PathStack::updateStats(string direction, string phase)
{
    //15.1 update forward phase statistics
    if (phase == "FORWARD")
    {
        //15.1.1 increment forward step counter
        stats.forwardSteps++;
        
        //15.1.2 check for turns
        // turns break the straight run so we reset the counter
        if (direction == "LEFT" || direction == "RIGHT")
        {
            stats.turns++;
            stats.currentStraightRun = 0;
        }
        //15.1.3 check for straight moves
        // straight moves increase the current run and update longest run
        else if (direction == "FORWARD" || direction == "BACKWARD")
        {
            stats.straightMoves++;
            stats.currentStraightRun++;
            
            //15.1.3.1 update longest run if current run is longer
            if (stats.currentStraightRun > stats.longestStraightRun)
            {
                stats.longestStraightRun = stats.currentStraightRun;
            }
        }
        //15.1.4 check for zone transitions
        // entering or exiting a zone counts as a zone transition
        else if (direction.find("ENTER_ZONE") != string::npos 
              || direction.find("EXIT_ZONE") != string::npos)
        {
            stats.zoneTransitions++;
        }
        //15.1.5 check for aisle transitions
        // moving to or returning from an aisle counts as aisle transition
        else if (direction.find("MOVE_TO_AISLE") != string::npos 
              || direction.find("RETURN_FROM_AISLE") != string::npos)
        {
            stats.aisleTransitions++;
        }
        //15.1.6 check for shelf transitions
        // moving to or returning from a shelf counts as shelf transition
        else if (direction.find("MOVE_TO_SHELF") != string::npos 
              || direction.find("RETURN_FROM_SHELF") != string::npos)
        {
            stats.shelfTransitions++;
        }
    }
    //15.2 update return phase statistics
    // during return, we only count the number of return steps
    else
    {
        stats.returnSteps++;
    }
}

//Step 16: reset all statistics counters to 0
// this is called when clearing the stack to prepare for a new journey.
// all counters must be reset so the next path starts with clean statistics.
// i reset each field individually following the coding style rule of
// no initializer lists or bulk assignments
void PathStack::resetStats()
{
    stats.forwardSteps = 0;
    stats.returnSteps = 0;
    stats.turns = 0;
    stats.straightMoves = 0;
    stats.zoneTransitions = 0;
    stats.aisleTransitions = 0;
    stats.shelfTransitions = 0;
    stats.longestStraightRun = 0;
    stats.currentStraightRun = 0;
}

//Step 17: draw the warehouse map with robot position
// this function creates an ASCII art visualization of the warehouse
// showing the robot's current position. the warehouse has 3 zones
// (A, B, C) arranged horizontally. each zone can show the current
// aisle and shelf the robot is in. the robot is marked with [R].
// i clear the screen before each draw so the map appears to animate
// as the robot moves. i use ANSI escape codes for screen clearing
// which work on Linux, Mac, and modern Windows terminals. after
// drawing, i pause for 800ms so the user can see each step
void PathStack::drawMap(string zone, string aisle, string shelf, 
                        string direction, string phase, int stepNum, int totalSteps)
{
    //17.1 clear the terminal screen
    // ANSI escape code \033[2J clears screen, \033[H moves cursor to top
    cout << "\033[2J\033[H";
    
    //17.2 print header
    cout << "============================================" << endl;
    cout << "   WAREHOUSE ROBOT NAVIGATION SYSTEM" << endl;
    cout << "============================================" << endl;
    cout << endl;
    
    //17.3 determine robot position for each zone
    // i check which zone the robot is in and mark it with [R]
    string zoneA_line1 = "           ";
    string zoneA_line2 = "           ";
    string zoneA_line3 = "           ";
    
    string zoneB_line1 = "           ";
    string zoneB_line2 = "           ";
    string zoneB_line3 = "           ";
    
    string zoneC_line1 = "           ";
    string zoneC_line2 = "           ";
    string zoneC_line3 = "           ";
    
    string baseMarker = "[*]";
    
    //17.4 place robot in correct zone
    if (zone == "A")
    {
        //17.4.1 robot is in zone A
        if (aisle == "")
        {
            //17.4.1.1 robot just entered zone, no specific aisle yet
            zoneA_line3 = "  [R]      ";
        }
        else
        {
            //17.4.1.2 robot is in a specific aisle
            zoneA_line1 = " Aisle " + aisle + "  ";
            
            if (shelf == "")
            {
                //17.4.1.2.1 robot in aisle but no shelf yet
                zoneA_line3 = "  [R]      ";
            }
            else
            {
                //17.4.1.2.2 robot reached a shelf
                zoneA_line2 = " Shelf " + shelf + "  ";
                zoneA_line3 = "  [R] ***  ";
            }
        }
    }
    else if (zone == "B")
    {
        //17.4.2 robot is in zone B
        if (aisle == "")
        {
            zoneB_line3 = "  [R]      ";
        }
        else
        {
            zoneB_line1 = " Aisle " + aisle + "  ";
            
            if (shelf == "")
            {
                zoneB_line3 = "  [R]      ";
            }
            else
            {
                zoneB_line2 = " Shelf " + shelf + "  ";
                zoneB_line3 = "  [R] ***  ";
            }
        }
    }
    else if (zone == "C")
    {
        //17.4.3 robot is in zone C
        if (aisle == "")
        {
            zoneC_line3 = "  [R]      ";
        }
        else
        {
            zoneC_line1 = " Aisle " + aisle + "  ";
            
            if (shelf == "")
            {
                zoneC_line3 = "  [R]      ";
            }
            else
            {
                zoneC_line2 = " Shelf " + shelf + "  ";
                zoneC_line3 = "  [R] ***  ";
            }
        }
    }
    else
    {
        //17.4.4 robot is at base (no zone)
        baseMarker = "[R*]";
    }
    
    //17.5 draw the warehouse grid
    cout << "  +===========+===========+===========+" << endl;
    cout << "  |  ZONE  A  |  ZONE  B  |  ZONE  C  |" << endl;
    cout << "  |" << zoneA_line1 << "|" << zoneB_line1 << "|" << zoneC_line1 << "|" << endl;
    cout << "  |" << zoneA_line2 << "|" << zoneB_line2 << "|" << zoneC_line2 << "|" << endl;
    cout << "  |" << zoneA_line3 << "|" << zoneB_line3 << "|" << zoneC_line3 << "|" << endl;
    cout << "  +===========+===========+===========+" << endl;
    cout << "  BASE: " << baseMarker << endl;
    cout << endl;
    
    //17.6 print step information
    cout << "  Step " << stepNum << "/" << totalSteps << " | Direction: " << direction << endl;
    cout << "  Phase: " << phase << "  | Steps in stack: " << size << endl;
    
    //17.7 print special markers
    if (shelf != "" && phase == "FORWARD")
    {
        cout << "  *** ITEM LOCATION REACHED ***" << endl;
    }
    else if (zone == "" && phase == "RETURN")
    {
        cout << "  *** ROBOT RETURNED TO BASE SUCCESSFULLY ***" << endl;
    }
    else if (phase == "RETURN")
    {
        cout << "  Reversing: " << reverseDirection(direction) << "  ->  " << direction << endl;
    }
    
    cout << "--------------------------------------------" << endl;
    
    //17.8 pause for 800ms so user can see the step
    // this creates an animation effect as the robot moves
    #ifdef _WIN32
        Sleep(800);
    #else
        usleep(800000);
    #endif
}

//Step 18: display statistics report after journey completion
// this function prints a detailed report of the robot's journey
// showing efficiency metrics, transition counts, and other statistics.
// efficiency is calculated as the percentage of straight moves out of
// total moves - higher efficiency means fewer turns and a more direct path.
// this helps evaluate the quality of the path planning algorithm
void PathStack::displayStats()
{
    //18.1 calculate total steps and efficiency
    int totalSteps = stats.forwardSteps + stats.returnSteps;
    int efficiency = 0;
    
    if (totalSteps > 0)
    {
        efficiency = (stats.straightMoves * 100) / totalSteps;
    }
    
    //18.2 print header
    cout << endl;
    cout << "============================================" << endl;
    cout << "         PATH STATISTICS REPORT            " << endl;
    cout << "============================================" << endl;
    cout << "  Robot journey completed." << endl;
    cout << endl;
    
    //18.3 print step counts
    cout << "  Forward path steps    : " << stats.forwardSteps << endl;
    cout << "  Return path steps     : " << stats.returnSteps << endl;
    cout << "  Total steps taken     : " << totalSteps << endl;
    cout << endl;
    
    //18.4 print movement analysis
    cout << "  Turns made (L + R)    : " << stats.turns << endl;
    cout << "  Straight moves (F + B): " << stats.straightMoves << endl;
    cout << "  Zone transitions      : " << stats.zoneTransitions << endl;
    cout << "  Aisle transitions     : " << stats.aisleTransitions << endl;
    cout << "  Shelf transitions     : " << stats.shelfTransitions << endl;
    cout << endl;
    
    //18.5 print efficiency metrics
    cout << "  Efficiency score      : " << efficiency << "%" << endl;
    cout << "  (straight steps / total steps * 100)" << endl;
    cout << endl;
    cout << "  Longest straight run  : " << stats.longestStraightRun 
         << " consecutive FORWARD steps" << endl;
    cout << "============================================" << endl;
}