#include "displays.h"
#include "jb_math.h"
#include "liblvgl/core/lv_obj.h"
#include "liblvgl/core/lv_obj_pos.h"
#include "liblvgl/display/lv_display.h"
#include "liblvgl/llemu.hpp"
#include "liblvgl/misc/lv_area.h"
#include "liblvgl/widgets/image/lv_image.h"
#include "pros/llemu.hpp"
#include "pros/misc.hpp"
#include "pros/screen.h"
#include <cstring>
#include <string>
#include <vector>

extern char alliance;

extern int autonVariation;

extern std::string auton_name;
extern std::string other_text;

extern std::vector<std::string> routines;

lv_obj_t * Label;
lv_obj_t * VarLabel;
lv_obj_t * cont_row;
lv_obj_t * cont_col;
lv_obj_t * blueBtn;
lv_obj_t * redBtn;
lv_obj_t* img;

static lv_style_t style_btn;
static lv_style_t style_btn_pressed;
static lv_style_t style_btn_red;

const short autonVariations = 3;

const lv_coord_t minBtnWidth = 90;//px
const lv_coord_t routineWidth = 230;// 250 px max
const lv_coord_t verboseMargin = 10;//px

inline void BlackBackground() { lv_obj_set_style_bg_color( lv_screen_active(), lv_color_hex(0x000000), 0); }
inline void BlueBackground() { lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x00008B), 0); }
inline void RedBackground() { lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFF0000), 0); }
void display_img(const void * dir);
void hide_img();
void show_logo();

// PROS temperature screen external variable declarations
extern bool showTemp; // Toggle on and off the PROS lcd display
extern pros::MotorGroup left_mtr_group, right_mtr_group, lift, intake;
extern pros::Controller master;
extern char temp_buff[16];
// Update the screen's background color along with the screen's labels
void update_display_info(){
	// update the selected variation
	lv_label_set_text_fmt(VarLabel, "Variation %d selected!", autonVariation);
	// update the selected autonomous routine
	lv_label_set_text_fmt(Label,"%s",auton_name.c_str());
	if (alliance == 'b') BlueBackground();
	else if (alliance == 'r') RedBackground();
	else BlackBackground();
}
// This is the callback function that is run for every button event
static void btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    void * btn = lv_event_get_target(e);
    if(code == LV_EVENT_SHORT_CLICKED) {
		uintptr_t btnID = reinterpret_cast<uintptr_t>(lv_event_get_user_data(e));
		if (btnID == 99) { // temperature screen
			pros::lcd::initialize();
			pros::delay(100);
			pros::lcd::register_btn0_cb(turnOffScreen);
			pros::lcd::register_btn1_cb(show_logo);
			showTemp = true;
		}else if (btnID >= 100 && btnID < 200) { // variation buttons
			autonVariation = btnID - 99;
			printf("variation = %d\n",autonVariation);
		} else { // auton and alliance buttons
            // routine button: user_data is a pointer to char buffer
            const char* btnName = static_cast<const char*>(lv_event_get_user_data(e));
			if (strcmp(btnName, "Blue") == 0) {
				alliance = 'b';
			} else if (strcmp(btnName,"Red") == 0) {
				alliance = 'r';
			} else {
				auton_name = btnName;
            	printf("%s\n", btnName);
			}
        }
		update_display_info();
    }
}

//------------------- Styles -------------------
static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa) {
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

void setupStyles() {
	/*Create a simple button style*/
	// temperature screen button style
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_text_color(&style_btn, lv_color_black());
    /*Create a style for the pressed state.
     *Use a color filter to simply modify all colors in this state*/
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_btn_pressed);
    lv_style_set_color_filter_dsc(&style_btn_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_btn_pressed, LV_OPA_20);
    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_btn_red, lv_palette_darken(LV_PALETTE_RED, 2));
	// setup image
	img = lv_image_create(lv_screen_active());
}
// ----------- buttons -----------
static void SetToRed(_lv_obj_t * btn) { // Make the button red (blue is default color)
	lv_obj_add_style(btn, &style_btn, 0);
	lv_obj_add_style(btn, &style_btn_red, 0);
	lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
}

