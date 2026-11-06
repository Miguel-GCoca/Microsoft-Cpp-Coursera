#include "basics_capstone/pid_simulator.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "basics_capstone/user_interface.hpp"

namespace basics_capstone
{

/**
 * Second menu option: PID simulator that returns estimated characteristics
 * like rise time, overshoot, time constant for a given plant transfer function.
 */
void simulate_pid()
{
  std::cout << "\n============== PID Simulator ==============" << std::endl;

  std::cout <<
    R"(

    reference r ──►(+)──► error e ──► [ PID ] ──► u ──► [ Plant ] ──┬──► y
                    ▲(-)                                            │
                    └───────────────────────────────────────────────┘
)"
            << std::endl;
  std::cout << "This simulation lets you tune PID gains for a controller for a plant.\n\n";
  std::cout << "The program will output an ascii graph of the step response\n";
  std::cout << "and a table with relevant control stats like rise time and overshoot.\n";

  PlantModel my_plant = choose_plant();
  PidGains my_gains = get_gains();

  double goal_point = 1.0;
  double duration = 5.0;
  bool exit_program = false;
  bool change_plant = false;

  while (!exit_program) {
    auto response_vector = run_simulation(my_plant, my_gains, goal_point, duration);

    plot_response(response_vector, goal_point);
    print_results(response_vector, goal_point);
    prompt_continue(change_plant, exit_program);
    if (exit_program) {
      break;
    }
    if (change_plant) {
      my_plant = choose_plant();
    }
    my_gains = get_gains();
  }

  waitForEnter();
}

void prompt_continue(bool & change_plant, bool & exit_program)
{
  std::cout << "\n============= Continue? =============" << std::endl;
  std::cout << "1. Continue, with same transfer function" << std::endl;
  std::cout << "2. Continue, with new transfer function" << std::endl;
  std::cout << "3. Exit program" << std::endl;
  std::cout << "Make a selection: ";
  int input = getMenuChoice(1, 3);

  switch (input) {
    case 1:
      change_plant = false;
      exit_program = false;
      break;
    case 2:
      change_plant = true;
      exit_program = false;
      break;
    case 3:
      change_plant = false;
      exit_program = true;
      break;
  }
}

/**
 * Returns the index of the first sample where the response has diverged:
 * y became inf/nan, or |y| grew past kDivergenceFactor times the setpoint.
 * Returns samples.size() if the response stayed bounded the whole run.
 */
static std::size_t find_divergence(const std::vector<Sample> & samples, double goal_point)
{
  constexpr double kDivergenceFactor = 10.0;
  const double bound = kDivergenceFactor * std::max(1.0, std::fabs(goal_point));

  for (std::size_t i = 0; i < samples.size(); i++) {
    if (!std::isfinite(samples[i].y) || std::fabs(samples[i].y) > bound) {
      return i;
    }
  }
  return samples.size();
}

void plot_response(const std::vector<Sample> & samples, double goal_point)
{
  constexpr int kRows = 25;    // number of plotted rows (time runs down the page)
  constexpr int kWidth = 50;   // number of columns (y runs left to right)

  // Only plot the samples before the response diverged (all of them if it didn't)
  const std::size_t end = find_divergence(samples, goal_point);
  if (end == 0) {
    printf("\nThe response diverged immediately, so there is nothing to plot.\n");
    return;
  }

  // Autoscale: the range must contain 0, the setpoint, and every plotted sample
  double lo = std::min(0.0, goal_point);
  double hi = std::max(0.0, goal_point);
  for (std::size_t i = 0; i < end; i++) {
    lo = std::min(lo, samples[i].y);
    hi = std::max(hi, samples[i].y);
  }
  if (hi - lo < 1e-9) {    // completely flat plot: give it a visible range
    hi = lo + 1.0;
  }

  // Map a value in [lo, hi] to a column in [0, kWidth - 1]
  auto to_column = [lo, hi](double value) {
      double fraction = (value - lo) / (hi - lo);
      return static_cast<int>(std::lround(fraction * (kWidth - 1)));
    };

  // Decimate: plot every stride-th sample
  std::size_t stride = (end - 1) / kRows;
  if (stride == 0) {
    stride = 1;
  }

  printf("\n%7s %8s  %-8.3f%*s%8.3f\n", "t", "y", lo, kWidth - 16, "", hi);
  for (std::size_t i = 0; i < end; i += stride) {
    std::string line(kWidth, ' ');
    line[to_column(goal_point)] = '|';      // setpoint first...
    line[to_column(samples[i].y)] = '*';    // ...so y wins if they overlap
    printf("%7.3f %8.4f  %s\n", samples[i].t, samples[i].y, line.c_str());
  }
  if (end < samples.size()) {
    printf(
      "\n  The response diverged at t = %.3f s, so the plot stops there.\n",
      samples[end].t);
  }
}

