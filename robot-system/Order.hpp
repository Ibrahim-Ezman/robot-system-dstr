#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
using namespace std;

//step 1.0 define the order structure
// i use a simple struct with only data fields and no constructor
// because the order data comes from CSV file and is set manually
// after creating the struct. if i used a constructor with parameters
// i would need to pass all 6 fields every time which makes the code
// harder to read. with plain struct i can create an empty order
// and then fill only the fields i need one by one
struct Order
{
    string orderID;
    string itemID;
    string customerName;
    string priority;
    string status;
    string timestamp;
};

#endif
