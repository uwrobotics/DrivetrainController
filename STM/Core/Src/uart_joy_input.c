/*
 * uart_joy_input.c
 *
 *  Created on: May 30, 2024
 *      Author: linyuchen
 */

#include "uart_joy_input.h"
#include "arcade_controller.h"


/*Message Scheme
 *
 * +--------------------+------------------+-------------+--------+
 *  startbit + DEVICE ID   Vertical speed    rotation	    CRC 8
 */

#define max(a,b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x) < 0 ? -(x) : (x))

uint8_t UART_rxBuffer[4];
uint8_t calc_rx_buffer[3];

const uint8_t handshake_message = {0b10000001};

void UartHandshake() {
	while(UART_rxBuffer[0] != 1) {
		HAL_UART_Receive(&huart4, UART_rxBuffer, 1, 200);
	}

	HAL_UART_Transmit(&huart4, &handshake_message, 1, 200);

	HAL_UART_Receive_DMA(&huart4, UART_rxBuffer, 4);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//Implemented the Handshake protocol ground station -> stm -> ground station
	//TODO: Implement the full CRC8
	//TODO: Implement heart beat and timeout
	//Implemented header checker
	//TODO: Implement boundary check
	//TODO: define return message scheme

	/*--------------------- Note: Debugger does not work with dma use HAL transmit ack ------------------*/
	if(UART_rxBuffer[0] == 0x81) {
		if(UART_rxBuffer[1] == 0x03) {
			//joystick message
			int8_t vertical_speed = UART_rxBuffer[2];
			int8_t rotate = UART_rxBuffer[3];

			//TODO: checking arcade driving logic and fix the control
			//TODO: add gtest(unit test support)


			/* variables to determine the quadrants*/

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

			HAL_UART_Transmit(&huart4, left_right, 2, 200); 

			// Calculate difference, max, and total
            //int8_t difference = vertical_speed - rotation;
            //int8_t maximum = max(abs(rotation), abs(vertical_speed));
            //int8_t total = vertical_speed + rotation;

            // Store calculations in calc_rx_buffer
            //calc_rx_buffer[0] = (uint8_t)difference;
            //calc_rx_buffer[1] = (uint8_t)maximum;
            //calc_rx_buffer[2] = (uint8_t)total;


			arcade_drive(rotate, vertical_speed);

			//uint8_t ack_rx_buffer[2] = {vertical_speed, rotation};
			//HAL_UART_Transmit(&huart4, ack_rx_buffer, 2, 200); 
			//HAL_UART_Transmit(&huart4, calc_rx_buffer, 3, 200); //transmit diff, max, and total
			
		} else if (UART_rxBuffer[1] == 0b00000010){
			//Heart beat message
		}
		HAL_UART_Receive_DMA(&huart4, UART_rxBuffer, 4);
	} else {
	}

	HAL_UART_Receive_DMA(&huart4, UART_rxBuffer, 4);
}



