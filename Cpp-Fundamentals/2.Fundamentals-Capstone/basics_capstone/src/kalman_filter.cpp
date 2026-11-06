#include "basics_capstone/kalman_filter.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "basics_capstone/user_interface.hpp"

namespace basics_capstone
{

namespace
{

// Simulation timing and goal
constexpr double kDt = 0.001;           // time step [s]
constexpr double kDuration = 5.0;       // run length [s]
constexpr double kSpeedSetpoint = 1.0;  // target wheel speed [rad/s]

// Fixed wheel motor: speed follows the command like K / (tau s + 1)
constexpr double kMotorGain = 1.0;
constexpr double kMotorTau = 0.5;
constexpr double kSpeedDecay = 1.0 - kDt / kMotorTau;        // A in v_next = A v + B u
constexpr double kInputGain = kMotorGain * kDt / kMotorTau;  // B in v_next = A v + B u

// Fixed PID gains acting on the speed error
constexpr double kKp = 2.0;
constexpr double kKi = 1.0;
constexpr double kKd = 0.05;

// Real disturbance: random change in wheel speed each step (bumps, friction, load) [rad/s]
constexpr double kDisturbanceStd = 0.005;

constexpr double kPi = 3.14159265358979323846;
constexpr int kTableEvery = 500;  // one Table Summary row every 500 steps (0.5 s)
const char * const kCsvPath = "wheel_kalman.csv";

/// PID controller memory carried from one step to the next.
struct PidState
{
  double integral = 0.0;
  double prev_error = 0.0;
  bool first_step = true;
};

/// One PID update. The first step reuses its own error so there is no derivative kick.
double pid_update(PidState & pid, double error)
{
  if (pid.first_step) {
    pid.prev_error = error;
    pid.first_step = false;
  }
  pid.integral += error * kDt;
  const double derivative = (error - pid.prev_error) / kDt;
  pid.prev_error = error;
  return kKp * error + kKi * pid.integral + kKd * derivative;
}

/// The real wheel: angle [rad] and speed [rad/s].
struct Wheel
{
  double angle = 0.0;
  double speed = 0.0;
};

/// Move the wheel forward one step with motor command u and speed disturbance w.
void step_wheel(Wheel & wheel, double u, double w)
{
  wheel.angle += wheel.speed * kDt;
  wheel.speed = kSpeedDecay * wheel.speed + kInputGain * u + w;
}

/// Size of one encoder tick [rad].
double encoder_step(int counts_per_rev)
{
  return 2.0 * kPi / counts_per_rev;
}

/// The encoder reports the true angle rounded to the nearest tick.
double read_encoder(double angle, double step)
{
  return std::round(angle / step) * step;
}

/**
 * Kalman filter memory for the state [angle, speed].
 * P is the 2x2 covariance of the estimate's error. It is symmetric, so three
 * numbers store it: p_aa (angle), p_ss (speed) and p_as (how the two errors are linked).
 */
struct KalmanState
{
  double angle = 0.0;
  double speed = 0.0;
  double p_aa = 1.0;
  double p_as = 0.0;
  double p_ss = 1.0;
  double gain_angle = 0.0;
  double gain_speed = 0.0;
};

/**
 * One Kalman cycle: predict with the wheel model and the previous command,
 * then correct the prediction with the encoder reading.
 */
void kalman_update(KalmanState & kf, const KalmanDesign & design, double u_prev, double reading)
{
  // Predict the state: the angle moves by speed * dt, the speed follows the motor model
  const double angle_pred = kf.angle + kDt * kf.speed;
  const double speed_pred = kSpeedDecay * kf.speed + kInputGain * u_prev;

  // Predict the covariance: P = F P F^T + Q, with F = [[1, dt], [0, A]].
  // The disturbance acts on speed only, so Q is added to the speed entry only.
  const double m_aa = kf.p_aa + 2.0 * kDt * kf.p_as + kDt * kDt * kf.p_ss;
  const double m_as = kSpeedDecay * (kf.p_as + kDt * kf.p_ss);
  const double m_ss = kSpeedDecay * kSpeedDecay * kf.p_ss + design.disturbance_cov;

  // Gains: the encoder measures angle only, so both gains divide by (angle variance + R).
  // The speed gain is nonzero because angle and speed errors are linked through p_as.
  const double innovation_cov = m_aa + design.noise_cov;
  kf.gain_angle = m_aa / innovation_cov;
  kf.gain_speed = m_as / innovation_cov;

  // Correct both states with the difference between the reading and the predicted angle
  const double innovation = reading - angle_pred;
  kf.angle = angle_pred + kf.gain_angle * innovation;
  kf.speed = speed_pred + kf.gain_speed * innovation;

  // The correction makes the filter more certain: P = (I - K H) P_pred
  kf.p_aa = (1.0 - kf.gain_angle) * m_aa;
  kf.p_as = (1.0 - kf.gain_angle) * m_as;
  kf.p_ss = m_ss - kf.gain_speed * m_as;
}

}  // namespace

/**
 * Third menu option: a PID holds a wheel at 1 rad/s using an encoder.
 * Compares a perfect system, raw encoder feedback, and Kalman-filtered feedback.
 */
void simulate_kalman()
{
  std::cout << "\n========= Wheel Encoder Kalman Filter =========" << std::endl;
  std::cout <<
    R"(
  setpoint ──►(+)──► [ PID ] ──► u ──► [ Wheel ] ──► angle ──► [ Encoder ] ──┐
               ▲(-)                                                          │
               └─── speed ◄── [ tick difference  OR  Kalman filter ] ◄───────┘
)"
            << std::endl;
  std::cout << "A PID holds a wheel at " << kSpeedSetpoint << " rad/s. The encoder only measures\n";
  std::cout << "angle in whole ticks, so the speed has to be worked out from the readings.\n\n";
  std::cout << "Three runs share the same random bumps on the wheel:\n";
  std::cout << "  perfect  - the PID knows the exact speed and there are no bumps\n";
  std::cout << "  raw      - speed = (tick change) / dt, no filter\n";
  std::cout << "  kalman   - speed estimated by a Kalman filter you design\n";

