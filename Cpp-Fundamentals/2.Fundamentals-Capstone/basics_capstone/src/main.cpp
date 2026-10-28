/**
 * Command Line Utility Tool
 * C++ Programming Fundamentals Capstone Project
 * 
 * This program implements a multi-function command-line utility tool.
 * Users can select from several utility functions via a menu interface.
 */
#include "basics_capstone/user_interface.hpp"
#include "basics_capstone/utility_functions.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>

int main() {
    // Welcome message
    std::cout << "===============================================" << std::endl;
    std::cout << "       WELCOME TO THE COMMAND LINE UTILITY     " << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "\nThis program provides various utility functions." << std::endl;
    
    int choice = 0;
    bool running = true;
    
    // Main program loop
    while (running) {
        // Display the menu
        displayMenu();
        
        // TODO: Get user choice with validation
        // If cin fails or choice is out of range, display error message
        // and handle the invalid input
        choice = getMenuChoice(1,5);
        
        // Process user choice
        switch (choice) {
            case 1:
                utilityFunction1();
                break;
            case 2:
                utilityFunction2();
                break;
            case 3:
                utilityFunction3();
                break;
            case 4:
                utilityFunction4();
                break;
            case 5:
                std::cout << "\nThank you for using the Command Line Utility Tool. Goodbye!" << std::endl;
                running = false;
                break;
            default:
                std::cout << "\nInvalid choice. Please enter a number between 1 and 5." << std::endl;
                waitForEnter();
        }
    }
    
    return 0;
}


