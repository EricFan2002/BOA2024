#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include "csv.cpp"

using namespace std;

double calculateVWAP(const std::vector<Transaction>& transactions) {
    double totalValue = 0.0;
    int totalVolume = 0;

    for (const auto& transaction : transactions) {
        totalValue += transaction.price * transaction.amount;
        totalVolume += transaction.amount;
    }

    if (totalVolume == 0) {
        return 0.0;
    }

    return totalValue / totalVolume;
}

int main() {
    std::vector<Transaction> transactions;

    // Read transaction data from a CSV file
    std::ifstream file("transactions.csv");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string instrumentId;
            double price;
            int volume;

            std::getline(ss, instrumentId, ',');
            ss >> price;
            ss.ignore();
            ss >> volume;

            transactions.push_back({instrumentId, price, volume});
        }
        file.close();
    }

    // Group transactions by instrument ID
    std::sort(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b) {
        return a.instrumentId < b.instrumentId;
    });

    std::string currentInstrument = "";
    std::vector<Transaction> instrumentTransactions;

    for (const auto& transaction : transactions) {
        if (transaction.instrumentId != currentInstrument) {
            if (!instrumentTransactions.empty()) {
                double vwap = calculateVWAP(instrumentTransactions);
                std::cout << "Instrument ID: " << currentInstrument << ", VWAP: " << std::fixed << std::setprecision(2) << vwap << std::endl;
                instrumentTransactions.clear();
            }
            currentInstrument = transaction.instrumentId;
        }
        instrumentTransactions.push_back(transaction);
    }

    if (!instrumentTransactions.empty()) {
        double vwap = calculateVWAP(instrumentTransactions);
        std::cout << "Instrument ID: " << currentInstrument << ", VWAP: " << std::fixed << std::setprecision(2) << vwap << std::endl;
    }

    return 0;
}