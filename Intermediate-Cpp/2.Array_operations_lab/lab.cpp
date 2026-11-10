#include <array>
#include <algorithm>
#include <cstdio>
#include <iostream>

void bubbleSort(int my_array[], int size);

int main(){
    int c_style[20] = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 62};
    std::array<int, 20> std_style = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 68};

    printf("c_style array: ");
    for(int i = 0; i < 20; i++){
        printf("%d ", c_style[i]);
    }
    bubbleSort(c_style, 20);
    printf("\nsorted c_style: ");
    for(int i = 0; i < 20; i++){
        printf("%d ", c_style[i]);
    }

    printf("\nStd_style array: ");
    for(int element : std_style){
        std::cout << element << " ";
    }
    std::sort(std_style.begin(), std_style.end());
    printf("\nSorted std_style array");
    for(int element : std_style){
        std::cout << element << " ";
    }

    
    return 0;
}

void bubbleSort(int my_array[], int size){
    for (int i = 0; i < size - 1; i++){
        bool swapped = false;

        for (int j = 0; j < size -i -1; j++){
            if(my_array[j] > my_array[j+1]){
                int temp = my_array[j];
                my_array[j] = my_array[j+1];
                my_array[j+1] = temp;
                swapped = true;
            }
        }
        if (!swapped){
            break;
        }
    }
}