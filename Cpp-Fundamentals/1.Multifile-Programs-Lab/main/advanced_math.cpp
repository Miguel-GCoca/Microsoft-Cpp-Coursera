#include "../includes/advanced_math.h"
#include <cmath>
double power(double base, int exponent){
    return pow(base, exponent);
}
double squareRoot(double number){
    return sqrt(number);
}
double absolute(double number){
    return (number >= 0 ? number : number * -1);
}