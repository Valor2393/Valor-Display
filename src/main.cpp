/**
 *	This program was written by John Burger
 *	Valor 2393S Robotics — Soren Schurter and John Burger
 *	November 2026 — VEX V5 Override Robotics Competition 
 *	This program is an example that shows off our brain display features:
 * Autonomous Selector
 * Temperature monitoring screen (shows motor temperatures)
*/

#include "main.h"
#include "pros/abstract_motor.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

// Program Dependencies

#include "jb_math.h"
#include "displays.h"// Auton selector & temperature screen


pros::IMU imu(14);
inline double get_heading(pros::IMU& gyro = imu) {return wrap_to_360(gyro.get_rotation() * 1.0054 );} 
void odom_verbose();

// ———————————————— Drive Chassis ————————————————
pros::MotorGroup left_mtr_group({-11, 12, 13},pros::v5::MotorGears::blue); // left motor group	
pros::MotorGroup right_mtr_group({-18, -19, 20},pros::v5::MotorGears::blue); // right motor group 

// Add enum containing three modes to cycle through
enum controllerMode {ARCADE, TANK, SPLIT};

// ———————————————— Lift ————————————————
pros::MotorGroup lift({-9,16},pros::v5::MotorGears::blue);
// ———————————————— Intake ————————————————
pros::MotorGroup intake({-15});

	
// ———————————————— Controller ————————————————
pros::Controller master(CONTROLLER_MASTER);



// ———————————————— Auton Variables ————————————————
char alliance = ' ';

int autonVariation = 1;

std::string auton_name = "Skills";

std::vector<std::string> routines {"Side","Close Split", "Solo","Close","Skills"};
void wait_for_gyro_cal(uint32_t max_wait = 60000);
// Other Declarations
void controller_display();
//bool try_odom_reset(bool reset_east = true, bool reset_west = true, bool reset_north = false, bool reset_south = false , int margin = 25, bool force_reset =false);

char temp_buff[16]; // output text to controller's LCD display

/*############################ Settings #############################
|																	|
|																	|
####################################################################*/
// ———————————————— Operation Control ————————————————
double dead_zone = 10;
inline void add_dead_zone(double& value, double = dead_zone) { if (std::abs(value) < dead_zone) value = 0;}
controllerMode drive_mode = ARCADE;
#define TURN_RATO  0.65 // limit turning speed

// ———————————————— Debuging Variables ————————————————
bool showTemp = false; // When true the brain's LCD displays the motors' temperatures
// showTemp is set to true in driver control
std::string other_text = " "; // Used to print a message the controller's LCD

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	left_mtr_group.set_zero_position_all(0);				// Rest drivetrain motor encoders
	right_mtr_group.set_zero_position_all(0); 	

	left_mtr_group.set_brake_mode(pros::MotorBrake::coast); 	// Allow the drivetrain to coast
	right_mtr_group.set_brake_mode(pros::MotorBrake::coast);

	imu.reset(); 													// Reset the gyro

	// ——————————————————————————— Background Tasks ———————————————————————————
	pros::Task odom_verbose_task([&]{odom_verbose();});
	pros::Task display_task([&]{controller_display();});
	pros::Task temp_screen_task([=] {temperature_monitoring();});



	// –––––– Task Priorities ––––––
	// Auxiliary tasks
	odom_verbose_task.set_priority(TASK_PRIORITY_DEFAULT - 1);
	display_task.set_priority(TASK_PRIORITY_DEFAULT - 1);
	temp_screen_task.set_priority(TASK_PRIORITY_DEFAULT - 1);

	// ——————————————————————————— Auton Selector ———————————————————————————
	setupStyles();
	create_buttons();
	additional_features();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
	while (true) {
		// —— Check if gyro calibrate incorrectly——
		if (std::abs(imu.get_rotation()) > 0.5 && !imu.is_calibrating()) {
			master.rumble("_");
			// update controller screen
			other_text = "Gyro! "; // Display this error instead of odometry
			master.rumble(".");
			other_text += std::to_string(get_heading());
			pros::delay(5000); //wait before checking again
		}
		pros::delay(500);
	}
}


void autonomous() {
	//! Do not continue with autonomous if the gyro failed to calibrate
	wait_for_gyro_cal(7000);
 	// ———————————————— Setup ————————————————
	uint32_t star_t = pros::millis(); 	// get clock at start of auton

	// turn the toggle to alliance color (Regardless of routine)
	if (auton_name == "Close") {
		// routine code

	} else if (auton_name == "Side") {
		// routine code

	} 
	else if (auton_name == "Close Split")
	{
		// routine code
	}
	else if (auton_name == "Solo")
	{
		// Add solo AWP code here
	}
	else if (auton_name == "Skills")
	{	
		// Add Skills code here	
	}
	else { // no routine selected;
	}
	printf("Routine duration: %.0u\n",pros::millis()-star_t);
}

