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
vector<Transaction> mTransactions;

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
    string client;
    string stock;
} sellOrdersList[1000000], buyOrdersList[1000000];
int sellOrderCnt = 0;
int buyOrderCnt = 0;
int sellOrderAccumulate = 0;
int buyOrderAccumulate = 0;

int performPositionCheckingSellMorning(string client, string stock, int amount) // return how much can be bought
{
    if (clients[client].positionCheck == 1)
    {
        if (clients[client].position.count(stock))
        {
            return 0;
            // return clients[client].position[stock];
        }
        else
            return 0;
    }
    else
        return amount;
}

pair<double, int> getBestPrice()
{
    while (!sellOrders.empty())
    {
        auto order = sellOrders.top();
        sellOrders.pop();
        sellOrdersList[sellOrderCnt].price = order->price;
        sellOrdersList[sellOrderCnt].client = order->client;
        sellOrdersList[sellOrderCnt].stock = order->instrument;
        sellOrdersList[sellOrderCnt].shares = order->quantity;
        sellOrderCnt++;
        cout << "SELL " << order->price << " at " << order->time << endl;
    }
    while (!buyOrders.empty())
    {
        auto order = buyOrders.top();
        buyOrders.pop();
        buyOrdersList[buyOrderCnt].price = order->price;
        buyOrdersList[buyOrderCnt].client = order->client;
        buyOrdersList[buyOrderCnt].stock = order->instrument;
        buyOrdersList[buyOrderCnt].shares = order->quantity;
        buyOrderCnt++;
        cout << "BUY " << order->price << " at " << order->time << endl;
    }
    int sellIndex = 0, buyIndex = 0;
    int sellAvail = performPositionCheckingSellMorning(sellOrdersList[0].client, sellOrdersList[0].stock, sellOrdersList[0].shares);
    sellOrderAccumulate += sellAvail;
    buyOrderAccumulate += buyOrdersList[0].shares;
    double bestPrice = 0.0;
    int maxQuantity = 0;
    while (sellIndex < sellOrderCnt && buyIndex < buyOrderCnt)
    {
        cout << " TRADE BUY @ " << buyOrdersList[buyIndex].price << " sell @" << sellOrdersList[sellIndex].price << endl;
        cout << "    AMOUNT " << sellOrderAccumulate << " | " << buyOrderAccumulate << endl;
        if (buyOrdersList[buyIndex].price >= sellOrdersList[sellIndex].price)
        {
            int quantity = min(sellOrderAccumulate, buyOrderAccumulate);
            if (quantity > maxQuantity)
            {
                maxQuantity = quantity;
                bestPrice = sellOrdersList[sellIndex].price; // Taking the sell price as the transaction price
                cout << bestPrice << " is better with quantity " << maxQuantity << endl;
            }
            if (sellOrderAccumulate < buyOrderAccumulate)
            {
                sellIndex++;
                if (sellIndex >= sellOrderCnt)
                    break;
                int sellAvail = performPositionCheckingSellMorning(sellOrdersList[sellIndex].client, sellOrdersList[sellIndex].stock, sellOrdersList[sellIndex].shares);
                sellOrderAccumulate += sellAvail;
            }
            else
            {
                buyIndex++;
                if (buyIndex >= buyOrderCnt)
                    break;
                buyOrderAccumulate += buyOrdersList[buyIndex].shares;
                clients[buyOrdersList[buyIndex].client].position[buyOrdersList[buyIndex].stock] += buyOrdersList[buyIndex].shares;
            }
        }
        else
        {
            sellIndex++; // Increase sell index if the sell price is higher than the buy price
            if (sellIndex >= sellOrderCnt)
                break;
            int sellAvail = performPositionCheckingSellMorning(sellOrdersList[sellIndex].client, sellOrdersList[sellIndex].stock, sellOrdersList[sellIndex].shares);
            sellOrderAccumulate += sellAvail;
        }
    }
    cout << "@ " << bestPrice << " match " << maxQuantity << endl;
    return make_pair<>(bestPrice, maxQuantity);
}

void addTransaction(string from, string to, string stock, int amount, double price, int hr, int min, int sec)
{
    Transaction t;
    t.from = from;
    t.to = to;
    t.amount = amount;
    t.hours = hr;
    t.mins = min;
    t.secs = sec;
    t.stock = stock;
    mTransactions.push_back(t);
}

void performAuction(double price, int totalAmount)
{
    while (!sellOrders.empty())
    {
        auto order = sellOrders.top();
        sellOrders.pop();
        sellOrdersList[sellOrderCnt].price = order->price;
        sellOrdersList[sellOrderCnt].client = order->client;
        sellOrdersList[sellOrderCnt].stock = order->instrument;
        sellOrdersList[sellOrderCnt].shares = order->quantity;
        sellOrderCnt++;
        cout << "SELL " << order->price << " at " << order->time << endl;
    }
    while (!buyOrders.empty())
    {
        auto order = buyOrders.top();
        buyOrders.pop();
        buyOrdersList[buyOrderCnt].price = order->price;
        buyOrdersList[buyOrderCnt].client = order->client;
        buyOrdersList[buyOrderCnt].stock = order->instrument;
        buyOrdersList[buyOrderCnt].shares = order->quantity;
        buyOrderCnt++;
        cout << "BUY " << order->price << " at " << order->time << endl;
    }
    int sellIndex = 0, buyIndex = 0;
    int sellOrderAccumulate = 0;
    int buyOrderAccumulate = 0;
    while (sellIndex <= sellOrderCnt && buyIndex <= buyOrderCnt && thisOrderAmount != 0)
    {
        int thisOrderAmount = min(buyOrdersList[buyIndex].shares, sellOrdersList[sellIndex].shares);
        thisOrderAmount = min(totalAmount, thisOrderAmount);
        addTransaction(sellOrdersList[sellIndex].client, buyOrdersList[buyIndex].client, )
    }
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
    pair<double, int> res = getBestPrice();
    int accumulateSell = 0;
    int accumulateBuy = 0;
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