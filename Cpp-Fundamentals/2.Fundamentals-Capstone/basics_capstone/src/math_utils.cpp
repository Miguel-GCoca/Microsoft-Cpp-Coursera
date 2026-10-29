#include "basics_capstone/math_utils.hpp"
#include <cmath>

double magnitude(double x, double y, double z){
    return sqrt((pow(x,2) + pow(y,2) + pow(z,2)));
}