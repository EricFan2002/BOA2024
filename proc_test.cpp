// proc_test.cpp

#include <cassert>
#include <iostream>
#include "proc.cpp"

// Unit Tests

void testClientStructure() {
    Client client;
    client.clientId = "C1";
    client.rating = 5;
    client.positionCheck = 1;
    client.position["AAPL"] = 100;

    assert(client.clientId == "C1");
    assert(client.rating == 5);
    assert(client.positionCheck == 1);
    assert(client.position["AAPL"] == 100);

    std::cout << "testClientStructure passed" << std::endl;
}

void testInstrumentStructure() {
    Instrument instrument;
    instrument.instrumentId = "AAPL";

    assert(instrument.instrumentId == "AAPL");

    std::cout << "testInstrumentStructure passed" << std::endl;
}

void testOrderStructure() {
    Order order;
    order.orderId = "O1";
    order.instrument = "AAPL";
    order.client = "C1";
    order.side = "BUY";
    order.price = 150.0;
    order.quantity = 100;
    order.hours = 9;
    order.mins = 30;
    order.secs = 0;

    assert(order.orderId == "O1");
    assert(order.instrument == "AAPL");
    assert(order.client == "C1");
    assert(order.side == "BUY");
    assert(order.price == 150.0);
    assert(order.quantity == 100);
    assert(order.hours == 9);
    assert(order.mins == 30);
    assert(order.secs == 0);

    std::cout << "testOrderStructure passed" << std::endl;
}

void testTransactionStructure() {
    Transaction transaction;
    transaction.from = "C1";
    transaction.to = "C2";
    transaction.stock = "AAPL";
    transaction.amount = 100;
    transaction.hours = 9;
    transaction.mins = 30;
    transaction.secs = 0;

    assert(transaction.from == "C1");
    assert(transaction.to == "C2");
    assert(transaction.stock == "AAPL");
    assert(transaction.amount == 100);
    assert(transaction.hours == 9);
    assert(transaction.mins == 30);
    assert(transaction.secs == 0);

    std::cout << "testTransactionStructure passed" << std::endl;
}

void testPerformPositionCheckingSellMorning() {
    Client client;
    client.clientId = "C1";
    client.positionCheck = 1;
    client.position["AAPL"] = 100;
    clients["C1"] = client;

    assert(performPositionCheckingSellMorning("C1", "AAPL", 50) == 0);
    assert(performPositionCheckingSellMorning("C1", "GOOGL", 50) == 0);

    client.positionCheck = 0;
    clients["C1"] = client;

    assert(performPositionCheckingSellMorning("C1", "AAPL", 50) == 50);
    assert(performPositionCheckingSellMorning("C1", "GOOGL", 50) == 50);

    std::cout << "testPerformPositionCheckingSellMorning passed" << std::endl;
}

void testAddOrder() {
    Order order = addOrder("C1", "AAPL", 100, 150, 9, 30, 0);

    assert(order.client == "C1");
    assert(order.instrument == "AAPL");
    assert(order.quantity == 100);
    assert(order.price == 150);
    assert(order.hours == 9);
    assert(order.mins == 30);
    assert(order.secs == 0);

    std::cout << "testAddOrder passed" << std::endl;
}

void testTestIfMorning() {
    Order order1;
    order1.hours = 9;
    order1.mins = 0;
    assert(testIfMorning(order1) == 1);

    Order order2;
    order2.hours = 9;
    order2.mins = 30;
    assert(testIfMorning(order2) == 0);

    Order order3;
    order3.hours = 10;
    order3.mins = 0;
    assert(testIfMorning(order3) == 0);

    std::cout << "testTestIfMorning passed" << std::endl;
}

// Integration Tests

void testAddTransactionAndGetBestPrice() {
    mInstruments.clear();
    mOrders.clear();
    mTransactions.clear();
    buyOrders.clear();
    sellOrders.clear();

    Instrument instrument1;
    instrument1.instrumentId = "AAPL";
    mInstruments.push_back(instrument1);

    Order order1 = addOrder("C1", "AAPL", 100, 150, 9, 30, 0);
    Order order2 = addOrder("C2", "AAPL", 50, 160, 9, 30, 0);
    Order order3 = addOrder("C3", "AAPL", 80, 140, 9, 30, 0);
    Order order4 = addOrder("C4", "AAPL", 120, 155, 9, 30, 0);

    buyOrders["AAPL"].push(&order1);
    buyOrders["AAPL"].push(&order2);
    sellOrders["AAPL"].push(&order3);
    sellOrders["AAPL"].push(&order4);

    map<string, double> bestPrices = getBestPrice();

    assert(bestPrices["AAPL"] == 150);

    addTransaction("C3", "C2", "AAPL", 50, 160, 9, 30, 0);

    assert(mTransactions.size() == 1);
    assert(mTransactions[0].from == "C3");
    assert(mTransactions[0].to == "C2");
    assert(mTransactions[0].stock == "AAPL");
    assert(mTransactions[0].amount == 50);
    assert(mTransactions[0].hours == 9);
    assert(mTransactions[0].mins == 30);
    assert(mTransactions[0].secs == 0);

    std::cout << "testAddTransactionAndGetBestPrice passed" << std::endl;
}

void testProcessMorningAuction() {
    mInstruments.clear();
    mOrders.clear();
    mTransactions.clear();
    buyOrders.clear();
    sellOrders.clear();

    Instrument instrument1;
    instrument1.instrumentId = "AAPL";
    mInstruments.push_back(instrument1);

    Order order1;
    order1.orderId = "O1";
    order1.instrument = "AAPL";
    order1.client = "C1";
    order1.side = "BUY";
    order1.price = 150.0;
    order1.quantity = 100;
    order1.hours = 9;
    order1.mins = 30;
    order1.secs = 0;

    Order order2;
    order2.orderId = "O2";
    order2.instrument = "AAPL";
    order2.client = "C2";
    order2.side = "BUY";
    order2.price = 160.0;
    order2.quantity = 50;
    order2.hours = 9;
    order2.mins = 30;
    order2.secs = 0;

    Order order3;
    order3.orderId = "O3";
    order3.instrument = "AAPL";
    order3.client = "C3";
    order3.side = "SELL";
    order3.price = 140.0;
    order3.quantity = 80;
    order3.hours = 9;
    order3.mins = 30;
    order3.secs = 0;

    Order order4;
    order4.orderId = "O4";
    order4.instrument = "AAPL";
    order4.client = "C4";
    order4.side = "SELL";
    order4.price = 155.0;
    order4.quantity = 120;
    order4.hours = 9;
    order4.mins = 30;
    order4.secs = 0;

    mOrders.push_back(order1);
    mOrders.push_back(order2);
    mOrders.push_back(order3);
    mOrders.push_back(order4);

    processMorningAuction();

    assert(mTransactions.size() == 2);
    assert(mTransactions[0].from == "C3");
    assert(mTransactions[0].to == "C2");
    assert(mTransactions[0].stock == "AAPL");
    assert(mTransactions[0].amount == 50);
    assert(mTransactions[1].from == "C3");
    assert(mTransactions[1].to == "C1");
    assert(mTransactions[1].stock == "AAPL");
    assert(mTransactions[1].amount == 30);

    std::cout << "testProcessMorningAuction passed" << std::endl;
}

int main() {
    testClientStructure();
    testInstrumentStructure();
    testOrderStructure();
    testTransactionStructure();
    testPerformPositionCheckingSellMorning();
    testAddOrder();
    testTestIfMorning();
    testAddTransactionAndGetBestPrice();
    testProcessMorningAuction();

    return 0;
}