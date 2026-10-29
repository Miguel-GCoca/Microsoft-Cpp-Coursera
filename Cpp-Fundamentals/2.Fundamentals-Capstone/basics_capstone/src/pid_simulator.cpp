#include "basics_capstone/pid_simulator.hpp"
#include "basics_capstone/user_interface.hpp"
#include <iostream>
#include <cstdio>

/**
 * Second menu option: PID simulator that returns estimated characteristics 
 * like rise time, overshoot, time constant for a given plant transfer function.
 */
void simulatePID() {
    std::cout << "\n============== PID Simulator ==============" << std::endl;
    std::cout << "Simulation of tuning a controller for a plant\n";

    PlantModel myPlant = choosePlant();
    
    waitForEnter();
}
void plotResponse(const std::vector<Sample>& samples, double setpoint){

}
void printMetrics(const std::vector<Sample>& samples, double setpoint){

}
PlantModel choosePlant(){
    PlantModel myModel;

    std::cout << "\n========= Choose Plant Type =========" << std::endl;
    std::cout << "1. 1st Order Transfer Function" << std::endl;
    std::cout << "2. 2nd Order Transfer Function" << std::endl;
    std::cout << "3. Robot Joint with Stiffness and Damping" << std::endl;

    int input = getMenuChoice(1,3);
    double K, tau, wn, zeta, J, c, k;

    switch(input){
        case 1:
            std::cout << R"(
            ___K____
             τs + 1     Please be prepared to provide K and τ
            )";

            printf("Input a value for K\n");
            K = getDoubleGreaterThanZero();

            printf("Input a value for τ\n");
            tau = getDoubleGreaterThanZero();

            myModel.a0 = 1;
            myModel.a1 = tau;
            myModel.a2 = 0.0;
            myModel.b0 = K;

            break;
        case 2:
            std::cout << R"(
            ________K_ωn^2_______
             s^2 + 2ζωn s + ωn^2     Please be prepared to provide K, ωn and ζ
            )";

            printf("Input a value for K\n");
            K = getDoubleGreaterThanZero();

            printf("Input a value for ωn\n");
            wn = getDoubleGreaterThanZero();

            printf("Input a value for ζ\n");
            zeta = getPositiveDouble();

            myModel.a0 = wn * wn;
            myModel.a1 = 2 * zeta * wn;
            myModel.a2 = 1.0;
            myModel.b0 = K * wn * wn;
            break;
        case 3:
            std::cout << R"(
            ________1______
             Js^2 + cs + k     Please be prepared to provide J, c and k
            )";
            printf("Input a value for J\n");
            J = getDoubleGreaterThanZero();

            printf("Input a value for c\n");
            c = getPositiveDouble();

            printf("Input a value for k\n");
            k = getPositiveDouble();

            myModel.a0 = k;
            myModel.a1 = c;
            myModel.a2 = J;
            myModel.b0 = 1;
            break;
    }

    return myModel;
}
PIDGains getGains(){
    PIDGains myGains;
    return myGains;
}
std::vector<Sample> runSimulation(const PlantModel& plant, const PIDGains& gains, double setpoint, double duration){
    std::vector<Sample> myVector;
    return myVector;

}