void create_buttons(){
	/*Create a container with ROW flex direction*/  
    cont_row = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont_row, 460,  60);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);
	lv_coord_t btnWidth = 402.0/autonVariations; // scale the variation buttons to fit screen
	if (btnWidth < minBtnWidth) btnWidth = minBtnWidth;
    /*Create a container with COLUMN flex direction*/ 
	// The autonomous routine column
    cont_col = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont_col, routineWidth, 172);
    lv_obj_align_to(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_LEFT, 0,5);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
	// Fill the Variations row with buttons
    for (uint32_t i = 100; i < autonVariations + 100; i++) {
        lv_obj_t * obj;
        lv_obj_t * label;
        /*Add items to the row*/
        obj = lv_button_create(cont_row);
		lv_obj_set_size(obj, btnWidth, LV_PCT(100)); // shrink buttons to prevent vertical scrolling
        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Var %" LV_PRIu32, i - 99 ); // set label
		// Pass the button id to the callback event handler
		lv_obj_add_event_cb(obj, btn_event_cb, LV_EVENT_ALL, (void *)i); 
        lv_obj_center(label);
	}
	// Fill the Routines Column with buttons
    for(std::string routine: routines) {
        lv_obj_t * btn;
        lv_obj_t * label;
        /*Add items to the column*/
        btn = lv_button_create(cont_col);
        lv_obj_set_size(btn, LV_PCT(100), LV_SIZE_CONTENT);
        label = lv_label_create(btn);
		lv_label_set_text_fmt(label, "%s", routine.c_str()); // add label
		
		lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0); // Set the corner radius
		// Allocate buffer dynamically
		char* btnName = new char[16];
		snprintf(btnName, 16, "%s", routine.c_str());
		lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, (void*)btnName); 
        lv_obj_center(label);

	}
	//Alliance Buttons
	blueBtn = lv_button_create(lv_screen_active());
	lv_obj_t * blueLabel = lv_label_create(blueBtn);
	lv_obj_align_to(blueBtn, cont_col, LV_ALIGN_OUT_TOP_RIGHT, 70, 30);
	lv_label_set_text(blueLabel, "Blue");
	// create call back
	lv_obj_add_event_cb(blueBtn, btn_event_cb, LV_EVENT_ALL, (void*)"Blue"); 
	// Red
	redBtn = lv_button_create(lv_screen_active());
	lv_obj_t * redLabel = lv_label_create(redBtn);
	lv_obj_align_to(redBtn, blueBtn, LV_ALIGN_OUT_TOP_RIGHT, 70, 36);
	lv_label_set_text(redLabel, "Red");
	SetToRed(redBtn);
	// create call back
	lv_obj_add_event_cb(redBtn, btn_event_cb, LV_EVENT_ALL, (void*)"Red"); 
}
void additional_features() {
	// Create the output text
	Label = lv_label_create(lv_screen_active()); //create label and puts it on the screen
	lv_label_set_text(Label, "Select a Routine!"); //sets label text
	lv_obj_align(Label, LV_ALIGN_TOP_LEFT, routineWidth + 10 + verboseMargin, 100);
	// Create the output text box for the auton variation
	VarLabel = lv_label_create(lv_screen_active()); //create label and puts it on the screen
	char starting_Label [27];
	snprintf(starting_Label,sizeof(starting_Label),"Variation %.0d selected!",autonVariation); // dynamically print the starting variation
	//sprintf(starting_Label,"Variation %.0d selected!",autonVariation); // deprecated
    lv_label_set_text(VarLabel, starting_Label); //sets label text
	lv_obj_align_to(VarLabel, Label, LV_ALIGN_TOP_LEFT, 0, 90);
	// Add show temp screen btn
	lv_obj_t * tmpBtn;
	lv_obj_t * tmpLabel;
	tmpBtn = lv_button_create(lv_screen_active());
    lv_obj_remove_style_all(tmpBtn);
    lv_obj_add_style(tmpBtn, &style_btn, 0);
    lv_obj_add_style(tmpBtn, &style_btn_pressed, LV_STATE_PRESSED);
	lv_obj_set_size(tmpBtn, 160, LV_SIZE_CONTENT);
	lv_obj_align_to(tmpBtn,VarLabel,LV_ALIGN_TOP_LEFT,0,25);
	tmpLabel = lv_label_create(tmpBtn);
	lv_label_set_text(tmpLabel, "Show Motor Temps");
	lv_obj_add_event_cb(tmpBtn, btn_event_cb, LV_EVENT_ALL, (void *)99); 
    lv_obj_center(tmpLabel);
	update_display_info(); // Set labels and background color
}