  int counts_per_rev = choose_encoder();
  KalmanDesign design = get_kalman_design(counts_per_rev);
  bool exit_program = false;
  bool change_encoder = false;

  while (!exit_program) {
    std::random_device seed_source;
    const WheelRun run = run_wheel_simulation(counts_per_rev, design, seed_source());

    print_kalman_results(run, counts_per_rev, design);
    print_table_summary(run.samples);
    if (write_kalman_csv(run.samples, kCsvPath)) {
      printf("\nEvery sample was saved to %s for graphing.\n", kCsvPath);
    } else {
      printf("\nError: could not write %s.\n", kCsvPath);
    }

    prompt_kalman_continue(change_encoder, exit_program);
    if (exit_program) {
      break;
    }
    if (change_encoder) {
      counts_per_rev = choose_encoder();
    }
    design = get_kalman_design(counts_per_rev);
  }

  waitForEnter();
}

int choose_encoder()
{
  std::cout << "\n========= Choose Encoder Resolution =========" << std::endl;
  std::cout << "1. High   (4096 counts per revolution)" << std::endl;
  std::cout << "2. Medium (1024 counts per revolution)" << std::endl;
  std::cout << "3. Low    (64 counts per revolution)" << std::endl;
  std::cout << "Make a selection: ";
  const int input = getMenuChoice(1, 3);

  switch (input) {
    case 1:
      return 4096;
    case 2:
      return 1024;
    default:
      return 64;
  }
}

