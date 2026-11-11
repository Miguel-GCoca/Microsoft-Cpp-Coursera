#include <iostream>

int main(){
    
    int seating [5][5] = {
        {0, 1, 0, 1, 0},
        {1, 1, 0, 0, 1},
        {0, 0, 1, 1, 0},
        {1, 0, 0, 1, 1},
        {0, 1, 1, 0, 0},
    };

    //safe access function
    auto getSeat = [&](int row, int col) -> int{
        if (row >= 0 && row < 5 && col >=0 && col < 5){
            return seating[row][col];
        }else{
            std::cout << "invalid seat position: R" << row <<" C" << col << std::endl;
            return -1;
        }
    };

    std::cout << "valid access: R2, C3: " << getSeat(2,3) << std::endl;
    std::cout << "invalid access: R6, C3: " << getSeat(6,3) << std::endl;
    
    return 0;
}