void display_img(const void * dir) {
	// hide auton selector
	lv_obj_add_flag(cont_col, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(cont_row, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(blueBtn, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(redBtn, LV_OBJ_FLAG_HIDDEN);
	//display image
	img = lv_image_create(lv_screen_active());
	lv_obj_remove_flag(img, LV_OBJ_FLAG_HIDDEN);
	lv_image_set_src(img,dir);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
}
void show_logo() {
	showTemp = false;
	pros::lcd::shutdown();
	display_img("S:/brain_logo.bin");
}
void hide_img() {
	lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
	// show auton selector
	lv_obj_remove_flag(cont_col, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(cont_row, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(blueBtn, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(redBtn, LV_OBJ_FLAG_HIDDEN);
	
}

// #################### PROS temperature screen #######################
/* Turn off the PROS lcd display (i.e. the temperature screen)
 * this allows the LVGL auton selector to be visible
*/
void turnOffScreen() {
	pros::lcd::shutdown();
	hide_img();
	showTemp = false;
}
void printTemps() {
	std::vector<double> L_temp = left_mtr_group.get_temperature_all();
	std::vector<double> R_temp = right_mtr_group.get_temperature_all();
	std::vector<double> lift_temp = lift.get_temperature_all();
	std::vector<double> intake_temp = intake.get_temperature_all();
	pros::lcd::print(0, "\t\t"
	"Left: %.0f %.0f %.0f", L_temp[0],L_temp[1],L_temp[2]);
	pros::lcd::print(1, "\t\t"
	"Right: %.0f %.0f %.0f", R_temp[0],R_temp[1],R_temp[2]);
	pros::lcd::print(3,"Lift: %.0f %.0f      Intake: %.0f",
		lift_temp[0],lift_temp[1], intake_temp[0]);
}
// checks if any motor in the group is overheating
bool any_over_temp(const pros::MotorGroup& group) {
    const auto temps = group.is_over_temp_all();
    return std::any_of(temps.begin(), temps.end(), [](int32_t v){ return v != 0; });
}
// ------------- Print Motor's Temperatures to the brain's screen -------------
void temperature_monitoring() {
	pros::delay(400);
	while (!showTemp) {
		pros::delay(100);
	}
	pros::lcd::initialize(); // turn on PROS display
	pros::delay(100);
	pros::lcd::register_btn0_cb(turnOffScreen);
	pros::lcd::register_btn1_cb(show_logo);
	
	std::string hot_message = "Overheating";

	std::string driveTemp = " ";
	pros::delay(110);
	while (true) {
		if (showTemp) {
			std::vector<double> left_temps = left_mtr_group.get_temperature_all();
			std::vector<double> right_temps = right_mtr_group.get_temperature_all();
			int avg_temp = (vect_avg(left_temps) + vect_avg(right_temps))/2.0;
			std::vector<double> lift_temp = lift.get_temperature_all();
			std::vector<double> intake_temp = intake.get_temperature_all();

			if (any_over_temp(left_mtr_group) || any_over_temp(right_mtr_group)){
				master.clear_line(0);
				pros::delay(50);
				pros::c::screen_erase();
				pros::c::screen_set_pen(pros::c::COLOR_CRIMSON);
				pros::c::screen_fill_rect(1,1,500,300);
				printTemps();
				std::cout << "over temperature!! \n";
				// Warn user
				driveTemp = "Hot!";
				snprintf(temp_buff, sizeof(temp_buff), "%s lift:%.0f", driveTemp.c_str(), std::max({lift_temp[0],lift_temp[1]}));
				if (other_text == " ") {
					other_text = hot_message;
				}
				// Vibrate Controller
				pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, ".");
				pros::delay(300);
			} else {
				pros::c::screen_erase();
				printTemps();
				if (avg_temp <= 35) {
					driveTemp = "Good";
				} else if (avg_temp <= 45){
					driveTemp = "Ok";
				} else if (avg_temp >45 ){
					driveTemp = "Warm";
				}
				snprintf(temp_buff, sizeof(temp_buff), "%s L:%.0f i:%.0f", driveTemp.c_str(), std::max({lift_temp[0],lift_temp[1]}),intake_temp[0]);
				if (other_text == hot_message){
					other_text = " ";
				}
				pros::c::screen_set_pen(pros::c::COLOR_GREEN);
				pros::c::screen_fill_rect(0,0,480,300);
			}
			pros::delay(7000);
		} else {
			pros::delay(100);
		}
		
	}
}