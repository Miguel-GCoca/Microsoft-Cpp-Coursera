#include "basics_capstone/inverse_kinematics.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "basics_capstone/math_utils.hpp"
#include "basics_capstone/user_interface.hpp"

namespace basics_capstone
{

const double kPi = std::acos(-1.0);

void calculate_ik()
{
  double h, l1, l2, x, y, z, theta1, theta2_up, theta2_down, theta3_up, theta3_down;
  bool is_singularity, is_reachable, got_target = false;

  std::cout << "\n========== 3-DOF IK Calculator ==========" << std::endl;
  std::cout << "   Calculator for spherical RRR robot arm  " << std::endl;

  std::cout <<
    R"(
    z                   o  (θ3)
    ^                  / \
    |              L1 /   \ L2
    +--->x           /     \
                    /       \
              (θ2) o         >>  end effector
                   |              (x, y, z)
                   | h
                   |
              _____|_____
             |    (θ1)   |
             |__about z__|
    )"
            << std::endl;
  std::cout <<
    "You will need values for the base height and link lengths to initiallize RRR robot\n";
  initialize_robot(h, l1, l2);

  std::cout << "Next enter values for target end effector position in xyz\n";

  while (!got_target) {
    set_ee_target(x, y, z);
    is_reachable = check_reachability(h, l1, l2, x, y, z);
    if (is_reachable) {
      got_target = true;
      if (std::abs(x) > 1e-9 || std::abs(y) > 1e-9) {
        is_singularity = false;
      } else {
        std::cout << "Important: Target pose is a singularity\n";
        std::cout << "Valid values for θ1 are therefore infinite.\n";
        is_singularity = true;
      }
    } else {
      std::cout << "Error: Target EE position is not reachable by robot.\n";
    }
  }

  double s = z - h;
  double r = magnitude(x, y);
  double d = magnitude(r, s);

  if (!is_singularity) {
    theta1 = calculate_theta1(x, y);
  }
  theta3_down = calculate_theta3(l1, l2, d);
  theta3_up = -theta3_down;
  theta2_up = calculate_theta2(r, s, l1, l2, theta3_up);
  theta2_down = calculate_theta2(r, s, l1, l2, theta3_down);

  std::cout << "With your values, h: " << h << " L1: " << l1 << " L2: " << l2 << "\n";
  std::cout << "The target (x, y, z): (" << x << ", " << y << ", " << z << ")\n";
  std::cout << "Elbow up position: ";
  if (is_singularity) {
    std::cout << "any, ";
  } else {
    printf("θ1: %.2f, ", theta1 * 180 / kPi);
  }
  printf("θ2: %.2f, θ3: %.2f\n", theta2_up * 180 / kPi, theta3_up * 180 / kPi);

  std::cout << "Elbow down position: ";
  if (is_singularity) {
    std::cout << "any, ";
  } else {
    printf("θ1: %.2f, ", theta1 * 180 / kPi);
  }
  printf("θ2: %.2f, θ3: %.2f\n", theta2_down * 180 / kPi, theta3_down * 180 / kPi);

  waitForEnter();
}

double calculate_theta1(double x, double y)
{
  return atan2(y, x);
}

double calculate_theta3(double l1, double l2, double d)
{
  // using law of cosines
  double c = (pow(d, 2) - pow(l1, 2) - pow(l2, 2)) / (2 * l1 * l2);
  c = std::clamp(c, -1.0, 1.0);
  return acos(c);
}

double calculate_theta2(double r, double s, double l1, double l2, double theta3)
{
  return atan2(s, r) - atan2(l2 * sin(theta3), l1 + l2 * cos(theta3));
}

void initialize_robot(double & h, double & l1, double & l2)
{
  std::cout << "Enter value for base link length(h >= 0)\n";
  h = getPositiveDouble();

  std::cout << "Enter length of first link (L1 > 0)\n";
  while (true) {
    double input;
    input = getPositiveDouble();
    if (input == 0) {
      std::cout << "Error: Enter a value greater than zero\n";
    } else {
      l1 = input;
      break;
    }
  }

  std::cout << "Enter length of second link (L2 > 0)\n";
  while (true) {
    double input;
    input = getPositiveDouble();
    if (input == 0) {
      std::cout << "Error: Enter a value greater than zero\n";
    } else {
      l2 = input;
      break;
    }
  }
}

void set_ee_target(double & x, double & y, double & z)
{
  std::cout << "Enter value of x: \n";
  x = getDouble();
  std::cout << "Enter value of y: \n";
  y = getDouble();
  std::cout << "Enter value of z: \n";
  z = getDouble();
}

bool check_reachability(double h, double l1, double l2, double x, double y, double z)
{
  double min_radius = std::abs(l1 - l2);
  double max_radius = l1 + l2;
  double target_distance_from_shoulder = magnitude(x, y, z - h);

  if (target_distance_from_shoulder <= max_radius &&
    target_distance_from_shoulder >= min_radius)
  {
    return true;
  } else {
    return false;
  }
}

}  // namespace basics_capstone
