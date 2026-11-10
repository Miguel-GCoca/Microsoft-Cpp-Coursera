#include <vector>

void bubbleSort(int my_array[], int size);

int main(){
    int c_style[20] = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 62};
    std::vector<int> std_style = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 68};


    return 0;void bubbleSort(int my_array[]);
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