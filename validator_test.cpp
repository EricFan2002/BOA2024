#include <iostream>
#include <cassert>
#include "csv.hpp"
#include "validator.cpp"

// Test the processOrder function
void testProcessOrder() {
    vector<Client> clients;
    vector<Instrument> instruments;

    Client client1 = {"A", {"USD", "SGD"}, 1, 1};
    Client client2 = {"B", {"USD", "SGD", "JPY"}, 0, 2};
    clients.push_back(client1);
    clients.push_back(client2);

    Instrument instrument1 = {"SIA", "SGD", 100};
    Instrument instrument2 = {"APL", "USD", 50};
    instruments.push_back(instrument1);
    instruments.push_back(instrument2);

    Order order1 = {"9:00:01", 9, 0, 1, "A1", "SIA", 1500, "A", 0.0, "Buy"};
    Order order2 = {"9:02:00", 9, 2, 0, "B1", "APL", 4500, "B", 32.1, "Sell"};
    Order order3 = {"9:05:00", 9, 5, 0, "C1", "SIA", 105, "A", 32.0, "Buy"};
    Order order4 = {"9:10:00", 9, 10, 0, "D1", "XYZ", 300, "A", 0.0, "Sell"};
    Order order5 = {"9:29:01", 9, 29, 1, "B2", "SIA", 500, "A", 32.1, "Sell"};

    assert(processOrder(order1, clients, instruments) == "GOOD");
    assert(processOrder(order2, clients, instruments) == "GOOD");
    assert(processOrder(order3, clients, instruments) == "REJECTED - INVALID LOT SIZE");
    assert(processOrder(order4, clients, instruments) == "REJECTED - INSTRUMENT NOT FOUND");
    assert(processOrder(order5, clients, instruments) == "REJECTED - POSITION CHECK FAILED");
    std::cout << "testProcessOrder passed" << std::endl;
}

// Run all unit tests
void runUnitTests() {
    testProcessOrder();
}

// Integration test for the processOrder function
void integrationTestProcessOrder() {
    vector<Client> clients;
    vector<Instrument> instruments;
    vector<Order> orders;

    readClientsCSV("input_clients.csv", clients);
    readInstrumentsCSV("input_instruments.csv", instruments);
    readOrdersCSV("input_orders.csv", orders);

    vector<string> expectedResults = {
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "GOOD",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "GOOD",
        "GOOD"
    };

    for (size_t i = 0; i < orders.size(); ++i) {
        assert(processOrder(orders[i], clients, instruments) == expectedResults[i]);
    }
    std::cout << "integrationTestProcessOrder passed" << std::endl;
}

// Run all integration tests
void runIntegrationTests() {
    integrationTestProcessOrder();
}

int main() {
    runUnitTests();
    runIntegrationTests();
    return 0;
}