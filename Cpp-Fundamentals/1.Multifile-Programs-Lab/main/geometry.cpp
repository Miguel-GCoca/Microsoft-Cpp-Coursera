#include "../includes/geometry.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double circleArea(double radius){
    return M_PI * radius *radius;
}
double circlePerimeter(double radius){
    return 2* M_PI * radius;
}
double rectangleArea(double length, double width){
    return length * width;
}
double rectanglePerimeter(double length, double width){
    return 2*(length+width);
}