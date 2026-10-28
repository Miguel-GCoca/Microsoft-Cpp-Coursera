#include <iostream>
#include "basic_math.h"
#include "advanced_math.h"
#include "geometry.h"
int main(){
    std::cout << " ======= math lib demo =====" << "\n";
    std::cout << " =======  the basics  ======" << "\n";
    std::cout << " Addition 5+3= " << add(5,3) << "\n";
    std::cout << " Subtraction 5-3= " << subtract(5,3) << "\n";
    std::cout << " Multiplication 5*3= " << multiply(5,3) << "\n";
    std::cout << " Division 5/3= " << divide(5,3) << "\n\n";


    std::cout << " =======  Advanced  =======" << "\n";
    std::cout << " Power 5^3= " << power(5,3) << "\n";
    std::cout << " Sqrt 5= " << squareRoot(5) << "\n";
    std::cout << " Absolute of -5.5= " << absolute(-5.5) << "\n\n";

    std::cout << " =======  Geometry  =======" << "\n";
    double radius =5.0;
    std::cout << " Circle with radius 5" << "\n";
    std::cout << " Area " << circleArea(radius) << "\n";
    std::cout << " Perimeter " << circlePerimeter(radius) << "\n";
    double length = 4.0, width = 6.0;
    std::cout << " Rectangle with L: " << length << " W: " << width << "\n";
    std::cout << " Area " << rectangleArea(length, width) << "\n";
    std::cout << " Perimeter " << rectanglePerimeter(length, width) << std::endl;
    return 0;
}