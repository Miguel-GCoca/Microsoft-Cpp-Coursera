#include "basics_capstone/user_interface.hpp"
#include <iostream>
#include <limits>

/**
 * Displays the main menu options to the user
 */
void displayMenu() {
    std::cout << "\n=============== MAIN MENU ===============" << std::endl;
    std::cout << "1. 3-DOF IK Calculator" << std::endl;
    std::cout << "2. Joint Control PID Sim" << std::endl;
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
            std::cout << "Enter a number between " << min << " and " << max << std::endl;
        }
    }
    return input;
}

/**
 * Get user decimal value, positive or negative.
 */
double getDouble(){
    bool gotNum = false;
    double input;
    while (!gotNum){
        if (std::cin >> input){
            gotNum = true;
        }
        else{
            clearInputBuffer();
            std::cout << "Error: Expected number as input\n";
            std::cout << "Enter a valid decimal value" << std::endl;
        }
    }
    return input;
}

/**
 * Get POSITIVE user decimal value
 */
double getPositiveDouble(){
    bool gotNum = false;
    double input;
    while(!gotNum){
        input = getDouble();
        if (input >= 0){
            gotNum = true;
        }
        else{
            std::cout << "Error: Cannot process negative values\n";
            std::cout << "Please enter a Positive decimal value or zero" << std::endl;
        }
    }
    return input;
}

/**
 * Get POSITIVE user decimal value
 */
double getDoubleGreaterThanZero(){
    bool gotNum = false;
    double input;

    while(!gotNum){
        input = getDouble();
        if (input > 0){
            gotNum = true;
        }
        else{
            std::cout << "Error: Cannot process negative or zero\n";
            std::cout << "Please enter a Positive decimal value > zero" << std::endl;
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