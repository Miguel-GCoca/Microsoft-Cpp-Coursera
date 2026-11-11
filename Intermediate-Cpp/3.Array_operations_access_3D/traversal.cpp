#include <iostream>

int main(){
    
    int seating [5][5] = {
        {0, 1, 0, 1, 0},
        {1, 1, 0, 0, 1},
        {0, 0, 1, 1, 0},
        {1, 0, 0, 1, 1},
        {0, 1, 1, 0, 0},
    };

    //row by row traversal
    std::cout << "seating, row by row:\n";
    for (int row = 0; row < 5; row++){
        std::cout << "Row " << (row + 1) << ": ";
        for (int col = 0; col < 5; col++){
            std::cout << seating[row][col] << " ";
        }
        std::cout << std::endl;
    }
    
    //column by column traversal
    int available = 0, occupied = 0;
    for (int row = 0; row < 5; row++){
        for(int col = 0; col < 5; col++){
            if(seating[row][col] == 0){
                available ++;
            }else{
                occupied++;
            }
        }
    }
    std::cout << "\nSummary: " << available << "are available, ";
    std::cout << "and " << occupied << "are not available";

    return 0;
}