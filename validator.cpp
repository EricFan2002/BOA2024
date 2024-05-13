#include <vector>
#include <string>
#include <iostream>
#include "csv.cpp"

using namespace std;

// Function to find an instrument by its ID
Instrument* findInstrument(const string& instrumentId, const vector<Instrument>& instruments)
{
    for (const auto& instrument : instruments)
    {
        if (instrument.instrumentId == instrumentId)
        {
            return const_cast<Instrument*>(&instrument);
        }
    }
    return nullptr;
}

// Function to find a client by its ID
Client* findClient(const string& clientId, const vector<Client>& clients)
{
    for (const auto& client : clients)
    {
        if (client.clientId == clientId)
        {
            return const_cast<Client*>(&client);
        }
    }
    return nullptr;
}

// Function to process a single order based on the specified policies
string processOrder(Order& order, const vector<Client>& clients, const vector<Instrument>& instruments)
{
    string rejectionReason;

    // Policy 1: Instrument not found
    Instrument* instrument = findInstrument(order.instrument, instruments);
    if (instrument == nullptr)
    {
        rejectionReason = "REJECTED - INSTRUMENT NOT FOUND";
        return rejectionReason;
    }

    // Policy 2: Mismatch currency
    Client* client = findClient(order.client, clients);
    if (client != nullptr)
    {
        bool currencyFound = false;
        for (const auto& currency : client->currencies)
        {
            if (currency[0] == instrument->currency[0])
            {
                currencyFound = true;
                break;
            }
        }
        if (!currencyFound)
        {
            rejectionReason = "REJECTED - MISMATCH CURRENCY";
            return rejectionReason;
        }
    }

    // Policy 3: Invalid lot size
    if (order.quantity % instrument->lotSize != 0)
    {
        rejectionReason = "REJECTED - INVALID LOT SIZE";
        return rejectionReason;
    }

    // Policy 4: Position check failed
    if (client != nullptr)
    {
        if (client->positionCheck && order.side == "Sell")
        {
            rejectionReason = "REJECTED - POSITION CHECK FAILED";
            return rejectionReason;
        }
    }

    return "GOOD";
}

// Function to match orders based on the specified policies
void matchOrders(vector<Order>& orders, const vector<Client>& clients, const vector<Instrument>& instruments)
{
    vector<pair<Order, string>> rejectedOrders;

    for (auto& order : orders)
    {
        string result = processOrder(order, clients, instruments);

        if (result != "GOOD")
        {
            rejectedOrders.push_back(make_pair(order, result));
            cout << "Order " << order.orderId << " rejected. Reason: " << result << endl;
        }
        else
        {
            // Order passed all policy checks, add it to the order book for potential matching
            // Implement the matching logic here based on your requirements
            cout << "Order " << order.orderId << " passed all policy checks and can be added to the order book." << endl;
        }
    }

    // Generate the report of rejected orders
    cout << "\nRejected Orders Report:" << endl;
    for (const auto& rejectedOrder : rejectedOrders)
    {
        cout << "Order ID: " << rejectedOrder.first.orderId << ", Rejection Reason: " << rejectedOrder.second << endl;
    }
}

int main()
{
    vector<Client> clients;
    vector<Instrument> instruments;
    vector<Order> orders;

    // Read clients, instruments, and orders from CSV files
    readClientsCSV("input_clients.csv", clients);
    readInstrumentsCSV("input_instruments.csv", instruments);
    readOrdersCSV("input_orders.csv", orders);
    
    // Match orders
    matchOrders(orders, clients, instruments);

    return 0;
}