KalmanDesign get_kalman_design(int counts_per_rev)
{
  const double step = encoder_step(counts_per_rev);
  const double real_noise_cov = step * step / 12.0;  // variance of rounding to the nearest tick
  const double real_disturbance_cov = kDisturbanceStd * kDisturbanceStd;

  KalmanDesign design;
  printf("\n=========== Design the Kalman Filter ===========\n");
  printf("Give the filter two covariances. Typing the real values gives a well-tuned\n");
  printf("filter; other values show what happens when the filter is wrong.\n");
  printf("(Scientific notation works, e.g. 3.1e-06)\n\n");

  printf("Encoder noise covariance R [rad^2]\n");
  printf("  real value for this encoder: %.3g\n", real_noise_cov);
  printf("Enter R: ");
  design.noise_cov = getDoubleGreaterThanZero();

  printf("Wheel disturbance covariance Q [(rad/s)^2 per step]\n");
  printf("  real value for this wheel: %.3g\n", real_disturbance_cov);
  printf("Enter Q: ");
  design.disturbance_cov = getPositiveDouble();

  return design;
}

WheelRun run_wheel_simulation(int counts_per_rev, const KalmanDesign & design, unsigned int seed)
{
  const double step = encoder_step(counts_per_rev);
  std::mt19937 generator(seed);
  std::normal_distribution<double> disturbance(0.0, kDisturbanceStd);

  Wheel perfect_wheel, raw_wheel, kf_wheel;
  PidState perfect_pid, raw_pid, kf_pid;
  KalmanState kf;
  double raw_prev_reading = 0.0;
  double kf_prev_u = 0.0;

  WheelRun run;
  const int num_steps = static_cast<int>(std::lround(kDuration / kDt));
  run.samples.reserve(num_steps + 1);

  for (int i = 0; i <= num_steps; i++) {
    WheelSample sample;
    sample.t = i * kDt;

    // 1. Perfect: the PID sees the exact speed
    sample.perfect_v = perfect_wheel.speed;
    sample.perfect_u = pid_update(perfect_pid, kSpeedSetpoint - perfect_wheel.speed);

    // 2. Raw: speed is the change between two encoder readings divided by dt
    const double raw_reading = read_encoder(raw_wheel.angle, step);
    sample.raw_v_true = raw_wheel.speed;
    sample.raw_v_meas = (i == 0) ? 0.0 : (raw_reading - raw_prev_reading) / kDt;
    raw_prev_reading = raw_reading;
    sample.raw_u = pid_update(raw_pid, kSpeedSetpoint - sample.raw_v_meas);

    // 3. Kalman: speed estimated from the encoder readings and the wheel model
    const double kf_reading = read_encoder(kf_wheel.angle, step);
    kalman_update(kf, design, kf_prev_u, kf_reading);
    sample.kf_v_true = kf_wheel.speed;
    sample.kf_v_hat = kf.speed;
    sample.kf_u = pid_update(kf_pid, kSpeedSetpoint - kf.speed);
    kf_prev_u = sample.kf_u;

    run.samples.push_back(sample);

    // The raw and Kalman wheels get the same random bump, so the comparison is fair
    const double w = disturbance(generator);
    step_wheel(perfect_wheel, sample.perfect_u, 0.0);
    step_wheel(raw_wheel, sample.raw_u, w);
    step_wheel(kf_wheel, sample.kf_u, w);
  }

  run.final_gain_angle = kf.gain_angle;
  run.final_gain_speed = kf.gain_speed;
  return run;
}

