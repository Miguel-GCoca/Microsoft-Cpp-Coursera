#ifndef BASICS_CAPSTONE__INVERSE_KINEMATICS_
#define BASICS_CAPSTONE__INVERSE_KINEMATICS_
void calculateIK();
void initializeRobot(double& h, double& L1, double& L2);
void setEETarget(double& x, double& y, double& z);
bool checkReachability(double h, double L1, double L2, double x, double y, double z);
void calculateJacobian();
#endif //BASICS_CAPSTONE__INVERSE_KINEMATICS_