void print_results(const std::vector<Sample> & samples, double goal_point)
{
  if (samples.empty()) {
    return;
  }
  if (find_divergence(samples, goal_point) < samples.size()) {
    printf("\nNo results: the response diverged, so step-response metrics don't apply.\n");
    return;
  }

  constexpr double kPi = 3.14159265358979323846;

  // 1. Final value and steady-state error
  const double final_value = samples.back().y;
  const double steady_state_error = goal_point - final_value;

  printf("\n=========== Step Response Results ===========\n");
  printf("  %-24s %9.4f\n", "Final value", final_value);
  printf("  %-24s %9.4f\n", "Steady-state error", steady_state_error);

  // Every metric below is relative to final_value, so stop if it is ~0
  if (std::fabs(final_value) < 1e-9) {
    printf("  (Final value is ~0, so the remaining metrics are undefined.)\n");
    return;
  }

  // 2. Peak and peak time: index of the largest y
  std::size_t peak_index = 0;
  for (std::size_t i = 1; i < samples.size(); i++) {
    if (samples[i].y > samples[peak_index].y) {
      peak_index = i;
    }
  }
  const double peak = samples[peak_index].y;
  const double peak_time = samples[peak_index].t;

  // 3. Overshoot: how far the peak goes past the final value
  const double overshoot = std::max(0.0, (peak - final_value) / final_value * 100.0);

  // 4. Undershoot: deepest dip below the final value after the peak
  double min_after_peak = peak;
  for (std::size_t i = peak_index; i < samples.size(); i++) {
    min_after_peak = std::min(min_after_peak, samples[i].y);
  }
  const double undershoot = std::max(0.0, (final_value - min_after_peak) / final_value * 100.0);

  // 5. Rise time: first crossings of 10% and 90% of the final value
  double t10 = -1.0, t90 = -1.0;
  for (const Sample & s : samples) {
    if (t10 < 0.0 && s.y >= 0.1 * final_value) {
      t10 = s.t;
    }
    if (s.y >= 0.9 * final_value) {
      t90 = s.t;
      break;
    }
  }

  // 6. Settling time: scan backward for the last sample outside the 2% band.
  //    The sample after it is where y entered the band for good.
  const double band = 0.02 * std::fabs(final_value);
  double settling_time = 0.0;
  for (std::size_t i = samples.size(); i-- > 0; ) {
    if (std::fabs(samples[i].y - final_value) > band) {
      settling_time = samples[i + 1].t;
      break;
    }
  }

  printf("  %-24s %9.4f   (at t = %.3f s)\n", "Peak", peak, peak_time);
  printf("  %-24s %9.2f %%\n", "Overshoot", overshoot);
  printf("  %-24s %9.2f %%\n", "Undershoot", undershoot);
  if (t90 >= 0.0) {
    printf("  %-24s %9.3f s\n", "Rise time (10-90%)", t90 - t10);
  } else {
    printf("  %-24s %9s\n", "Rise time (10-90%)", "n/a");
  }
  printf("  %-24s %9.3f s\n", "Settling time (2%)", settling_time);

  // 7. Closed-loop zeta and wn, estimated from overshoot and peak time
  if (overshoot > 0.01) {
    const double log_overshoot = std::log(overshoot / 100.0);
    const double zeta = -log_overshoot / std::sqrt(kPi * kPi + log_overshoot * log_overshoot);
    const double wn = kPi / (peak_time * std::sqrt(1.0 - zeta * zeta));
    printf("  %-24s %9.4f\n", "Est. closed-loop zeta", zeta);
    printf("  %-24s %9.4f rad/s\n", "Est. closed-loop wn", wn);
  } else {
    printf("  (No overshoot, so zeta and wn can't be estimated.)\n");
  }
}

