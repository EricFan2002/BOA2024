#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <vector>
#include <string>
#include <map>

using namespace std;

// Data structure for client information
struct Client
{
    string clientId;
    vector<string> currencies;
    int positionCheck;
    int rating;
    map<string, int> position;
};

// Data structure for instrument information
struct Instrument
{
    string instrumentId;
    string currency;
    int lotSize;
};

// Data structure for order information
struct Order
{
    string time;
    int hours;
    int mins;
    int secs;
    string orderId;
    string instrument;
    int quantity;
    string client;
    double price;
    string side;
    int valid;
};

// Function declarations
void readClientsCSV(const string &filename, vector<Client> &clients);
void readInstrumentsCSV(const string &filename, vector<Instrument> &instruments);
void readOrdersCSV(const string &filename, vector<Order> &orders);

#endif // CSV_READER_HPP