void print_kalman_results(const WheelRun & run, int counts_per_rev, const KalmanDesign & design)
{
  const std::vector<WheelSample> & samples = run.samples;
  if (samples.empty()) {
    return;
  }

  // Root-mean-square sums for every comparison
  double raw_speed_sq = 0.0, kf_speed_sq = 0.0;
  double raw_reading_sq = 0.0, kf_reading_sq = 0.0;
  double raw_command_sq = 0.0, kf_command_sq = 0.0;
  for (const WheelSample & s : samples) {
    if (!std::isfinite(s.raw_v_true) || !std::isfinite(s.kf_v_true)) {
      printf("\nNo results: a run diverged (speed became infinite).\n");
      printf("Try covariances closer to the real values.\n");
      return;
    }
    raw_speed_sq += (s.raw_v_true - s.perfect_v) * (s.raw_v_true - s.perfect_v);
    kf_speed_sq += (s.kf_v_true - s.perfect_v) * (s.kf_v_true - s.perfect_v);
    raw_reading_sq += (s.raw_v_meas - s.raw_v_true) * (s.raw_v_meas - s.raw_v_true);
    kf_reading_sq += (s.kf_v_hat - s.kf_v_true) * (s.kf_v_hat - s.kf_v_true);
    raw_command_sq += (s.raw_u - s.perfect_u) * (s.raw_u - s.perfect_u);
    kf_command_sq += (s.kf_u - s.perfect_u) * (s.kf_u - s.perfect_u);
  }
  const double n = static_cast<double>(samples.size());

  printf("\n================ Wheel Speed Results ================\n");
  printf("  Encoder: %d counts/rev    Filter: R = %.3g, Q = %.3g\n\n",
    counts_per_rev, design.noise_cov, design.disturbance_cov);
  printf("  %-28s %12s %12s\n", "(RMS over the whole run)", "no filter", "kalman");
  printf("  %-28s %12.4f %12.4f  rad/s\n", "Speed error vs perfect",
    std::sqrt(raw_speed_sq / n), std::sqrt(kf_speed_sq / n));
  printf("  %-28s %12.4f %12.4f  rad/s\n", "Speed the PID sees vs real",
    std::sqrt(raw_reading_sq / n), std::sqrt(kf_reading_sq / n));
  printf("  %-28s %12.3f %12.3f\n", "Motor command vs perfect",
    std::sqrt(raw_command_sq / n), std::sqrt(kf_command_sq / n));
  printf("  %-28s angle %.4f, speed %.4f\n", "Final Kalman gains",
    run.final_gain_angle, run.final_gain_speed);
}

void print_table_summary(const std::vector<WheelSample> & samples)
{
  printf("\n============================ Table Summary ============================\n");
  printf("  %6s | %8s | %27s | %27s\n", "", "perfect", "no filter", "kalman");
  printf("  %6s | %8s | %8s %8s %9s | %8s %8s %9s\n",
    "t", "speed", "speed", "PID sees", "command", "speed", "estimate", "command");
  for (std::size_t i = 0; i < samples.size(); i += kTableEvery) {
    const WheelSample & s = samples[i];
    printf("  %6.2f | %8.4f | %8.4f %8.3f %9.2f | %8.4f %8.4f %9.3f\n",
      s.t, s.perfect_v, s.raw_v_true, s.raw_v_meas, s.raw_u, s.kf_v_true, s.kf_v_hat, s.kf_u);
  }
}

bool write_kalman_csv(const std::vector<WheelSample> & samples, const std::string & path)
{
  std::ofstream file(path);
  if (!file) {
    return false;
  }
  file.precision(8);
  file << "t,perfect_v,perfect_u,raw_v_true,raw_v_meas,raw_u,kf_v_true,kf_v_hat,kf_u\n";
  for (const WheelSample & s : samples) {
    file << s.t << ',' << s.perfect_v << ',' << s.perfect_u << ','
         << s.raw_v_true << ',' << s.raw_v_meas << ',' << s.raw_u << ','
         << s.kf_v_true << ',' << s.kf_v_hat << ',' << s.kf_u << '\n';
  }
  return static_cast<bool>(file);
}

void prompt_kalman_continue(bool & change_encoder, bool & exit_program)
{
  std::cout << "\n============= Continue? =============" << std::endl;
  std::cout << "1. Run again, same encoder, new filter design" << std::endl;
  std::cout << "2. Run again, new encoder and filter design" << std::endl;
  std::cout << "3. Exit program" << std::endl;
  std::cout << "Make a selection: ";
  const int input = getMenuChoice(1, 3);

  change_encoder = (input == 2);
  exit_program = (input == 3);
}

}  // namespace basics_capstone
