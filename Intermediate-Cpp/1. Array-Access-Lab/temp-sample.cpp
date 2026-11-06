#include <iostream>
#include <array>
int main() {
    // Declare and initialize a C-style array with 10 temperature readings
    int c_temps[10] = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};
    // Declare and initialize std::array with the same temperature readings
    std::array<int, 10> std_temps = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};
    std::cout << "Temperature data loaded successfully!" << std::endl;
    return 0;
}
