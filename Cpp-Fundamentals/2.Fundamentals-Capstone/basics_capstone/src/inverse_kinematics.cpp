#include "basics_capstone/inverse_kinematics.hpp"
#include "basics_capstone/user_interface.hpp"
#include <iostream>
#include <cmath>


/**
 * First function - [Main function for IK calculator]
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
    std::cout << "The target (x, y, z): (" << x << ", " << y << ", " << z << ")";

    bool reachable = checkReachability(h,L1,L2,x,y,z);
    if (reachable){
        std::cout << "Is reachable\n";
    }
    else{
        std::cout << "Is not reachable\n";
    }

    // TODO: Implement your first utility function
    // 3. Process the input
    // 4. Display the results
    
    waitForEnter();
}

void calculateJacobian(){

}

void initializeRobot(double& h, double& L1, double& L2){
    std::cout << "Enter value for base link length(H >= 0)\n";
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

    std::cout << "Enter length of first link (L2 > 0)\n";
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
    std::cout << "Enter value of of x: \n";
    x = getDouble();
    std::cout << "Enter value of of y: \n";
    y = getDouble();
    std::cout << "Enter value of of z: \n";
    z = getDouble();
}
//TODO: fix bad rachability math.
bool checkReachability(double h, double L1, double L2, double x, double y, double z){
    if (magnitude(h, L1, L2) >= magnitude(x,y,z)){
        return true;
    }
    else{
        return false;
    }
}

double magnitude(double x = 0.0, double y = 0.0, double z = 0.0){
    return sqrt((pow(x,2) + pow(y,2) + pow(z,2)));
}