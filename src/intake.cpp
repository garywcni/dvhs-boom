#include "pros/misc.hpp"
#include "pros/motors.hpp"

//declare montor class
pros::Motor intake(12, pros::MotorGearset::green);
//declare controller class
pros::Controller controller(pros::E_CONTROLLER_MASTER);
bool buttonl1pressed = false;
bool buttonl2pressed = false;

int intakeState = 0;

void updateIntake () {
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
        if (!buttonl1pressed) {

            buttonl1pressed = true;

            if(intakeState == 0 || intakeState == 2) {
                intakeState=1;
                intake.move_voltage(-12000);

            } else if (intakeState == 1) {
                intakeState = 0;
                intake.move_voltage(0);
            }

        }
    } else {
        buttonl1pressed = false;
    }


    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
        if (!buttonl2pressed) {

            buttonl2pressed = true;

            if(intakeState == 0 || intakeState == 1) {
                intakeState=1;
                intake.move_voltage(12000);

            } else if (intakeState == 1) {
                intakeState = 0;
                intake.move_voltage(0);
            }
        }
    } else {
        buttonl2pressed = false;
    }
}
