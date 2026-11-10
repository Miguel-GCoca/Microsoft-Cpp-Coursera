#include <array>
#include <numeric>
#include <cstdio>

int main(){
    int c_style[20] = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 62};
    std::array<int, 20> std_style = {64, 14, 90, 49, 27, 54, 36, 96, 28, 20, 17, 51, 86, 84, 26, 40, 12, 70, 21, 68};

    int sum = std::accumulate(std_style.begin(), std_style.end(), 0);
    float mean = sum / std_style.size();
    
    printf("the sum is: %d, and the mean is: %f",sum, mean);

    return 0;
}