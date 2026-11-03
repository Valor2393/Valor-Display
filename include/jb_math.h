#pragma once

#include "pros/motor_group.hpp"

// get the average position of the motors in the group
double avg_pose(const pros::MotorGroup& group);
// return vector's average value (int)
double vect_avg(std::vector<int>& vect);
// return vector's average value (double)
double vect_avg(std::vector<double>& vect);

// Function to convert degrees to radians
double deg_to_rad(double degrees);
// Function to convert radians to degrees
double rad_to_deg(double radians);
// Convert rotations > 360 to 0-360
double wrap_to_360(double degrees);
// Map rotations to their appropriate 0 to 2 Pi range
// Input angle in Rad, not degrees
double angle_wrapping(double angle);
