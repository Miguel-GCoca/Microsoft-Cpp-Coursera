#ifndef BASICS_CAPSTONE__INVERSE_KINEMATICS_HPP_
#define BASICS_CAPSTONE__INVERSE_KINEMATICS_HPP_

namespace basics_capstone
{

/// First menu option: prints the IK solutions of a 3-DOF spherical RRR arm.
void calculate_ik();

/// Prompts for the base link length and both link lengths.
void initialize_robot(double & h, double & l1, double & l2);

/// Prompts for the end effector target position.
void set_ee_target(double & x, double & y, double & z);

/// Returns true if the target lies inside the arm's spherical-shell workspace.
bool check_reachability(double h, double l1, double l2, double x, double y, double z);

/// Base rotation about z.
double calculate_theta1(double x, double y);

/// Elbow angle from the law of cosines (elbow down solution).
double calculate_theta3(double l1, double l2, double d);

/// Shoulder angle for a given elbow angle.
double calculate_theta2(double r, double s, double l1, double l2, double theta3);

}  // namespace basics_capstone

#endif  // BASICS_CAPSTONE__INVERSE_KINEMATICS_HPP_
