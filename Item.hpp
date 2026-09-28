#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
using namespace std;


struct Item
{
    string itemID;
    string name;
    string zoneID;
    string aisleID;
    string shelfID;
    int quantity;
    double weight_kg;
};

struct ItemLocation
{
    string zoneID;
    string aisleID;
    string shelfID;
    bool found;
};

#endif