std::vector<Sample> run_simulation(
  const PlantModel & plant, const PidGains & gains,
  double goal_point, double duration)
{
  std::vector<Sample> my_vector;
  Sample iteration;
  constexpr double kDt = 0.001;
  int num_steps = duration / kDt;
  double y = 0.0, y_prime = 0.0, y_double_prime = 0.0;
  double total_error, derivative_error, proportional_error, integral_error = 0.0;
  double prev_error = goal_point - y;  // prevent infinitley large initial change

  for (int i = 0; i <= num_steps; i++) {
    iteration.t = i * kDt;
    iteration.y = y;

    // calculate total error
    total_error = goal_point - y;

    // calculate Integral error estimate
    integral_error = integral_error + total_error * kDt;

    // Calculate Derivative error estimate
    derivative_error = (total_error - prev_error) / kDt;
    prev_error = total_error;

    iteration.u = gains.kp * total_error + gains.ki * integral_error +
      gains.kd * derivative_error;

    my_vector.push_back(iteration);

    if (plant.a2 == 0) {  // first order transfer function
      y_prime = (plant.b0 * iteration.u - plant.a0 * y) / plant.a1;
      y = y + y_prime * kDt;
    } else {  // second order transfer function
      y_double_prime = (plant.b0 * iteration.u - plant.a1 * y_prime - plant.a0 * y) / plant.a2;
      y_prime = y_prime + y_double_prime * kDt;
      y = y + y_prime * kDt;
    }
  }

  return my_vector;
}

PlantModel choose_plant()
{
  PlantModel my_model;

  std::cout << "\n========= Choose Plant Type =========" << std::endl;
  std::cout << "1. 1st Order Transfer Function" << std::endl;
  std::cout << "2. 2nd Order Transfer Function" << std::endl;
  std::cout << "3. Robot Joint with Stiffness and Damping" << std::endl;
  std::cout << "Make a selection: ";
  int input = getMenuChoice(1, 3);
  double gain, tau, wn, zeta, inertia, damping, stiffness;

  switch (input) {
    case 1:
      std::cout << R"(
    ___K___     Please provide K and τ
    τs + 1
    )";

      printf("\nInput a value for K: ");
      gain = getDoubleGreaterThanZero();

      printf("Input a value for τ: ");
      tau = getDoubleGreaterThanZero();

      my_model.a0 = 1;
      my_model.a1 = tau;
      my_model.a2 = 0.0;
      my_model.b0 = gain;

      break;
    case 2:
      std::cout <<
        R"(
    ______K_ωn^2_______     Please provide K, ωn and ζ
    s^2 + 2ζωn s + ωn^2
    )";

      printf("\nInput a value for K: ");
      gain = getDoubleGreaterThanZero();

      printf("Input a value for ωn: ");
      wn = getDoubleGreaterThanZero();

      printf("Input a value for ζ: ");
      zeta = getPositiveDouble();

      my_model.a0 = wn * wn;
      my_model.a1 = 2 * zeta * wn;
      my_model.a2 = 1.0;
      my_model.b0 = gain * wn * wn;
      break;
    case 3:
      std::cout << R"(
    ______1______     Please provide J, c and k
    Js^2 + cs + k
    )";
      printf("\nInput a value for J: ");
      inertia = getDoubleGreaterThanZero();

      printf("Input a value for c: ");
      damping = getPositiveDouble();

      printf("Input a value for k: ");
      stiffness = getPositiveDouble();

      my_model.a0 = stiffness;
      my_model.a1 = damping;
      my_model.a2 = inertia;
      my_model.b0 = 1;
      break;
  }

  return my_model;
}

PidGains get_gains()
{
  PidGains my_gains;
  printf("\nNext provide values for Kp, Ki, Kd.\nA value of zero disables the term.\n");

  printf("Enter proportional gain Kp: ");
  my_gains.kp = getPositiveDouble();

  printf("Enter integral gain Ki: ");
  my_gains.ki = getPositiveDouble();

  printf("Enter derivative gain Kd: ");
  my_gains.kd = getPositiveDouble();

  return my_gains;
}

}  // namespace basics_capstone
