#include "csv.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <float.h>
#include <vector>

using namespace std;

string getRange(string &t, int start, int end)
{
    string res = "";
    for (int i = start; i < end; i++)
    {
        res += t[i];
    }
    return res;
}

// Helper function to split a string by a delimiter
vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    string tmpString = "";
    while (getline(tokenStream, token, delimiter))
    {
        if (tmpString != "")
        {
            if (token[token.length() - 1] == '"')
            {
                tmpString += getRange(token, 0, token.length() - 1);
                tokens.push_back(tmpString);
                tmpString = "";
            }
            else
            {
                tmpString += getRange(token, 0, token.length() - 1) + ",";
            }
        }
        else
        {
            if (token[0] == '"')
            {
                tmpString += getRange(token, 1, token.length()) + ",";
            }
            else
            {
                tokens.push_back(token);
                tmpString = "";
            }
        }
    }
    return tokens;
}

// Function to read and process the clients CSV
void readClientsCSV(const string &filename, vector<Client> &clients)
{
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header
    cout << "Reading Clients CSV: " << line << endl;

    while (getline(file, line))
    {
        vector<string> tokens = split(line, ',');
        Client client;
        client.clientId = tokens[0];
        cout << "DEBUG " << tokens[1] << endl;
        client.currencies = split(tokens[1], ',');
        client.positionCheck = tokens[2][0] == 'Y' ? 1 : 0;
        client.rating = stoi(tokens[3]);
        clients.push_back(client);

        cout
            << "ClientID: " << client.clientId << ", Currencies: ";
        for (auto i : client.currencies)
            cout << i << " | ";
        cout << ", Position Check: " << client.positionCheck << ", Rating: " << client.rating << endl;
    }
    file.close();
}

// Function to read and process the instruments CSV
void readInstrumentsCSV(const string &filename, vector<Instrument> &instruments)
{
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header
    cout << "Reading Instruments CSV..." << endl;

    while (getline(file, line))
    {
        vector<string> tokens = split(line, ',');
        Instrument instrument;
        instrument.instrumentId = tokens[0];
        instrument.currency = tokens[1];
        instrument.lotSize = stoi(tokens[2]);
        instruments.push_back(instrument);

        cout
            << "InstrumentID: " << instrument.instrumentId << ", Currency: " << instrument.currency
            << ", Lot Size: " << instrument.lotSize << endl;
    }
    file.close();
}

// Remove those orders that exceed 9:30:00 from the orders vector
void removeLateOrders(vector<Order> &orders)
{
    for (int i = 0; i < orders.size(); i++)
    {
        if (orders[i].hours > 9 || (orders[i].hours == 9 && orders[i].mins > 30) || (orders[i].hours == 9 && orders[i].mins == 30 && orders[i].secs > 0))
        {
            orders.erase(orders.begin() + i);
            i--;
        }
    }
}

// Function to read and process the orders CSV
void readOrdersCSV(const string &filename, vector<Order> &orders)
{
    ifstream file(filename);
    if (!file)
    {
        cerr << "Failed to open orders CSV file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    cout << "Reading Orders CSV..." << endl;
    while (getline(file, line))
    {
        vector<string> tokens = split(line, ',');
        Order order;
        order.time = tokens[0];

        // Split the time into hours, minutes, and seconds
        vector<string> timeParts = split(order.time, ':');
        order.hours = stoi(timeParts[0]);
        order.mins = stoi(timeParts[1]);
        order.secs = stoi(timeParts[2]);

        order.orderId = tokens[1];
        order.instrument = tokens[2];
        order.quantity = stoi(tokens[3]);
        order.client = tokens[4];
        order.side = tokens[6];
        if (tokens[5] == "Market")
        {
            cout << "Side " << order.side << endl;
            if (order.side[0] == 'S')
            {
                order.price = 0;
            }
            else
            {
                order.price = DBL_MAX;
            }
        }
        else
        {
            order.price = stod(tokens[5]);
        }
        if (order.side[order.side.length() - 1] < '0')
            order.side[order.side.length() - 1] = 0;
        orders.push_back(order);

        cout << "Time: " << order.time << " (Hours: " << order.hours << ", Mins: " << order.mins << ", Secs: " << order.secs << ")"
             << ", OrderID: " << order.orderId << ", Instrument: " << order.instrument
             << ", Quantity: " << order.quantity << ", Client: " << order.client << ", Price: " << order.price
             << ", Side: >" << order.side << "<" << endl;
    }

    file.close();
}

// Function to create a client object
Client make_client(const string &clientId, const string &currencies, const string &positionCheck, int rating)
{
    Client client;
    client.clientId = clientId;
    client.currencies = split(currencies, ',');
    client.positionCheck = positionCheck[0] == 'Y' ? 1 : 0;
    client.rating = rating;
    return client;
}

// Function to create an instrument object
Instrument make_instrument(const string &instrumentId, const string &currency, int lotSize)
{
    Instrument instrument;
    instrument.instrumentId = instrumentId;
    instrument.currency = currency;
    instrument.lotSize = lotSize;
    return instrument;
}

// Function to create an order object
Order make_order(const string &time, const string &orderId, const string &instrument, int quantity, const string &client, double price, const string &side)
{
    Order order;
    order.time = time;
    order.orderId = orderId;
    order.instrument = instrument;
    order.quantity = quantity;
    order.client = client;
    order.price = price;
    order.side = side;
    return order;
}