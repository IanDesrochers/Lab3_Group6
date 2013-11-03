/**
  ******************************************************************************
  * @file    main.c
  * @author  Group 6
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   Main entry point for Lab3 - Tilt angle accelerometer application
  */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"
#include "lab3_filter.h"
#include "lab3_pwm.h"
#include "lab3_orientation.h"

/* Private Variables ------------------------------------------------------------------*/

static uint32_t tim2_interrupt = 0;
static uint32_t tim5_interrupt = 0;
static uint32_t tap_interrupt = 0;
static uint32_t mode = 0;

/* Private Functions ---------------------------------------------------------*/

/** @defgroup Private_Functions
  * @{
  */

/**
  * @brief  Updates LED intensity in pulse mode
	* @note   This function is necessary to decouple the PWM pulse speed
	*         from the PWM driving frequency
	* @param  *led_pwm: LED_PWM struct of an LED
  * @retval None
  */

static void display_led_pulse(struct LED_PWM *led_pwm) {
	if (led_pwm->led_pwm_update_pulse_count >= PWM_UPDATE_INTENSITY_FREQUENCY*led_pwm->pwm_pulse_speed/led_pwm->max_pwm_intensity/1000) {
		update_led_pwm_intensity_pulse(led_pwm);
		led_pwm->led_pwm_update_pulse_count = 0;
	}
	led_pwm->led_pwm_update_pulse_count++;
}

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */

/**
  * @brief  Change mode between tilt angle reading to PWM LED pulse
	* @param  None
  * @retval None
  */

void change_mode() {																													//Change mode function, 0 = tilt, 1=PWM
	if (mode) {
		mode = 0;
	} else {
		mode = 1;
	}
}

/**
  * @brief  Reads accelerometer to get orientation and updates LEDs to display it
	* @param  *orientation: pointer to an Orientation struct representing the accelerometer
  * @retval None
  */

void display_orientation(struct Orientation *orientation) {
	update_orientation(orientation);																																													//Update Orientation with new accelerometer values
	printf("Pitch: %fd\tRoll: %fd\n", orientation->moving_average_pitch.average, orientation->moving_average_roll.average); 	//Print new orientation to debugger stream
	if (!mode) {																																																							//Check if we're in tilt angle detection mode
		uint32_t new_led_intensities[4];
		if (orientation->moving_average_roll.average >= 0) {
			new_led_intensities[2] = orientation->moving_average_roll.average * MAX_PWM_INTENSITY / 90;														//Intensity based off of ratio of angle to 90d
			new_led_intensities[0] = 0;																																														//Also ensures that opposing LEDs are not on at the same time
		} else {
			new_led_intensities[2] = 0;
			new_led_intensities[0] = -orientation->moving_average_roll.average * MAX_PWM_INTENSITY / 90;					
		}
		if (orientation->moving_average_pitch.average >= 0) {
			new_led_intensities[3] = orientation->moving_average_pitch.average * MAX_PWM_INTENSITY / 90;					
			new_led_intensities[1] = 0;
		} else {
			new_led_intensities[3] = 0;
			new_led_intensities[1] = -orientation->moving_average_pitch.average * MAX_PWM_INTENSITY / 90;					
		}
		update_led_intensities(new_led_intensities, sizeof(new_led_intensities)/sizeof(new_led_intensities[0]), TIM4);								//Update LED intensities
	}
}

/**
  * @brief  Main entry point
	* @param  None
  * @retval int: Error code
  */

int main()
{
	struct Orientation orientation;																							//Struct to hold orientation data from accelerometer
	struct LED_PWM led_pwm_1, led_pwm_2, led_pwm_3, led_pwm_4;									//Structs for 4 LEDs
	
	//Calls to initialize used peripherals
	init_accelerometer();																												//Initialize accelerometer
	init_TIM2();																																//Initialize TIM2 as acclerometer sampling timer
	init_EXTI();																																//Initialize EXTI for tap detection
	init_PWM();																																	//Initialize LEDs and HW timer for PWM LED operation
	init_sample_rate_test();																										//Initalize GPIO for accelerometer sample rate measurement
	
	//Struct Initialization
	init_orientation(&orientation);
	init_LED_PWM(&led_pwm_1, MAX_PWM_INTENSITY, PWM_PULSE_SPEED, 0, 1, TIM4);					//initialize with values for the specified LED, with intensity, pulse speed, phase, and capture compare register (CCR)
	init_LED_PWM(&led_pwm_2, MAX_PWM_INTENSITY, PWM_PULSE_SPEED, 125, 2, TIM4);				//initialize with values for the specified LED, with intensity, pulse speed, phase, and capture compare register (CCR)
	init_LED_PWM(&led_pwm_3, MAX_PWM_INTENSITY, PWM_PULSE_SPEED, 250, 3, TIM4);				//initialize with values for the specified LED, with intensity, pulse speed, phase, and capture compare register (CCR)
	init_LED_PWM(&led_pwm_4, MAX_PWM_INTENSITY, PWM_PULSE_SPEED, 375, 4, TIM4);				//initialize with values for the specified LED, with intensity, pulse speed, phase, and capture compare register (CCR)
	
	while(1) {
		if (tap_interrupt != 0) {																									//Check if we've received a tap interrupt
			tap_interrupt = 0;																											//Reset interrupt flag
			change_mode();																													//Change mode from displaying tilt angle to PWM or vice versa
		}
		if (tim2_interrupt != 0) {																								//Check if we've received a TIM2 interrupt
			tim2_interrupt = 0;																											//Reset interrupt flag
			GPIO_ToggleBits(GPIOD, GPIO_Pin_0);																			//Toggle sample rate measurement pin
			display_orientation(&orientation);																			//Do an orientation reading
		}
		if (tim5_interrupt != 0) {																								//Check if we've received a TIM5 interrupt
			tim5_interrupt = 0;																											//Reset interrupt flag
			if (mode) {																															//Check if we're in PWM pulse mode
				display_led_pulse(&led_pwm_1);																				//Update LED intensities
				display_led_pulse(&led_pwm_2);
				display_led_pulse(&led_pwm_3);
				display_led_pulse(&led_pwm_4);
			}
		}
	}
}

/**
  * @brief  IRQ handler for TIM2
	* @param  None
  * @retval None
  */

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)													//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);																//Reset interrupt pending bit
		tim2_interrupt++;																													//Increment TIM2 interrupt flag
  }
}

void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)													//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);																//Reset interrupt pending bit
		tim5_interrupt++;																													//Increment TIM5 interrupt flag
  }
}

void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)																		//Checks interrupt status register to ensure an interrupt is pending
  {
    EXTI_ClearITPendingBit(EXTI_Line1);																				//Reset interrupt pending bit
		tap_interrupt++;																													//Increment tap interrupt flag
  }
}

/**
  * @}
  */
