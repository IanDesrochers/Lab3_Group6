#include <stdio.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "stm32f4_discovery_lis302dl.h"
#include "lab3_accelerometer.h"

#define PI 3.14159

void getOrientation(struct Orientation *orientation) {
	int32_t reading[3];
	LIS302DL_ReadACC(reading);
	orientation->pitch = 180*atan(reading[0]/sqrt(pow(reading[1],2)+pow(reading[2],2)))/PI;						//do math, get angle for pitch
	orientation->roll = 180*atan(reading[1]/sqrt(pow(reading[0],2)+pow(reading[2],2)))/PI;						//do more math, get angle for roll
	orientation->yaw = 0;																																							//
}

void init_accel() {
	LIS302DL_InitTypeDef LIS302DL_InitStruct; 											//Struct for initialization
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct; 			//struct for interrupt configuration
	
	uint8_t int_ctrl_reg_value = 0x38;															//(00111000) active high, push-pull, click interrupt, data ready
	uint8_t click_cfg_reg_value = 0x30;															//latch interrupt request to CLICK_SRC reg, and reset CLICK_SRC to 0 when read
	//uint8_t click_thsz_reg_value = 0x02;													//
	
	uint8_t click_window_reg_value = 0x7F;													//max time interval after end of latency interval where click detection can start again if device configured for double click detect
	uint8_t click_latency_reg_value = 0x7F;													//time interval where click detection is disabled after first click, if configured for double click detection
	uint8_t click_time_limit_reg_value = 0x03;											//maximum time interval that can elapse between the start of the click detect procedure and when the accel goes back below threshold
	uint8_t click_threshold_z_value = 0x0A;													//z-axis sensitivity threshold
	uint8_t click_threshold_xy_value = 0xAA;												//x&y axis sensitivity thresholld
	
	//Accelerometer Configuration
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; 																		//Either lowpower on or off
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100; 																			//options are 100Hz or 400Hz, we are sampling at 25Hz, no sense using faster freq
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE; 			//Turn on all 3 axes
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3; 																					//Define typical range of measured values, doubtfull we'll hit 9g
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL; 																				//Off, otherwise apply actuation force, sensors change their DC levels (way to make sure everything is working)
	LIS302DL_Init(&LIS302DL_InitStruct);
	
	//Accel Interrupt Config
	LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_NOTLATCHED; //wrong? and overwritten
  LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_XYZ_ENABLE; //overwritten
	LIS302DL_InterruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_ENABLE;//overwritten
  LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);
	
	//Specifiy value to write, register to write it too, and the number of bytes to be written
	LIS302DL_Write(&int_ctrl_reg_value, LIS302DL_CTRL_REG3_ADDR, sizeof(int_ctrl_reg_value));					//Enable click interrupt on INT0
	LIS302DL_Write(&click_cfg_reg_value, LIS302DL_CLICK_CFG_REG_ADDR, sizeof(click_cfg_reg_value));		//configure single/double click
  LIS302DL_Write(&click_threshold_xy_value, LIS302DL_CLICK_THSY_X_REG_ADDR, 1); 	    							//Configure Click Threshold on X/Y axis (10 x 0.5g)
  LIS302DL_Write(&click_threshold_z_value, LIS302DL_CLICK_THSZ_REG_ADDR, 1);												//Configure Click Threshold on Z axis (10 x 0.5g)
  LIS302DL_Write(&click_time_limit_reg_value, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);								//Configure Time Limit
  LIS302DL_Write(&click_latency_reg_value, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);											//Configure Latency
  LIS302DL_Write(&click_window_reg_value, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);    										//Configure Click Window

}