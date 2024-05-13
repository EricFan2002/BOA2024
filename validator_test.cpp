#include <iostream>
#include <cassert>
#include "csv.cpp"
#include "validator.cpp"

// Unit tests for the processOrder function
void testProcessOrder()
{
    // Test findInstrument
    vector<Instrument> instruments = {
        {"INSTR1", "USD", 1000},
        {"INSTR2", "EUR", 500},
        {"INSTR3", "GBP", 2000}
    };

    Instrument* found = findInstrument("INSTR2", instruments);
    assert(found != nullptr);
    assert(found->instrumentId == "INSTR2");
    assert(found->currency == "EUR");
    assert(found->lotSize == 500);

    Instrument* notFound = findInstrument("INVALID", instruments);
    assert(notFound == nullptr);

    // Test findClient
    vector<Client> clients = {
        {"CLIENT1", {"USD", "EUR"}},
        {"CLIENT2", {"GBP"}},
        {"CLIENT3", {"JPY", "CHF"}}
    };

    Client* foundClient = findClient("CLIENT2", clients);
    assert(foundClient != nullptr);
    assert(foundClient->clientId == "CLIENT2");
    assert(foundClient->currencies == vector<string>{"GBP"});

    Client* notFoundClient = findClient("INVALID", clients);
    assert(notFoundClient == nullptr);

    // Test processOrder
    clients = {
        {"CLIENT1", {"USD", "EUR"}, true},
        {"CLIENT2", {"GBP"}, false}
    };
    instruments = {
        {"INSTR1", "USD", 1000},
        {"INSTR2", "EUR", 500},
        {"INSTR3", "GBP", 2000}
    };

    Order order1 = {"12:34:56", 12, 34, 56, "ORDER1", "INVALID", 1000, 0, "CLIENT1", 0.0, "Buy", 0};
    assert(processOrder(order1, clients, instruments) == "REJECTED - INSTRUMENT NOT FOUND");

    Order order2 = {"13:45:12", 13, 45, 12, "ORDER2", "INSTR3", 2000, 0, "CLIENT1", 0.0, "Buy", 0};
    assert(processOrder(order2, clients, instruments) == "REJECTED - MISMATCH CURRENCY");

    Order order3 = {"14:56:23", 14, 56, 23, "ORDER3", "INSTR1", 1001, 0, "CLIENT1", 0.0, "Buy", 0};
    assert(processOrder(order3, clients, instruments) == "REJECTED - INVALID LOT SIZE");

    Order order4 = {"15:23:45", 15, 23, 45, "ORDER4", "INSTR1", 1000, 0, "CLIENT1", 0.0, "Sell", 0};
    assert(processOrder(order4, clients, instruments) == "REJECTED - POSITION CHECK FAILED");

    Order order5 = {"16:34:12", 16, 34, 12, "ORDER5", "INSTR3", 2000, 0, "CLIENT2", 0.0, "Buy", 0};
    assert(processOrder(order5, clients, instruments) == "GOOD");

    std::cout << "All unit tests passed!" << std::endl;
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
        "GOOD",
        "GOOD",
        "REJECTED - MISMATCH CURRENCY",
        "REJECTED - INVALID LOT SIZE",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
        "GOOD",
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