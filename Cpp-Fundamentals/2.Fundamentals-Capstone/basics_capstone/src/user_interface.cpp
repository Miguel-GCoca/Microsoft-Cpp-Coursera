#include "basics_capstone/user_interface.hpp"
#include <iostream>
#include <limits>

/**
 * Displays the main menu options to the user
 */
void displayMenu() {
    std::cout << "\n=============== MAIN MENU ===============" << std::endl;
    std::cout << "1. Utility Function 1" << std::endl;
    std::cout << "2. Utility Function 2" << std::endl;
    std::cout << "3. Utility Function 3" << std::endl;
    std::cout << "4. Utility Function 4" << std::endl;
    std::cout << "5. Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter your choice (1-5): ";
}

/**
 * Get user choice with validation, handle invalid input.
 */
int getMenuChoice(int min, int max){
    bool gotNum = false; 
    int input;
    while (!gotNum){
        if (std::cin >> input){
            if (input >= min && input <= max){
                gotNum = true;
            }
            else{
                std::cout << "Error: Invalid input range\n";
                std::cout << "Enter a value between " << min << " and " << max << std::endl;
            }
        }
        else{
            clearInputBuffer();
            std::cout << "Error: Invalid menu input\n";
            std::cout << "Enter a value between " << min << " and " << max << std::endl;
        }
    }
    return input;
}

/**
 * Clears the input buffer to prevent issues with subsequent input operations
 */
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * Waits for the user to press Enter before continuing
 */
void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    clearInputBuffer();
    std::cin.get();
}