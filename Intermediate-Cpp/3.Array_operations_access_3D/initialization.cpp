#include <iostream>

int main(){
    
    int seating [5][5] = {
        {0, 1, 0, 1, 0},
        {1, 1, 0, 0, 1},
        {0, 0, 1, 1, 0},
        {1, 0, 0, 1, 1},
        {0, 1, 1, 0, 0},
    };
    std::cout << "Theater seating chart initialized\n";
    std::cout << "0 = Available, 1 = Occupied\n";

    std::cout << "Seat at row 2, column 3: " << seating[1][2] << "\n";
    std::cout << "Seat at row 3, column 2: " << seating[3][2] << "\n";

    //changing availability manually
    seating[2][1] = 1;
    
    return 0;
}