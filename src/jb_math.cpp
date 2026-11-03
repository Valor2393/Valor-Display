#include "jb_math.h"
#include <cmath>


double avg_pose(const pros::MotorGroup& group) {
	double sum = 0;
    int8_t number_of_motors = group.size();
	for (int i = 0; i < number_of_motors; i++) {
		sum += group.get_position_all()[i];
	}
	return sum/number_of_motors;
}
double vect_avg(std::vector<int>& vect) { // integer
	if (vect.empty()) return 0;
	double  sum = 0;
	for (double v : vect) {
		sum += v;
	}
	double average = sum/(double)vect.size();
	return average;
}
double vect_avg(std::vector<double>& vect) { // double
	if (vect.empty()) return 0;
	double  sum = 0;
	for (double v : vect) {
		sum += v;
	}
	double average = sum/vect.size();
	return average;
}

// Function to convert degrees to radians
double deg_to_rad(double degrees) {
	return degrees * M_PI / 180.0;
}
// Function to convert radians to degrees
double rad_to_deg(double radians) {
	return radians * 180.0 / M_PI;
}


/*
Units: Degrees
Output: inputted value as a range between 0-360°
*/
double wrap_to_360(double angle) {
    double wrapped = fmod(angle, 360.0);
    if (wrapped < 0) wrapped += 360.0;
    return wrapped;
}

/*
Units: Radians 
Map rotations to their appropriate 0 to 2 Pi range
*/
double angle_wrapping(double angle) {
	return std::remainder(angle, 2.0 * M_PI);	
}
