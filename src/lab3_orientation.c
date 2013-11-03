/**
  ******************************************************************************
  * @file    lab3_orientation.c
  * @author  Group 6
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   Provides a structure and functionality to implement tilt detection
  */

/* Includes ------------------------------------------------------------------*/

#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery_lis302dl.h"
#include "lab3_orientation.h"

/* Private Functions ---------------------------------------------------------*/

/** @defgroup Private_Functions
  * @{
  */

/**
  * @brief  Get raw accelerometer readings and calculate orientation (roll, pitch)
	* @param  *orientation: Pointer to an Orientation struct, to update
  * @retval None
  */

static void read_accelerometer(struct Orientation *orientation) {
	int32_t reading[3];
	
	LIS302DL_ReadACC(reading);																																										//Read LIS302DL output register, and calculate the acceleration (mg)
	
	orientation->rawx = reading[0];
	orientation->rawy = reading[1];
	orientation->rawz = reading[2];
	
	orientation->x = (orientation->rawx*ACC11 + orientation->rawy*ACC21 + orientation->rawz*ACC31) + ACC10;				//Calibrate X
	orientation->y = (orientation->rawx*ACC12 + orientation->rawy*ACC22 + orientation->rawz*ACC32) + ACC20;				//Calibrate Y
	orientation->z = (orientation->rawx*ACC13 + orientation->rawy*ACC23 + orientation->rawz*ACC33) + ACC30;				//Calibrate Z
	
	orientation->pitch = 180*atan(orientation->x/sqrt(pow(orientation->y,2)+pow(orientation->z,2)))/PI;						//Calculate pitch angle
	orientation->roll = 180*atan(orientation->y/sqrt(pow(orientation->x,2)+pow(orientation->z,2)))/PI;						//Calculate roll angle
	orientation->yaw = 0;																																													//Calculate yaw angle (always 0 with the STM32F4 Discovery's accelerometer configuration)
}

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */

/**
  * @brief  Get accelerometer's current reading, and update Orientation's moving average struct
	* @param  *orientation: Pointer to an Orientation struct, to update
  * @retval None
  */

void update_orientation(struct Orientation *orientation) {
	read_accelerometer(orientation);																																							//Read data into orientation struct
	
	insert_value(&orientation->moving_average_pitch, orientation->pitch);																					//Insert most recent pitch reading into moving average filter
	insert_value(&orientation->moving_average_roll, orientation->roll);																						//Insert most recent roll reading into moving average filter

	calculate_average(&orientation->moving_average_pitch);																												//Calculate pitch average
	calculate_average(&orientation->moving_average_roll);																													//Calculate roll average
}

/**
  * @brief  Initalize accelerometer for use as a tilt angle sensor with tap detection
	* @param  None
  * @retval None
  */

void init_accelerometer() {
	LIS302DL_InitTypeDef LIS302DL_InitStruct; 																																		//Struct for accelerometer initialization
	
	uint8_t int_ctrl_reg_value = 0x38;																																						//(0011 1000) Active high, push-pull, click interrupt, data ready
	uint8_t click_cfg_reg_value = 0x10;																																						//(0001 0000) Set interrupt on single Z click, don't latch interrupt signal high
	
	uint8_t click_window_reg_value = 0x7F;																																				//(0111 1111) Max time interval after end of latency interval where click detection can start again if device configured for double click detect
	uint8_t click_latency_reg_value = 0x7F;																																				//(0111 1111) Time interval where click detection is disabled after first click, if configured for double click detection
	uint8_t click_time_limit_reg_value = 0x03;																																		//(0000 0011) Maximum time interval that can elapse between the start of the click detect procedure and when the accel goes back below threshold
	uint8_t click_threshold_z_value = 0x0A;																																				//(0000 1010) Z-axis sensitivity threshold
	uint8_t click_threshold_xy_value = 0xAA;																																			//(0000 1010) X&Y-axis sensitivity thresholld
	
	//Accelerometer Configuration
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; 																								//Either lowpower on or off
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100; 																									//Options are 100Hz or 400Hz, we are sampling at 25Hz, no sense using faster freq
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE; 									//Turn on all 3 axes
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3; 																											//Define typical range of measured values, doubtfull we'll hit 9g
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL; 																										//Off, otherwise apply actuation force, sensors change their DC levels (way to make sure everything is working)
	LIS302DL_Init(&LIS302DL_InitStruct);																																					//Apply initialization settings to accelerometer
	
	//Advanced accelerometer configuration
	LIS302DL_Write(&int_ctrl_reg_value, LIS302DL_CTRL_REG3_ADDR, sizeof(int_ctrl_reg_value));											//Enable click interrupt on INT0
	LIS302DL_Write(&click_cfg_reg_value, LIS302DL_CLICK_CFG_REG_ADDR, sizeof(click_cfg_reg_value));								//Configure single click detection
  LIS302DL_Write(&click_threshold_xy_value, LIS302DL_CLICK_THSY_X_REG_ADDR, 1); 	    													//Configure Click Threshold on X/Y axis (10 x 0.5g)
  LIS302DL_Write(&click_threshold_z_value, LIS302DL_CLICK_THSZ_REG_ADDR, 1);																		//Configure Click Threshold on Z axis (10 x 0.5g)
  LIS302DL_Write(&click_time_limit_reg_value, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);														//Configure Time Limit
  LIS302DL_Write(&click_latency_reg_value, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);																	//Configure Latency
  LIS302DL_Write(&click_window_reg_value, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);    																//Configure Click Window
}

/**
  * @brief  Initalize orientation struct
	* @param  *orientation: Pointer to Orientation structure to be initialized
  * @retval None
  */

void init_orientation(struct Orientation *orientation) {
	orientation->rawx = 0;																																												//Set ALL THE THINGS to zero
	orientation->rawy = 0;
	orientation->rawz = 0;
	orientation->x = 0;
	orientation->y = 0;
	orientation->z = 0;
	orientation->pitch = 0;
	orientation->roll = 0;
	orientation->yaw = 0;
	init_moving_average(&orientation->moving_average_pitch);																											//Initialize moving average filter for pitch reading
	init_moving_average(&orientation->moving_average_roll);																												//Initialize moving average filter for roll reading
	init_moving_average(&orientation->moving_average_yaw);																												//Initialize moving average filter for yaw reading
}

/**
  * @}
  */
