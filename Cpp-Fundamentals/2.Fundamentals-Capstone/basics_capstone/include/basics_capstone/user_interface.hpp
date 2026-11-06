#ifndef BASICS_CAPSTONE__USER_INTERFACE_HPP_
#define BASICS_CAPSTONE__USER_INTERFACE_HPP_
void displayMenu();
void clearInputBuffer();
void waitForEnter();
int getMenuChoice(int min, int max);
double getPositiveDouble();
double getDouble();
double getDoubleGreaterThanZero();
#endif //BASICS_CAPSTONE__USER_INTERFACE_HPP_