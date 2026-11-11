#include <iostream>

int main(){

    int mat1[3][3] = {
        {34, 20, 8},
        {14, 76, 66},
        {3, 29, 92}
    };
    int mat2[3][3] = {
        {45, 27, 96},
        {23, 65, 17},
        {83, 50, 47}
    };

    //element wise addition
    int mat3[3][3];
    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            mat3[i][j] = mat1[i][j] + mat2[i][j];
        }
    }

    //matrix multiplication
    int mat4[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            for (int k = 0; k < 3; k++){
                mat4[i][j]= mat1[i][k] * mat2[k][j];
            }
        }
    }
    return 0;
}