controllerMode next_mode(controllerMode config)
{
	return  (config == SPLIT)? ARCADE : static_cast<controllerMode>(config + 1);
}

/**
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	showTemp = true; // Set motor settings for driver control
	left_mtr_group.set_brake_mode(MOTOR_BRAKE_COAST);
	right_mtr_group.set_brake_mode(MOTOR_BRAKE_COAST);
	while (true) {
		// Change driving mode
		if (master.get_digital_new_press(DIGITAL_RIGHT)) {
			drive_mode = next_mode(drive_mode);
		}
		double y = master.get_analog(ANALOG_LEFT_Y);
		double y_r = master.get_analog(ANALOG_RIGHT_Y);
		double x = master.get_analog(ANALOG_LEFT_X);
		double x_r = master.get_analog(ANALOG_RIGHT_X);
		double L, R; // outputs to the left and right side of drive
		// Apply dead-zone
		add_dead_zone(y);
		add_dead_zone(y_r);
		add_dead_zone(x);
		add_dead_zone(x_r);
		
		// Set the drivetrain's left and right power according to drive mode
		switch (drive_mode ){
			case ARCADE:
				L = y + x * TURN_RATO;
				R = y - x * TURN_RATO;
				break;
			case TANK:
				L = y;
				R = y_r;
				break;
			case SPLIT:
				L = y + x_r * TURN_RATO;
				R = y - x_r * TURN_RATO;
		}
		// Apply power to drivetrain			
		left_mtr_group.move(L);
		right_mtr_group.move(R);

		
		// ————————————— Special buttons —————————————
		if (master.get_digital_new_press(DIGITAL_DOWN)) {
			other_text = " "; // clear custom message on display
		}

		pros::delay(10);// Delay to save resources
	}
}


			/*###################### Function Definitions #######################
		<	|																	|	>
	<		|																	|		>
<			|																	|			>
	<		|																	|		>
		<	|																	|	>
			####################################################################*/



void controller_display() {
	pros::delay(500);
	char buffer[16];
	while (true) {
		for (int i = 0; i < 2; i++) {
			master.clear_line(0);
			pros::delay(50);
			if (i == 0) { // custom message
				if (other_text != " ")
					snprintf(buffer,sizeof(buffer),"%s",other_text.c_str());
				else {
					// Custom Odometry
					//snprintf(buffer,sizeof(buffer),"%.1f,%.1f", bot.x,bot.y);
					// LemLib Odometry
					//snprintf(buffer,sizeof(buffer),"%.1f,%.1f,%.1f", chassis.getPose().x,chassis.getPose().y,get_heading());
					if (pros::competition::is_connected()) i ++;
				}
			}
			if (i == 1) {
				if (pros::competition::is_disabled()) {
					// print autonomous selection
					snprintf(buffer,sizeof(buffer), "%c,%s,var%d",alliance,auton_name.c_str(),autonVariation);
				} else { // driver control
					strcpy(buffer, temp_buff);
				}
				
			}

			master.set_text(0,0, buffer);
			pros::delay(1000);
		}
	}
}
void odom_verbose() {
	while (true) {
		char orientation[41];
		char orientation_two[41];
		char auton_data[41];
		// Custom odometry 
		//snprintf(orientation, sizeof(orientation),"Bot's Pose -> X: %.2f, Y:  %.2f, %.2f°", bot.x, bot.y, bot.heading);
		// LemLib's tracking center
		//snprintf(orientation_two, sizeof(orientation_two),"Tracking -> X: %.2f, Y: %.2f, %.2f°",chassis.getPose().x,chassis.getPose().y,chassis.getPose().theta);
		// Auton Selector
		snprintf(auton_data, sizeof(auton_data),"%s, %c, var: %d\t lift %.0f intake:%.1fW",auton_name.c_str(),alliance,autonVariation, lift.get_position(),intake.get_power());
		pros::lcd::set_text(5, auton_data);
		pros::lcd::set_text(6, orientation);
		pros::lcd::set_text(7, orientation_two);
		pros::delay(200); // 5Hz refresh rate
	}
	pros::lcd::clear(); // clear the display to prevent misleading information
}
void wait_for_gyro_cal(uint32_t max_wait) {
	uint32_t t0 = pros::millis();
	while (imu.is_calibrating() && pros::millis() - t0 <  max_wait) {
		pros::delay(10); 
		other_text = "Waiting for gyro cal";
	}
}
