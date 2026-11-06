#ifndef BASICS_CAPSTONE__KALMAN_FILTER_HPP_
#define BASICS_CAPSTONE__KALMAN_FILTER_HPP_

#include <string>
#include <vector>

namespace basics_capstone
{

/// The filter's design: the covariances the user believes describe the wheel and encoder.
struct KalmanDesign
{
  double disturbance_cov = 0.0;  // Q: variance of the random speed change per step [(rad/s)^2]
  double noise_cov = 0.0;        // R: variance of the encoder reading error [rad^2]
};

/// One time step of the three side-by-side runs: perfect, no filter, and Kalman.
struct WheelSample
{
  double t = 0.0;
  double perfect_v = 0.0;   // speed of the perfect system (no disturbance, exact speed)
  double perfect_u = 0.0;   // motor command of the perfect system
  double raw_v_true = 0.0;  // real speed when the PID uses raw encoder differences
  double raw_v_meas = 0.0;  // speed the raw PID sees (tick difference / dt)
  double raw_u = 0.0;       // motor command with raw feedback
  double kf_v_true = 0.0;   // real speed when the PID uses the Kalman estimate
  double kf_v_hat = 0.0;    // Kalman speed estimate
  double kf_u = 0.0;        // motor command with Kalman feedback
};

/// A whole run: every sample plus the Kalman gains at the end.
struct WheelRun
{
  std::vector<WheelSample> samples;
  double final_gain_angle = 0.0;
  double final_gain_speed = 0.0;
};

/// Third menu option: wheel speed control with and without a Kalman filter.
void simulate_kalman();
int choose_encoder();
KalmanDesign get_kalman_design(int counts_per_rev);
WheelRun run_wheel_simulation(
  int counts_per_rev, const KalmanDesign & design, unsigned int seed);
void print_kalman_results(
  const WheelRun & run, int counts_per_rev, const KalmanDesign & design);
void print_table_summary(const std::vector<WheelSample> & samples);
bool write_kalman_csv(const std::vector<WheelSample> & samples, const std::string & path);
void prompt_kalman_continue(bool & change_encoder, bool & exit_program);

}  // namespace basics_capstone

#endif  // BASICS_CAPSTONE__KALMAN_FILTER_HPP_
