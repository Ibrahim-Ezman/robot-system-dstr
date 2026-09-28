#ifndef ORDERQUEUE_HPP
#define ORDERQUEUE_HPP

#include "Order.hpp"
#include <iostream>

//step 1.0 define the node structure for the order queue
// i chose a linked list based queue instead of array based queue
// because with array i would need to set a fixed size like arr[100]
// and if more than 100 orders arrive the queue would overflow
// with linked list every enqueue just creates one new node on the heap
// so the queue can grow to any size needed
// each node holds one order and a pointer to the next node in the queue
struct OrderNode
{
    Order data;
    OrderNode* nextAddress;
};

//step 2.0 define the queue class for order management
// i use queue because orders must be processed in FIFO order
// (first in first out). the oldest order should be processed first
// which is fair to customers. if i used a stack instead, the newest
// order would be processed first (LIFO) which would be unfair.
// the queue has front and rear pointers - front points to the oldest
// order, rear points to the newest order. this is the same pattern
// as the queue implementation in our lecture slides
class OrderQueue
{
private:
    OrderNode* front;
    OrderNode* rear;
    int size;

public:
    OrderQueue();
    ~OrderQueue();
    
    //step 2.1 core queue operations
    void enqueue(Order order);
    Order dequeue();
    Order peek();
    bool isEmpty();
    int getSize();
    void displayAll();
    
    //step 2.2 helper function to check for duplicate orders
    bool orderExists(string orderID);
};

#endif
