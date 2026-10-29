#include "basics_capstone/inverse_kinematics.hpp"
#include "basics_capstone/user_interface.hpp"
#include "basics_capstone/math_utils.hpp"
#include <iostream>
#include <cmath>



/**
 * First menu option: returns IK of 3dof RRR spherical robot arm.
 */
void calculateIK() {
    std::cout << "\n========== 3-DOF IK Calculator ==========" << std::endl;
    std::cout << "   Calculator for spherical RRR robot arm  " << std::endl;
    
    double h,L1,L2,x,y,z;

    std::cout << "You will need values for the base height and link lengths to initiallize RRR robot\n";
    initializeRobot(h, L1, L2);

    std::cout << "Next enter values for target end effector position in xyz\n";
    setEETarget(x, y, z);
    
    std::cout << "With your values, h: " << h << " L1: "<< L1 << " L2: " << L2 << "\n";
    std::cout << "The target (x, y, z): (" << x << ", " << y << ", " << z << ")\n";

    bool reachable = checkReachability(h,L1,L2,x,y,z);
    if (reachable){
        std::cout << "Is reachable\n";
    }
    else{
        std::cout << "Is not reachable\n";
    }

    // TODO:
    // 3. Process the input
    // 4. Display the results
    
    waitForEnter();
}

void calculateJacobian(){

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