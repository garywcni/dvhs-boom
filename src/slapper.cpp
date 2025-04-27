//#include "globals.hpp"
#include "pros/motors.hpp"

bool  buttonr2pressed = false;

//declare montor class, 10 is slapper motor port, E_MOTOR_GEAR_(COLOR WE DONT KNOW YET)
pros::Motor slapper(10, pros::MotorGearset::red);
//declare controller class
pros::Controller slapper_controller(pros::E_CONTROLLER_MASTER);

int slapperState = 0;

 void updateSlapper () {
    if (slapper_controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
        if (!buttonr2pressed) {

            buttonr2pressed = true;

            if(slapperState == 0 || slapperState == 2) {
                slapperState=1;
                slapper.move_voltage(-6000);

            } else if (slapperState == 1) {
                slapperState = 0;
                slapper.move_voltage(0);
            }
        }
    }
    
    else{

        buttonr2pressed = false;
    }
}