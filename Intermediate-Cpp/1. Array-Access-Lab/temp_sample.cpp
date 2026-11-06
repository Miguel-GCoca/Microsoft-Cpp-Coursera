#include <iostream>
#include <array>
int main() {

    int c_temps[10] = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};
    std::array<int, 10> std_temps = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};
    // Access the 5th, 7th, and 10th temperature readings
    std::cout << "5th reading - C-style: " << c_temps[4] << std::endl;
    std::cout << "5th reading - std::array: " << std_temps[4] << std::endl;
    std::cout << "7th reading - c-style: " << c_temps[6] << std::endl;
    std::cout << "7th reading - std::array: " << std_temps[9] << std::endl;
    std::cout << "10th reading  c-style: " << c_temps[9] << std::endl;
    std::cout << "10th reading  std::array: " << std_temps[9] << std::endl;
    std::cout << "Out-of-bounds test:" << std::endl;
    std::cout << "C-style[10]: " << c_temps[10] << " (unsafe!)" << std::endl;

    std::cout << "\nOut of bounds test:\n";
    std::cout << "c-style[10]: " << c_temps[10] << "\n";
    try{
        std::cout << "std::array.at(10): " << std_temps.at(10) << std::endl;
    }
    catch (const std::out_of_range& e){
        std::cout << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
