#include "basics_capstone/pid_simulator.hpp"
#include "basics_capstone/user_interface.hpp"
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>

/**
 * Second menu option: PID simulator that returns estimated characteristics 
 * like rise time, overshoot, time constant for a given plant transfer function.
 */
void simulatePID() {
    std::cout << "\n============== PID Simulator ==============" << std::endl;

    std::cout << R"(

    reference r ──►(+)──► error e ──► [ PID ] ──► u ──► [ Plant ] ──┬──► y
                    ▲(-)                                            │
                    └───────────────────────────────────────────────┘
)" << std::endl;
    std::cout << "This simulation lets you tune PID gains for a controller for a plant.\n\n";
    std::cout << "The program will output an ascii graph of the step response\n";
    std::cout << "and a table with relevant control stats like rise time and overshoot.\n";

    PlantModel myPlant = choosePlant();
    PIDGains myGains = getGains();

    double goalPoint = 1.0;
    double duration = 5.0;
    bool exitProgram = false;
    bool changePlant = false;
    
    while(!exitProgram){
        auto responseVector = runSimulation(myPlant, myGains, goalPoint, duration);

        plotResponse(responseVector, goalPoint);
        printResults(responseVector, goalPoint);
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

/**
 * Returns the index of the first sample where the response has diverged:
 * y became inf/nan, or |y| grew past divergenceFactor times the setpoint.
 * Returns samples.size() if the response stayed bounded the whole run.
 */
static std::size_t findDivergence(const std::vector<Sample>& samples, double goalPoint){
    constexpr double divergenceFactor = 10.0;
    const double bound = divergenceFactor * std::max(1.0, std::fabs(goalPoint));

    for (std::size_t i = 0; i < samples.size(); i++){
        if (!std::isfinite(samples[i].y) || std::fabs(samples[i].y) > bound){
            return i;
        }
    }
    return samples.size();
}

void plotResponse(const std::vector<Sample>& samples, double goalPoint){
    constexpr int rows = 25;    // number of plotted rows (time runs down the page)
    constexpr int width = 50;   // number of columns (y runs left to right)

    // Only plot the samples before the response diverged (all of them if it didn't)
    const std::size_t end = findDivergence(samples, goalPoint);
    if (end == 0){
        printf("\nThe response diverged immediately, so there is nothing to plot.\n");
        return;
    }

    // Autoscale: the range must contain 0, the setpoint, and every plotted sample
    double lo = std::min(0.0, goalPoint);
    double hi = std::max(0.0, goalPoint);
    for (std::size_t i = 0; i < end; i++){
        lo = std::min(lo, samples[i].y);
        hi = std::max(hi, samples[i].y);
    }
    if (hi - lo < 1e-9){    // completely flat plot: give it a visible range
        hi = lo + 1.0;
    }

    // Map a value in [lo, hi] to a column in [0, width - 1]
    auto toColumn = [lo, hi](double value){
        double fraction = (value - lo) / (hi - lo);
        return static_cast<int>(std::lround(fraction * (width - 1)));
    };

    // Decimate: plot every stride-th sample
    std::size_t stride = (end - 1) / rows;
    if (stride == 0){
        stride = 1;
    }

    printf("\n%7s %8s  %-8.3f%*s%8.3f\n", "t", "y", lo, width - 16, "", hi);
    for (std::size_t i = 0; i < end; i += stride){
        std::string line(width, ' ');
        line[toColumn(goalPoint)] = '|';      // setpoint first...
        line[toColumn(samples[i].y)] = '*';   // ...so y wins if they overlap
        printf("%7.3f %8.4f  %s\n", samples[i].t, samples[i].y, line.c_str());
    }
    if (end < samples.size()){
        printf("\n  The response diverged at t = %.3f s, so the plot stops there.\n", samples[end].t);
    }
}

void printResults(const std::vector<Sample>& samples, double goalPoint){
    if (samples.empty()){
        return;
    }
    if (findDivergence(samples, goalPoint) < samples.size()){
        printf("\nNo results: the response diverged, so step-response metrics don't apply.\n");
        return;
    }

    constexpr double pi = 3.14159265358979323846;

    // 1. Final value and steady-state error
    const double finalValue = samples.back().y;
    const double steadyStateError = goalPoint - finalValue;

    printf("\n=========== Step Response Results ===========\n");
    printf("  %-24s %9.4f\n", "Final value", finalValue);
    printf("  %-24s %9.4f\n", "Steady-state error", steadyStateError);

    // Every metric below is relative to finalValue, so stop if it is ~0
    if (std::fabs(finalValue) < 1e-9){
        printf("  (Final value is ~0, so the remaining metrics are undefined.)\n");
        return;
    }

    // 2. Peak and peak time: index of the largest y
    std::size_t peakIndex = 0;
    for (std::size_t i = 1; i < samples.size(); i++){
        if (samples[i].y > samples[peakIndex].y){
            peakIndex = i;
        }
    }
    const double peak = samples[peakIndex].y;
    const double peakTime = samples[peakIndex].t;

    // 3. Overshoot: how far the peak goes past the final value
    const double overshoot = std::max(0.0, (peak - finalValue) / finalValue * 100.0);

    // 4. Undershoot: deepest dip below the final value after the peak
    double minAfterPeak = peak;
    for (std::size_t i = peakIndex; i < samples.size(); i++){
        minAfterPeak = std::min(minAfterPeak, samples[i].y);
    }
    const double undershoot = std::max(0.0, (finalValue - minAfterPeak) / finalValue * 100.0);

    // 5. Rise time: first crossings of 10% and 90% of the final value
    double t10 = -1.0, t90 = -1.0;
    for (const Sample& s : samples){
        if (t10 < 0.0 && s.y >= 0.1 * finalValue){
            t10 = s.t;
        }
        if (s.y >= 0.9 * finalValue){
            t90 = s.t;
            break;
        }
    }

    // 6. Settling time: scan backward for the last sample outside the 2% band.
    //    The sample after it is where y entered the band for good.
    const double band = 0.02 * std::fabs(finalValue);
    double settlingTime = 0.0;
    for (std::size_t i = samples.size(); i-- > 0; ){
        if (std::fabs(samples[i].y - finalValue) > band){
            settlingTime = samples[i + 1].t;
            break;
        }
    }

    printf("  %-24s %9.4f   (at t = %.3f s)\n", "Peak", peak, peakTime);
    printf("  %-24s %9.2f %%\n", "Overshoot", overshoot);
    printf("  %-24s %9.2f %%\n", "Undershoot", undershoot);
    if (t90 >= 0.0){
        printf("  %-24s %9.3f s\n", "Rise time (10-90%)", t90 - t10);
    } else {
        printf("  %-24s %9s\n", "Rise time (10-90%)", "n/a");
    }
    printf("  %-24s %9.3f s\n", "Settling time (2%)", settlingTime);

    // 7. Closed-loop zeta and wn, estimated from overshoot and peak time
    if (overshoot > 0.01){
        const double L = std::log(overshoot / 100.0);
        const double zeta = -L / std::sqrt(pi * pi + L * L);
        const double wn = pi / (peakTime * std::sqrt(1.0 - zeta * zeta));
        printf("  %-24s %9.4f\n", "Est. closed-loop zeta", zeta);
        printf("  %-24s %9.4f rad/s\n", "Est. closed-loop wn", wn);
    } else {
        printf("  (No overshoot, so zeta and wn can't be estimated.)\n");
    }
}

std::vector<Sample> runSimulation(const PlantModel& plant, const PIDGains& gains, double goalPoint, double duration){
    std::vector<Sample> myVector;
    Sample iteration;
    constexpr double dt = 0.001;
    int numSteps = duration/dt;
    double y = 0.0, yPrime = 0.0, yDoublePrime = 0.0;
    double totalError, derivativeError, proportionalError, integralError = 0.0;
    double prevError = goalPoint - y; //prevent infinitley large initial change
    
    for (int i = 0; i <= numSteps; i++){
        iteration.t = i * dt;
        iteration.y = y;
        
        //calculate total error
        totalError = goalPoint - y;

        //calculate Integral error estimate
        integralError = integralError + totalError * dt;

        //Calculate Derivative error estimate
        derivativeError = (totalError - prevError) / dt;
        prevError = totalError;

        iteration.u = gains.Kp * totalError + gains.Ki * integralError + gains.Kd * derivativeError;

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
    printf("\nNext provide values for Kp, Ki, Kd.\nA value of zero disables the term.\n");

    printf("Enter proportional gain Kp: ");
    myGains.Kp = getPositiveDouble();

    printf("Enter integral gain Ki: ");
    myGains.Ki = getPositiveDouble();

    printf("Enter derivative gain Kd: ");
    myGains.Kd = getPositiveDouble();

    return myGains;
}
