#include <iostream>
#include "csv.cpp"
#include <queue>
#include <float.h>
#include <vector>
#include <map>
using namespace std;

vector<Client> mClients;
map<string, Client> clients;
vector<Instrument> mInstruments;
vector<Order> mOrders;
vector<Transaction> mTransactions;

struct SellComparatorRealTime
{
    bool operator()(const Order *a, const Order *b)
    {
        // sell: low price, client rating
        if (a->price != b->price)
            return a->price > b->price; // Prioritize lower price for sell orders
        else
            return clients[a->client].rating < clients[b->client].rating; // Prioritize higher user rating for equal prices
    }
};

struct BuyComparatorRealTime
{
    bool operator()(const Order *a, const Order *b)
    {
        if (a->price != b->price)
            return a->price < b->price; // Prioritize higher price for sell orders
        else
            return clients[a->client].rating < clients[b->client].rating; // Prioritize higher user rating for equal prices
    }
};

struct Time
{
    int hr;
    int min;
    int sec;
    bool operator<(const Time &other) const
    {
        if (hr != other.hr)
            return hr < other.hr;
        if (min != other.min)
            return min < other.min;
        return sec < other.sec;
    }
};

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
            return a->price > b->price; // Prioritize higher price for sell orders
        else
            return clients[a->client].rating < clients[b->client].rating; // Prioritize higher user rating for equal prices
    }
};

map<string, priority_queue<Order *, vector<Order *>, SellComparator>> sellOrders;
map<string, priority_queue<Order *, vector<Order *>, BuyComparator>> buyOrders;

map<string, priority_queue<Order *, vector<Order *>, SellComparatorRealTime>> sellOrdersRealTime;
map<string, priority_queue<Order *, vector<Order *>, BuyComparatorRealTime>> buyOrdersRealTime;

struct OrderAccumulate
{
    int shares;
    double price;
    string client;
    string stock;
    Order *order;
};

map<string, vector<OrderAccumulate>> sellOrdersList;
map<string, vector<OrderAccumulate>> buyOrdersList;

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
        }
        else
            return 0;
    }
    else
        return amount;
}

Order addOrder(string client, string instrument, int quantity, int price, int hours, int mins, int sec)
{
    Order o;
    o.instrument = instrument;
    o.client = client;
    o.hours = hours;
    o.mins = mins;
    o.secs = sec;
    o.price = price;
    o.quantity = quantity;
    return o;
}

map<string, double> getBestPrice()
{
    map<string, double> res;
    for (auto ins : mInstruments)
    {
        int maxSell = 0;
        double maxAmount = 0;
        string thisins = ins.instrumentId;
        sellOrderCnt = 0;
        buyOrderCnt = 0;
        buyOrderAccumulate = 0;
        sellOrderAccumulate = 0;
        sellOrdersList[thisins].reserve(sellOrders[thisins].size());
        buyOrdersList[thisins].reserve(buyOrders[thisins].size());
        while (!sellOrders[thisins].empty())
        {
            auto order = sellOrders[thisins].top();
            sellOrders[thisins].pop();
            sellOrdersList[thisins][sellOrderCnt].price = order->price;
            sellOrdersList[thisins][sellOrderCnt].client = order->client;
            sellOrdersList[thisins][sellOrderCnt].stock = order->instrument;
            sellOrdersList[thisins][sellOrderCnt].shares = order->quantity;
            sellOrderCnt++;
            cout << "SELL " << order->price << " at " << order->time << endl;
        }
        while (!buyOrders[thisins].empty())
        {
            auto order = buyOrders[thisins].top();
            buyOrders[thisins].pop();
            buyOrdersList[thisins][buyOrderCnt].price = order->price;
            buyOrdersList[thisins][buyOrderCnt].client = order->client;
            buyOrdersList[thisins][buyOrderCnt].stock = order->instrument;
            buyOrdersList[thisins][buyOrderCnt].shares = order->quantity;
            buyOrderAccumulate += order->quantity;
            buyOrderCnt++;
            cout << "BUY " << order->price << " at " << order->time << endl;
        }
        int sellI = 0;
        for (int i = 0; i < buyOrderCnt; i++)
        {
            auto buy = buyOrdersList[thisins][i];
            double price = buy.price;
            double maxSellPrice = 0;
            // cout << "sell min price " << sellOrdersList[thisins][sellI].price << endl;
            // cout << (sellOrdersList[thisins][sellI].price <= buy.price) << " " << (sellI < sellOrderCnt) << " " << sellOrderCnt << endl;
            while (sellOrdersList[thisins][sellI].price <= buy.price && sellI < sellOrderCnt)
            {
                sellOrderAccumulate += sellOrdersList[thisins][sellI].shares;
                maxSellPrice = maxSellPrice > sellOrdersList[thisins][sellI].price ? maxSellPrice : sellOrdersList[thisins][sellI].price;
                sellI++;
            }
            if (buy.price == DBL_MAX)
                price = maxSellPrice;
            cout << "@ " << price << " can buy " << buyOrderAccumulate << " can sell " << sellOrderAccumulate << endl;
            int sell = min(buyOrderAccumulate, sellOrderAccumulate);
            if (sell >= maxSell)
            {
                maxSell = sell;
                maxAmount = price;
            }
            buyOrderAccumulate -= buy.shares;
        }
        cout << thisins << " : " << maxAmount << endl;
        res[thisins] = maxAmount;
    }
    return res;
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
    cout << "-------------------\nTransaction Details:" << endl;
    cout << "From: " << t.from;
    cout << "  To: " << t.to;
    cout << " . Stock: " << t.stock;
    cout << " . Amount: " << t.amount;
    cout << " . Price: " << t.price;
    cout << " . Time: " << t.hours << ":" << t.mins << ":" << t.secs << endl;
    mTransactions.push_back(t);
}

