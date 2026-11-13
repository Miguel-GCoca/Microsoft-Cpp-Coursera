#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
int main() {
    const std::string inputFile = "inventory.txt";
    const std::string outputFile = "summary.txt";
    // Check if input file exists using C++17 filesystem
    if (!std::filesystem::exists(inputFile)) {
        std::cout << "Creating sample inventory file..." << std::endl;
        std::ofstream createFile(inputFile);
        createFile << "Widget A,25,15.50" << std::endl;
        createFile << "Widget B,40,22.00" << std::endl;
        createFile << "Widget C,15,8.75" << std::endl;
        createFile.close();
    }
    // Read inventory data
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Error opening inventory file" << std::endl;
        return 1;
    }
    // Process and summarize
    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error creating summary file" << std::endl;
        return 1;
    }
    outFile << "INVENTORY SUMMARY" << std::endl;
    outFile << "=================" << std::endl;
    std::string line;
    int totalItems = 0;
    double totalValue = 0.0;
    while (std::getline(inFile, line)) {
        // Simple CSV parsing (find commas)
        size_t firstComma = line.find(',');
        size_t secondComma = line.find(',', firstComma + 1);
        if (firstComma != std::string::npos && secondComma != std::string::npos) {
            std::string name = line.substr(0, firstComma);
            int quantity = std::stoi(line.substr(firstComma + 1, secondComma - firstComma - 1));
            double price = std::stod(line.substr(secondComma + 1));
            totalItems += quantity;
            totalValue += quantity * price;
            outFile << "Product: " << name << ", Qty: " << quantity
                    << ", Value: $" << std::fixed << std::setprecision(2)
                    << quantity * price << std::endl;
        }
    }
    outFile << std::endl << "Total Items: " << totalItems << std::endl;
    outFile << "Total Value: $" << std::fixed << std::setprecision(2) << totalValue << std::endl;
    inFile.close();
    outFile.close();
    std::cout << "Inventory summary completed!" << std::endl;
    std::cout << "Check summary.txt for results." << std::endl;
    return 0;
}
