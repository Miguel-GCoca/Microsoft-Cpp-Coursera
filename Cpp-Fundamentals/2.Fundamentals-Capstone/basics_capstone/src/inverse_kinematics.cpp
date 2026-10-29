#include "basics_capstone/inverse_kinematics.hpp"
#include "basics_capstone/user_interface.hpp"
#include "basics_capstone/math_utils.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdio>
const double PI = std::acos(-1.0);


/**
 * First menu option: returns IK of 3dof RRR spherical robot arm.
 */
void calculateIK() {
    double h,L1,L2,x,y,z,θ1,θ2Up,θ2Down,θ3Up,θ3Down;
    bool isSingularity, isReachable, gotTarget = false;

    std::cout << "\n========== 3-DOF IK Calculator ==========" << std::endl;
    std::cout << "   Calculator for spherical RRR robot arm  " << std::endl;
    
    std::cout << R"(     
    z                   o  (θ3)
    ^                  / \
    |              L1 /   \ L2
    +--->x           /     \
                    /       \
              (θ2) o         >>  end effector
                   |              (x, y, z)
                   | h
                   |
              _____|_____ 
             |    (θ1)   |  
             |__about z__|   
    )" << std::endl;
    std::cout << "You will need values for the base height and link lengths to initiallize RRR robot\n";
    initializeRobot(h, L1, L2);
    
    std::cout << "Next enter values for target end effector position in xyz\n";
    
    

    while(!gotTarget){
        setEETarget(x, y, z);
        isReachable = checkReachability(h,L1,L2,x,y,z);
        if (isReachable){
            gotTarget = true;
            if (std::abs(x) > 1e-9 || std::abs(y) > 1e-9){
                isSingularity = false;
            }
            else{
                std::cout << "Important: Target pose is a singularity\n";
                std::cout << "Valid values for θ1 are therefore infinite.\n";
                isSingularity = true;
            }
        }
        else{
            std::cout << "Error: Target EE position is not reachable by robot.\n";
        }
    }

    double s = z - h;
    double r = magnitude(x, y);
    double d = magnitude(r, s);

    if(!isSingularity){
        θ1 = calculateT1(x, y);
    }
    θ3Down = calculateT3(L1, L2, d);
    θ3Up = -θ3Down;
    θ2Up = calculateT2(r, s, L1, L2, θ3Up);
    θ2Down = calculateT2(r, s, L1, L2, θ3Down);


    std::cout << "With your values, h: " << h << " L1: "<< L1 << " L2: " << L2 << "\n";
    std::cout << "The target (x, y, z): (" << x << ", " << y << ", " << z << ")\n";
    std::cout << "Elbow up position: ";
    if(isSingularity){
        std::cout << "any, ";
    }
    else{
        printf("θ1: %.2f, ", θ1 * 180/PI);
    }
    printf("θ2: %.2f, θ3: %.2f\n", θ2Up * 180/PI, θ3Up * 180/PI);

    std::cout << "Elbow down position: ";
    if(isSingularity){
        std::cout << "any, ";
    }
    else{
        printf("θ1: %.2f, ", θ1 * 180/PI);
    }
    printf("θ2: %.2f, θ3: %.2f\n", θ2Down * 180/PI, θ3Down * 180/PI);

    waitForEnter();
}

double calculateT1(double x, double y){
    return atan2(y, x);
}
double calculateT3(double L1, double L2, double d){
    //using law of cosines
    double c = (pow(d,2) - pow(L1,2) - pow(L2,2)) / (2 * L1 * L2);
    c = std::clamp(c, -1.0,1.0);
    return acos(c);
}
double calculateT2(double r, double s, double L1, double L2, double θ3){
    return atan2(s,r) - atan2(L2 * sin(θ3), L1 + L2 * cos(θ3));
}

void initializeRobot(double& h, double& L1, double& L2){
    std::cout << "Enter value for base link length(h >= 0)\n";
    h = getPositiveDouble();

    std::cout << "Enter length of first link (L1 > 0)\n";
    while(true){
        double input;
        input = getPositiveDouble();
        if (input == 0){
            std::cout << "Error: Enter a value greater than zero\n";
        }
        else{
            L1 = input;
            break;
        }
    }

    std::cout << "Enter length of second link (L2 > 0)\n";
    while(true){
        double input;
        input = getPositiveDouble();
        if (input == 0){
            std::cout << "Error: Enter a value greater than zero\n";
        }
        else{
            L2 = input;
            break;
        }
    }
}

void setEETarget(double& x, double& y, double& z){
    std::cout << "Enter value of x: \n";
    x = getDouble();
    std::cout << "Enter value of y: \n";
    y = getDouble();
    std::cout << "Enter value of z: \n";
    z = getDouble();
}

bool checkReachability(double h, double L1, double L2, double x, double y, double z){
    double minRadius = std::abs(L1 - L2);
    double maxRadius = L1 + L2;
    double targetDistanceFromShoulder =  magnitude(x, y, z - h);

    if (targetDistanceFromShoulder <= maxRadius && targetDistanceFromShoulder >= minRadius){
        return true;
    }
    else{
        return false;
    }
}