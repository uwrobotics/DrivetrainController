/*
 * @file: arcade_controller.c
 * @brief: The file convert the joystick input into [-100, 100] mapping for left side motor and right side motor
 *
 *  Created on: May 30, 2024
 *      Author: linyuchen
 */

//TODO: Need Unit test

#include "arcade_controller.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x) < 0 ? -(x) : (x))

std_return_type arcade_drive(int8_t rotate, int8_t vertical_speed){
    //TODO: checking arcade driving logic and fix the control
	//TODO: add gtest(unit test support)


    /* variables to determine the quadrants*/

	//Note: this three variable is messing things up
    // left and right motor speeds
    int8_t left_motor_speed;
    int8_t right_motor_speed;

    //Note: this three variable is messing things up
    int8_t maximum = max(abs(rotate), abs(vertical_speed));
    int8_t total = vertical_speed + rotate;
    int8_t difference = vertical_speed - rotate;
    /*set speed according to the quadrant that the values are in*/
    if (vertical_speed >= 0){
        if (rotate >= 0){ // I quadrant
            left_motor_speed = maximum;
            right_motor_speed = difference;
        } else{            // II quadrant
            left_motor_speed = total;
            right_motor_speed = maximum;
        }
    } else{
        if (rotate >= 0){  // IV quadrant
            left_motor_speed = total;
            right_motor_speed = -maximum;
        } else{           // III quadrant
            left_motor_speed = -maximum;
            right_motor_speed = difference;
        }
    }

    int8_t left_right[2] = {left_motor_speed, right_motor_speed};
    SetLeftSideMotorSpeed(left_motor_speed);
    SetRightSideMotorSpeed(right_motor_speed);

    HAL_UART_Transmit(&huart4, left_right, 2, 200);




    //logic outline: prioritize turning, if no turning then check forward input
    // if(rotate > 0){
    // 	SetLeftSideMotorSpeed(rotate);
    //    	SetRightSideMotorSpeed(rotate/2);
    // }
    // else if(rotate < 0) {
    // 	SetLeftSideMotorSpeed(rotate);
    // 	SetRightSideMotorSpeed(rotate/2);
    // }
    // else if(vertical_speed != 0){
    // 	SetLeftSideMotorSpeed(vertical_speed);
    //    	SetRightSideMotorSpeed(-vertical_speed);
    // }
    // else{
    // 	SetLeftSideMotorSpeed(0);
    // 	SetRightSideMotorSpeed(0);
    // }

    return E_OK;
}
