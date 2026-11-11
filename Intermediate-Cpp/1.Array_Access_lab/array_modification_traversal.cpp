#include <iostream>
#include <array>
#include <stdexcept>

int main() {

    int c_temps[10] = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};
    std::array<int, 10> std_temps = {72, 75, 68, 80, 77, 73, 69, 82, 78, 76};

    c_temps[2] = 85;
    std_temps[2] = 85;

    std::cout << "C-style temps: ";
    for (int i = 0; i < 10; i++){
        std::cout << c_temps[i] << " ";
    }
    std::cout << "\n";
    std::cout << "std::array temps: ";
    for (const auto& temp : std_temps){
        std::cout << temp << " ";
    }
    std::cout << "\n";
    return 0;
}