void processAllMorningTransactions(map<string, double> targetPrice)
{
    map<string, double> res;
    for (auto ins : mInstruments)
    {
        int maxSell = 0;
        double maxAmount = 0;
        string thisins = ins.instrumentId;
        sellOrderCnt = 0;
        buyOrderCnt = 0;
        buyOrderAccumulate = 0;
        sellOrderAccumulate = 0;
        sellOrdersList[thisins].reserve(sellOrders[thisins].size());
        buyOrdersList[thisins].reserve(buyOrders[thisins].size());
        while (!sellOrders[thisins].empty())
        {
            auto order = sellOrders[thisins].top();
            sellOrders[thisins].pop();
            if (order->price != 0)
            {
                if (order->price > targetPrice[thisins])
                {
                    sellOrdersRealTime[thisins].push(order);
                }
                else
                {
                    sellOrdersList[thisins][sellOrderCnt].price = order->price;
                    sellOrdersList[thisins][sellOrderCnt].client = order->client;
                    sellOrdersList[thisins][sellOrderCnt].stock = order->instrument;
                    sellOrdersList[thisins][sellOrderCnt].shares = order->quantity;
                    sellOrdersList[thisins][sellOrderCnt].order = order;
                    sellOrderCnt++;
                    cout << "+SELL " << order->client << " " << order->price << " " << order->quantity << " at " << order->time << endl;
                }
            }
        }
        while (!sellOrders[thisins].empty())
        {
            auto order = sellOrders[thisins].top();
            sellOrders[thisins].pop();
            if (order->price == 0)
            {
                if (order->price > targetPrice[thisins])
                    break;
                sellOrdersList[thisins][sellOrderCnt].price = order->price;
                sellOrdersList[thisins][sellOrderCnt].client = order->client;
                sellOrdersList[thisins][sellOrderCnt].stock = order->instrument;
                sellOrdersList[thisins][sellOrderCnt].shares = order->quantity;
                sellOrdersList[thisins][sellOrderCnt].order = order;
                sellOrderCnt++;
                cout << "+SELL " << order->client << " " << order->price << " " << order->quantity << " at " << order->time << endl;
            }
            else
                break;
        }
        while (!buyOrders[thisins].empty())
        {
            auto order = buyOrders[thisins].top();
            buyOrders[thisins].pop();
            if (order->price > targetPrice[thisins])
            {
                buyOrdersList[thisins][buyOrderCnt].price = order->price;
                buyOrdersList[thisins][buyOrderCnt].client = order->client;
                buyOrdersList[thisins][buyOrderCnt].stock = order->instrument;
                buyOrdersList[thisins][buyOrderCnt].shares = order->quantity;
                buyOrdersList[thisins][buyOrderCnt].order = order;
                buyOrderAccumulate += order->quantity;
                buyOrderCnt++;
                cout << "+BUY " << order->client << " " << order->price << " " << order->quantity << " at " << order->time << endl;
            }
            else
            {
                if (order->price > targetPrice[thisins])
                {
                    buyOrdersRealTime[thisins].push(order);
                }
            }
        }
        int sellI = 0;
        int buyI = 0;
        int totalSale = 0;
        while (sellI < sellOrderCnt && buyI < buyOrderCnt)
        {
            auto &buy = buyOrdersList[thisins][buyI];
            auto &sell = sellOrdersList[thisins][sellI];
            int trade = min(buyOrdersList[thisins][buyI].shares, sellOrdersList[thisins][sellI].shares);
            totalSale += trade;
            cout << sell.client << " buy " << buy.client << " buy " << trade << endl;
            buy.shares -= trade;
            sell.shares -= trade;
            cout << " BUY -= " << buy.shares << " SELL -= " << sell.shares << endl;
            addTransaction(sell.client, buy.client, thisins, trade, targetPrice[thisins], 9, 30, 0);
            if (buy.shares == 0)
            {
                buyI++;
            }
            else
            {
                sellI++;
            }
        }
        while (sellI < sellOrderCnt)
        {
            auto &sell = sellOrdersList[thisins][sellI];
            Order *o = new Order;
            memcpy(o, sell.order, sizeof(Order));
            o->quantity = sell.shares;
            sellI++;
            sellOrdersRealTime[thisins].push(o);
            cout << "add extra sell " << sell.client << " " << o->quantity << endl;
        }
        while (buyI < buyOrderCnt)
        {
            auto &buy = buyOrdersList[thisins][buyI];
            Order *o = new Order;
            memcpy(o, buy.order, sizeof(Order));
            o->quantity = buy.shares;
            buyI++;
            buyOrdersRealTime[thisins].push(o);
            cout << "add extra buy " << buy.client << " " << o->quantity << endl;
        }
        cout << thisins << " Sale : " << totalSale << endl;
        res[thisins] = maxAmount;
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

int processMorningAuction()
{
    int i;
    for (i = 0; i < mOrders.size(); i++)
    {
        auto &oneOrder = mOrders[i];
        if (!testIfMorning(oneOrder))
            break;
        if (oneOrder.side[0] == 'B')
        {
            cout << "[B] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            buyOrders[oneOrder.instrument].push(&oneOrder);
        }
        else if (oneOrder.side[0] == 'S')
        {
            cout << "[S] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            sellOrders[oneOrder.instrument].push(&oneOrder);
        }
        else
        {
            cout << "Error " << oneOrder.orderId << " Type >" << oneOrder.side << "<" << endl;
        }
    }
    auto res = getBestPrice();
    buyOrders.clear();
    sellOrders.clear();
    for (int i = 0; i < mOrders.size(); i++)
    {
        auto &oneOrder = mOrders[i];
        if (!testIfMorning(oneOrder))
            break;
        if (oneOrder.side[0] == 'B')
        {
            cout << "[B] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            buyOrders[oneOrder.instrument].push(&oneOrder);
        }
        else if (oneOrder.side[0] == 'S')
        {
            cout << "[S] " << oneOrder.time << " " << oneOrder.side << " " << oneOrder.price << endl;
            sellOrders[oneOrder.instrument].push(&oneOrder);
        }
        else
        {
            cout << "Error " << oneOrder.orderId << " Type >" << oneOrder.side << "<" << endl;
        }
    }
    processAllMorningTransactions(res);
    // for (auto i : mInstruments)
    // {
    //     string name = i.instrumentId;
    //     while (!buyOrdersRealTime[name].empty())
    //     {
    //         auto &i = buyOrdersRealTime[name].top();
    //         cout << "REALTIME BUY " << i->client << " pri " << i->price << " amout " << i->quantity << endl;
    //         buyOrdersRealTime[name].pop();
    //     }
    // }
    // for (auto i : mInstruments)
    // {
    //     string name = i.instrumentId;
    //     while (!sellOrdersRealTime[name].empty())
    //     {
    //         auto &i = sellOrdersRealTime[name].top();
    //         cout << "REALTIME SELL " << i->client << " pri " << i->price << " amout " << i->quantity << endl;
    //         sellOrdersRealTime[name].pop();
    //     }
    // }
    return i;
}

int processRealTimeTransactions(int start)
{
    while (start < mOrders.size())
    {
        cout << "@" << start << endl;
        int if_any = 0;
        for (auto ins : mInstruments)
        {
            string thisins = ins.instrumentId;
            if (sellOrdersRealTime[thisins].empty() || buyOrdersRealTime[thisins].empty())
            {
                cout << thisins << " no order!" << endl;
                continue;
            }
            else
            {
                if (sellOrdersRealTime[thisins].top()->price <= buyOrdersRealTime[thisins].top()->price)
                {
                    int max_amount = min(sellOrdersRealTime[thisins].top()->quantity, buyOrdersRealTime[thisins].top()->quantity);
                    auto &sell = sellOrdersRealTime[thisins].top();
                    auto &buy = buyOrdersRealTime[thisins].top();
                    cout << "sell " << sell->price << " buy " << buy->price << endl;
                    sell->quantity -= max_amount;
                    buy->quantity -= max_amount;
                    Time time1{sell->hours, sell->mins, sell->secs};
                    Time time2{buy->hours, buy->mins, buy->secs};
                    Time maxTime = time1 < time2 ? time2 : time1;
                    addTransaction(sell->client, buy->client, thisins, max_amount, buy->price, maxTime.hr, maxTime.min, maxTime.sec);
                    if_any = 1;
                    if (sellOrdersRealTime[thisins].top()->quantity == 0)
                    {
                        sellOrdersRealTime[thisins].pop();
                    }
                    if (buyOrdersRealTime[thisins].top()->quantity == 0)
                    {
                        buyOrdersRealTime[thisins].pop();
                    }
                }
            }
        }
        if (!if_any)
        {
            auto &o = mOrders[start];
            if (o.side[0] == 'S')
            {
                sellOrdersRealTime[o.instrument].push(&o);
            }
            else
            {
                buyOrdersRealTime[o.instrument].push(&o);
            }
            start++;
        }
    }
    return start;
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
    removeLateOrders(mOrders);
    int i = processMorningAuction();
    i = processRealTimeTransactions(i);
}