//**************************************
//Main file of Boom-Boom robotics system
//**************************************
#include "main.h"
#include "intake.h"
#include "slapper.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

// --- Stringification Macros ---
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

// --- Debug Message ---
// Try changing this value to 0, 1, 2, or undefining it (0: disabled, 1: only Terminal)
#define DEBUG_LEVEL 1

#define LOG_1(message) do { \
    /* Format: <filename>:<line>: <your_message> */ \
    std::cout << __FILE__ << ":" << __LINE__ << ": " << (message) << std::endl; \
} while (0)

//-----------------------


// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups

#if 0 
pros::MotorGroup leftMotors({-5, 4, -3}, pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({6, -9, 7}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)
#else
pros::MotorGroup left_motors({-8, -7, -11}, pros::MotorGearset::blue); // left motors on ports 1, 2, 3
pros::MotorGroup right_motors({13, 4, 9}, pros::MotorGearset::blue); // right motors on ports 3, 5, 6
#endif

// Inertial Sensor on port 10
pros::Imu imu(10);

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor, port 20, not reversed
pros::Rotation horizontalEnc(20);
// vertical tracking wheel encoder. Rotation sensor, port 11, reversed
pros::Rotation verticalEnc(-11);
// horizontal tracking wheel. 2.75" diameter, 5.75" offset, back of the robot (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -5.75);
// vertical tracking wheel. 2.75" diameter, 2.5" offset, left of the robot (negative)
lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_275, -2.5);

// drivetrain settings
#if 0
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_4, // using new 4" omnis
                              360, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);
#else
lemlib::Drivetrain drivetrain(
	&left_motors, //left motor group
	&right_motors,//right motor group
	13, //10 inch track width
	lemlib::Omniwheel::NEW_325,//using new 4" omnis
	360, // drivetrain rpm is 360
	2 // horizontal drift is 2 
);
#endif

#if 0
// lateral motion controller
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            0, // integral gain (kI)
                                            3, // derivative gain (kD)
                                            3, // anti windup
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(2, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             10, // derivative gain (kD)
                                             3, // anti windup
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
);
#else
//lateral motion controller
lemlib::ControllerSettings linearController(5, //proportional gain (kP)
											0,
											0,
											3,
											1,
											100,
											2,
											500,
											0
);

// angular motion controller 
lemlib::ControllerSettings angularController(3.3,
											0,
											27,
											0,
											1,
											100,
											3,
											500,
											0
);
#endif

#if 0
// sensors for odometry
lemlib::OdomSensors sensors(&vertical, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            &horizontal, // horizontal tracking wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);
#else
lemlib::OdomSensors sensors(nullptr,
							nullptr,
							nullptr,
							nullptr,
							0
);
#endif 

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

#if 0
// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);
#else
// create the chassis class
lemlib::Chassis chassis(
	drivetrain, // drivetrain settings
	linearController, // lateral PID settings
	angularController, // angular PID settings
	sensors, // odometry sensors
	nullptr,
	nullptr
);
#endif

void on_center_button() 
{
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) 
	{
		pros::lcd::set_text(2, "I was pressed!");
	} 
	else 
	{
		pros::lcd::clear_line(2);
	}
}
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    pros::lcd::set_text(1, "Hello Team Boom! Boom!");

#if DEBUG_LEVEL == 1
    std::string version_string_runtime = "Version:";
    version_string_runtime += STRINGIFY(VERSION_MAJOR);
    version_string_runtime += ".";
    version_string_runtime += STRINGIFY(VERSION_MINOR);
    LOG_1(version_string_runtime);
    LOG_1("Enter initialize()");
#endif 

    pros::lcd::register_btn1_cb(on_center_button);
    chassis.calibrate(); // calibrate sensors

    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            // delay to save resources
            pros::delay(50);
        }
    });
}

/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(example_txt); // '.' replaced with "_" to make c++ happy

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void autonomous() {
#if 0
    // Move to x: 20 and y: 15, and face heading 90. Timeout set to 4000 ms
    chassis.moveToPose(20, 15, 90, 4000);
    // Move to x: 0 and y: 0 and face heading 270, going backwards. Timeout set to 4000ms
    chassis.moveToPose(0, 0, 270, 4000, {.forwards = false});
    // cancel the movement after it has traveled 10 inches
    chassis.waitUntil(10);
    chassis.cancelMotion();
    // Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPoint(45, -45, 1000, {.maxSpeed = 60});
    // Turn to face a direction of 90º. Timeout set to 1000
    // will always be faster than 100 (out of a maximum of 127)
    // also force it to turn clockwise, the long way around
    chassis.turnToHeading(90, 1000, {.direction = AngularDirection::CW_CLOCKWISE, .minSpeed = 100});
    // Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 116 to see how to define a path
    chassis.follow(example_txt, 15, 4000, false);
    // wait until the chassis has traveled 10 inches. Otherwise the code directly after
    // the movement will run immediately
    // Unless its another movement, in which case it will wait
    chassis.waitUntil(10);
    pros::lcd::print(4, "Traveled 10 inches during pure pursuit!");
    // wait until the movement is done
    chassis.waitUntilDone();
    pros::lcd::print(4, "pure pursuit finished!");
#endif
}

/**
 * Runs in driver control
 **/
void opcontrol() 
{
    initialize();
    // controller
    // loop to continuously update motors
    while (true) 
	{
     
        // get joystick positions
#if 0
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        // move the chassis with curvature drive
        chassis.arcade(leftY, rightX);
        // delay to save resources
#endif
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        chassis.tank(leftY, rightY, 0);
	
#if DEBUG_LEVEL == 1
    LOG_1("Enter updateIntake()");
#endif    
	updateIntake();
#if DEBUG_LEVEL == 1
    LOG_1("Enter updateSlapper()");
#endif   	
    updateSlapper();
    pros::delay(10);
    }
}
