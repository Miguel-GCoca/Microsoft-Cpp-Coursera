#ifndef BASICS_CAPSTONE__PID_SIMULATOR_HPP_
#define BASICS_CAPSTONE__PID_SIMULATOR_HPP_

#include <vector>

namespace basics_capstone
{

/// Plant transfer function b0 / (a2 s^2 + a1 s + a0). a2 == 0 means first order.
struct PlantModel
{
  double a2 = 0.0;
  double a1 = 0.0;
  double a0 = 0.0;
  double b0 = 0.0;
};

struct PidGains
{
  double kp = 0.0;
  double ki = 0.0;
  double kd = 0.0;
};

/// One simulation step: time, plant output and controller effort.
struct Sample
{
  double t = 0.0;
  double y = 0.0;
  double u = 0.0;
};

/// Second menu option: interactive PID step-response simulator.
void simulate_pid();
void plot_response(const std::vector<Sample> & samples, double goal_point);
void print_results(const std::vector<Sample> & samples, double goal_point);
void prompt_continue(bool & change_plant, bool & exit_program);
PlantModel choose_plant();
PidGains get_gains();
std::vector<Sample> run_simulation(
  const PlantModel & plant, const PidGains & gains,
  double goal_point, double duration);

}  // namespace basics_capstone

#endif  // BASICS_CAPSTONE__PID_SIMULATOR_HPP_
