#include <iostream>
#include "csv.hpp"
#include <queue>
#include <vector>
#include <map>
using namespace std;

vector<Client> mClients;
map<string, Client> clients;
vector<Instrument> mInstruments;
vector<Order> mOrders;

struct SellComparator
{
    bool operator()(const Order *a, const Order *b)
    {
        if (a->price != b->price)
            return a->price > b->price; // Prioritize lower price for sell orders
        else
            return clients[a->client].rating < clients[b->client].rating; // Prioritize higher user rating for equal prices
    }
};

struct BuyComparator
{
    bool operator()(const Order *a, const Order *b)
    {
        if (a->price != b->price)
            return a->price < b->price; // Prioritize higher price for sell orders
        else
            return clients[a->client].rating < clients[b->client].rating; // Prioritize higher user rating for equal prices
    }
};

priority_queue<Order *, vector<Order *>, SellComparator> sellOrders;
priority_queue<Order *, vector<Order *>, BuyComparator> buyOrders;

struct OrderAccumulate
{
    int shares;
    float price;
} sellOrdersList[1000000], buyOrdersList[1000000];
int sellOrderCnt = 0;
int buyOrderCnt = 0;
int sellOrderAccumulate = 0;
int buyOrderAccumulate = 0;

int performPositionCheckingSell(Order &order) // return how much can be bought
{
    if (clients[order.client].positionCheck == 1)
    {
        if (clients[order.client].position.count(order.instrument))
        {
            return clients[order.client].position[order.instrument];
        }
        else
            return 0;
    }
    else
        return order.quantity;
}

double getBestPrice()
{
    while (!sellOrders.empty())
    {
        auto order = sellOrders.top();
        sellOrders.pop();
        sellOrdersList[sellOrderCnt].price = order->price;
        sellOrderAccumulate = order->quantity;
        sellOrdersList[sellOrderCnt].shares = sellOrderAccumulate;
        sellOrderCnt++;
        cout << "SELL " << order->price << " at " << order->time << endl;
    }
    while (!buyOrders.empty())
    {
        auto order = buyOrders.top();
        buyOrders.pop();
        buyOrdersList[buyOrderCnt].price = order->price;
        buyOrderAccumulate = order->quantity;
        buyOrdersList[buyOrderCnt].shares = buyOrderAccumulate;
        buyOrderCnt++;
        cout << "BUY " << order->price << " at " << order->time << endl;
    }

    int sellIndex = 0, buyIndex = 0;
    double bestPrice = 0.0;
    int maxQuantity = 0;
    int sellTotal = 0;
    int buyTotal = 0;
    while (sellIndex < sellOrderCnt && buyIndex < buyOrderCnt)
    {
        cout << " buy @ " << buyOrdersList[buyIndex].price << " sell @" << sellOrdersList[sellIndex].price << endl;
        if (buyOrdersList[buyIndex].price >= sellOrdersList[sellIndex].price)
        {
            int quantity = min(sellTotal, buyTotal);
            if (quantity > maxQuantity)
            {
                maxQuantity = quantity;
                bestPrice = sellOrdersList[sellIndex].price; // Taking the sell price as the transaction price
                cout << bestPrice << " is better with quantity " << maxQuantity << endl;
            }
            if (sellTotal < buyTotal)
            {

                sellIndex++;
            }
            else
            {
                buyIndex++;
            }
        }
        else
        {
            sellIndex++; // Increase sell index if the sell price is higher than the buy price
        }
    }
    return bestPrice;
}

inline int testIfMorning(Order order)
{
    if (order.hours > 9)
        return 0;
    else if (order.hours == 9 && order.mins >= 30)
        return 0;
    return 1;
}

void processMorningAuction()
{
    // for (int i = 0; i < mOrders.size(); i++)
    // {
    //     cout << mOrders[i].time << " " << mOrders[i].side << " " << mOrders[i].price << endl;
    // }
    for (int i = 0; i < mOrders.size(); i++)
    {
        auto &oneOrder = mOrders[i];
        if (!testIfMorning(oneOrder))
            break;
        if (oneOrder.side[0] == 'B')
        {
            cout << "[B] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            buyOrders.push(&oneOrder);
        }
        else if (oneOrder.side[0] == 'S')
        {
            cout << "[S] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            sellOrders.push(&oneOrder);
        }
        else
        {
            cout << "Error " << oneOrder.orderId << " Type >" << oneOrder.side << "<" << endl;
        }
    }
    cout << "Best " << getBestPrice() << endl;
}

int main()
{
    readClientsCSV("input_clients.csv", mClients);
    for (auto i : mClients)
    {
        clients[i.clientId] = i;
    }
    readInstrumentsCSV("input_instruments.csv", mInstruments);
    readOrdersCSV("input_orders.csv", mOrders);
    processMorningAuction();
}