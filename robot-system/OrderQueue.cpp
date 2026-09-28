#include "OrderQueue.hpp"

//Step 3: constructor
// i set front and rear to NULL because the queue starts empty
// this is the same idea as setting head = NULL in linked list
// size starts at 0 because no orders yet
OrderQueue::OrderQueue()
{
    front = NULL;
    rear = NULL;
    size = 0;
}

//Step 4: destructor
// without this, every OrderNode created with new would stay in memory
// even after the program finishes using the queue. i call dequeue
// repeatedly until the queue is empty which deletes all nodes one by one
OrderQueue::~OrderQueue()
{
    while (!isEmpty())
    {
        dequeue();
    }
}

//Step 5: add a new order to the end of the queue
// this is how orders are stored when they arrive. the queue works
// as FIFO (first in first out) so the oldest order is always processed
// first. i insert at the rear because that is where new items go in
// a queue. if i inserted at front instead, it would behave like a
// stack (LIFO) and orders would be processed newest-first which is
// unfair. i chose linked list based queue instead of array based queue
// because with array i need a fixed size like arr[100] and if more
// than 100 orders arrive the queue overflows. with linked list every
// enqueue just creates one new node so the queue grows to any size.
// this is the same pattern as InsertToEndOfList in our lecture slides
void OrderQueue::enqueue(Order order)
{
    //5.1 check if this order ID already exists in the queue
    // duplicate orders should be rejected to avoid processing the same
    // order twice which would waste robot time and inventory
    if (orderExists(order.orderID))
    {
        cout << "Error: Order " << order.orderID << " already exists in queue. Rejected." << endl;
        return;
    }
    
    //5.2 create a new node and fill it with order data
    OrderNode* newnode = new OrderNode;
    newnode->data = order;
    newnode->nextAddress = NULL;
    
    //5.3 check if queue is empty - if so, front and rear both point to new node
    if (isEmpty())
    {
        front = rear = newnode;
    }
    else
    {
        //5.4 link current rear to new node, then move rear forward
        rear->nextAddress = newnode;
        rear = newnode;
    }
    
    //5.5 increase size counter
    size++;
}

//Step 6: remove and return the front order from the queue
// this removes the oldest order which is at the front. because queue
// is FIFO, the first order that was added is the first one we remove.
// this is exactly what we need for fair order processing - customers
// who ordered first get their items first. i save the order data before
// deleting the node because after delete the memory is freed and
// accessing it would be undefined behavior
Order OrderQueue::dequeue()
{
    //6.1 check if the queue is empty first
    if (isEmpty())
    {
        cout << "Error: Queue is empty. Cannot dequeue." << endl;
        Order emptyOrder;
        emptyOrder.orderID = "";
        emptyOrder.itemID = "";
        emptyOrder.customerName = "";
        emptyOrder.priority = "";
        emptyOrder.status = "";
        emptyOrder.timestamp = "";
        return emptyOrder;
    }
    
    //6.2 save the front node in a temp pointer so we can delete it later
    OrderNode* temp = front;
    
    //6.3 save the order data before we delete the node
    Order order = front->data;
    
    //6.4 move front forward to the next node
    front = front->nextAddress;
    
    //6.5 if front is now NULL, the queue is empty so rear should also be NULL
    if (front == NULL)
    {
        rear = NULL;
    }
    
    //6.6 delete the old front node to free memory
    delete temp;
    
    //6.7 decrease size counter
    size--;
    
    //6.8 return the order we saved
    return order;
}

//Step 7: peek at the front order without removing it
// sometimes we need to see what the next order is without actually
// dequeuing it. for example to check the priority before deciding
// which robot to assign. peek returns the data but does not change
// the queue at all
Order OrderQueue::peek()
{
    //7.1 check if the queue is empty
    if (isEmpty())
    {
        cout << "Error: Queue is empty. Cannot peek." << endl;
        Order emptyOrder;
        emptyOrder.orderID = "";
        emptyOrder.itemID = "";
        emptyOrder.customerName = "";
        emptyOrder.priority = "";
        emptyOrder.status = "";
        emptyOrder.timestamp = "";
        return emptyOrder;
    }
    
    //7.2 return the order stored in the front node
    return front->data;
}

//Step 8: check if the queue is empty
// i check front == NULL instead of size == 0. both would work but
// checking the pointer is more direct - if front points to nothing
// then there are no nodes in the queue. this is the same approach
// as checking head == NULL in a linked list
bool OrderQueue::isEmpty()
{
    return front == NULL;
}

//Step 9: get the current number of orders in the queue
int OrderQueue::getSize()
{
    return size;
}

//Step 10: display all orders in the queue from front to rear
// i walk through the linked list from front to rear and print each
// order's details. this helps the user see what orders are waiting
// to be processed and in what order they will be handled
void OrderQueue::displayAll()
{
    //10.1 check if the queue is empty
    if (isEmpty())
    {
        cout << "Order queue is empty." << endl;
        return;
    }
    
    //10.2 print header with total count
    cout << "\n=== PENDING ORDERS QUEUE ===" << endl;
    cout << "Total orders: " << size << endl;
    cout << "------------------------------------------------------------" << endl;
    
    //10.3 walk through the queue from front to rear
    OrderNode* current = front;
    int position = 1;
    
    while (current != NULL)
    {
        //10.3.1 print position number and mark the front order
        cout << "[" << position << "] ";
        if (position == 1)
        {
            cout << "[FRONT] ";
        }
        
        //10.3.2 print order details
        cout << "Order: " << current->data.orderID 
             << " | Item: " << current->data.itemID
             << " | Customer: " << current->data.customerName
             << " | Priority: " << current->data.priority
             << " | Time: " << current->data.timestamp << endl;
        
        //10.3.3 move to next node
        current = current->nextAddress;
        position++;
    }
    
    //10.4 print footer
    cout << "------------------------------------------------------------" << endl;
}

//Step 11: check if an order ID already exists in the queue
// this prevents duplicate orders from being added. i walk through
// the entire queue from front to rear and compare each order's ID
// with the given ID. if i find a match, i return true immediately.
// if i reach the end without finding a match, i return false.
// this is a linear search which is O(n) but necessary because the
// queue is not sorted by order ID
bool OrderQueue::orderExists(string orderID)
{
    //11.1 start at the front of the queue
    OrderNode* current = front;
    
    //11.2 walk through the queue until we reach the end
    while (current != NULL)
    {
        //11.2.1 check if this node's order ID matches
        if (current->data.orderID == orderID)
        {
            return true;
        }
        
        //11.2.2 move to next node
        current = current->nextAddress;
    }
    
    //11.3 if we reached here, the order ID was not found
    return false;
}
