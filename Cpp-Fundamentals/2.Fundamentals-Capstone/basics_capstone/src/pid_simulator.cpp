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
    PIDGains myGains = getGains();

    double stepInput = 1.0;
    double duration = 5.0;
    bool exitProgram = false;
    bool changePlant = false;
    
    while(!exitProgram){
        auto responseVector = runSimulation(myPlant, myGains, stepInput, duration);

        plotResponse(responseVector, stepInput);
        printResults(responseVector, stepInput);
        promptContinue(changePlant, exitProgram);
        if (exitProgram){
            break;
        }
        if (changePlant){
            myPlant = choosePlant();
        }
        myGains = getGains();
    }

    waitForEnter();
}

void promptContinue(bool& changePlant, bool& exitProgram){
    std::cout << "\n============= Continue? =============" << std::endl;
    std::cout << "1. Continue, with same transfer function" << std::endl;
    std::cout << "2. Continue, with new transfer function" << std::endl;
    std::cout << "3. Exit program" << std::endl;
    std::cout << "Make a selection: ";
    int input = getMenuChoice(1,3);

    switch(input){
        case 1:
            changePlant = false;
            exitProgram = false;
            break;
        case 2:
            changePlant = true;
            exitProgram = false;
            break;
        case 3:
            changePlant = false;
            exitProgram = true;
            break;
    }
}

void plotResponse(const std::vector<Sample>& samples, double stepInput){
    printf("\nresponse plotter here\n");
}
void printResults(const std::vector<Sample>& samples, double stepInput){
    printf("results print here\n");
}

std::vector<Sample> runSimulation(const PlantModel& plant, const PIDGains& gains, double stepInput, double duration){
    std::vector<Sample> myVector;
    constexpr double dt = 0.001;
    int numSteps = duration/dt;
    double y = 0.0, yPrime = 0.0, yDoublePrime = 0.0;
    Sample iteration;

    for (int i = 0; i <= numSteps; i++){
        iteration.u = stepInput;
        iteration.t = i * dt;
        iteration.y = y;

        myVector.push_back(iteration);

        if(plant.a2 ==0){ // first order transfer function
            yPrime = (plant.b0 * iteration.u - plant.a0 * y) / plant.a1;
            y = y + yPrime * dt;
        }
        else{ //second order transfer function
            yDoublePrime = (plant.b0 * iteration.u - plant.a1 * yPrime - plant.a0 * y) / plant.a2;
            yPrime = yPrime + yDoublePrime * dt;
            y = y + yPrime * dt;
        }
    }

    return myVector;

}

PlantModel choosePlant(){
    PlantModel myModel;

    std::cout << "\n========= Choose Plant Type =========" << std::endl;
    std::cout << "1. 1st Order Transfer Function" << std::endl;
    std::cout << "2. 2nd Order Transfer Function" << std::endl;
    std::cout << "3. Robot Joint with Stiffness and Damping" << std::endl;
    std::cout << "Make a selection: ";
    int input = getMenuChoice(1,3);
    double K, tau, wn, zeta, J, c, k;

    switch(input){
        case 1:
            std::cout << R"(
    ___K___     Please provide K and τ
    τs + 1 
    )";

            printf("\nInput a value for K: ");
            K = getDoubleGreaterThanZero();

            printf("Input a value for τ: ");
            tau = getDoubleGreaterThanZero();

            myModel.a0 = 1;
            myModel.a1 = tau;
            myModel.a2 = 0.0;
            myModel.b0 = K;

            break;
        case 2:
            std::cout << R"(
    ______K_ωn^2_______     Please provide K, ωn and ζ
    s^2 + 2ζωn s + ωn^2
    )";

            printf("\nInput a value for K: ");
            K = getDoubleGreaterThanZero();

            printf("Input a value for ωn: ");
            wn = getDoubleGreaterThanZero();

            printf("Input a value for ζ: ");
            zeta = getPositiveDouble();

            myModel.a0 = wn * wn;
            myModel.a1 = 2 * zeta * wn;
            myModel.a2 = 1.0;
            myModel.b0 = K * wn * wn;
            break;
        case 3:
            std::cout << R"(
    ______1______     Please provide J, c and k
    Js^2 + cs + k     
    )";
            printf("\nInput a value for J: ");
            J = getDoubleGreaterThanZero();

            printf("Input a value for c: ");
            c = getPositiveDouble();

            printf("Input a value for k: ");
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
    printf("\nNext provide values for Kp, Kd, Ki.\nA value of zero disables the term.\n");

    printf("Enter proportional gain Kp: ");
    myGains.Kp = getPositiveDouble();

    printf("Enter integral gain Ki: ");
    myGains.Ki = getPositiveDouble();

    printf("Enter derivative gain Kd: ");
    myGains.Kd = getPositiveDouble();

    return myGains;
}
