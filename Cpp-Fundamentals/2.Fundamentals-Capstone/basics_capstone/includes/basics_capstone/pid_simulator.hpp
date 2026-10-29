#ifndef BASICS_CAPSTONE__PID_SIMULATOR_
#define BASICS_CAPSTONE__PID_SIMULATOR_
#include <vector>
struct PlantModel{
    double a2 = 0.0;
    double a1 = 0.0;
    double a0 = 0.0;
    double b0 = 0.0; 
};
struct PIDGains{
    double Kp = 0.0;
    double Ki = 0.0;
    double Kd = 0.0; 
};
struct Sample{
    double t = 0.0;
    double y = 0.0;
    double u = 0.0;
};

void simulatePID();
void plotResponse(const std::vector<Sample>& samples, double setpoint);
void printMetrics(const std::vector<Sample>& samples, double setpoint);
PlantModel choosePlant();
PIDGains getGains();
std::vector<Sample> runSimulation(const PlantModel& plant, const PIDGains& gains,
                                  double setpoint, double duration);

#endif //BASICS_CAPSTONE__PID_SIMULATOR_