#include <stdio.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"
#include "lab3_filter.h"
#include "lab3_pwm.h"
#include "lab3_orientation.h"

//Define required offsets as well, post calibration
static uint32_t tim2_interrupt = 0;
static uint32_t tim5_interrupt = 0;
static uint32_t tap_interrupt = 0;
static uint32_t mode = 0;

void change_mode(void);
void display_orientation(struct Orientation *orientation);

int main()
{
	//Struct to hold orientation data from accelerometer
	struct Orientation orientation;
	struct LED_PWM led_pwm_1, led_pwm_2, led_pwm_3, led_pwm_4;
	
	//Calls to initialize used peripherals
	init_PWM();
	init_accelerometer();
	init_EXTI();
	init_TIM2();
	
	//Struct Initialization
	init_orientation(&orientation);
	init_LED_PWM(&led_pwm_1, MAX_PWM_INTENSITY);
	init_LED_PWM(&led_pwm_2, MAX_PWM_INTENSITY);
	init_LED_PWM(&led_pwm_3, MAX_PWM_INTENSITY);
	init_LED_PWM(&led_pwm_4, MAX_PWM_INTENSITY);
	
	while(1) {
		if (tap_interrupt!=0) {
			tap_interrupt = 0;
			change_mode();
		}
		if (tim2_interrupt!=0) {
			GPIO_ToggleBits(GPIOD, GPIO_Pin_0);
			tim2_interrupt = 0;
			display_orientation(&orientation);
		}
		if (tim5_interrupt!=0) {
			if (mode) {
				tim5_interrupt = 0;
				update_led_pwm_intensities_pulse(&led_pwm_1);
				update_led_pwm_intensities_pulse(&led_pwm_2);
				update_led_pwm_intensities_pulse(&led_pwm_3);
				update_led_pwm_intensities_pulse(&led_pwm_4);
			}
		}			
	}
}

void change_mode() {
	if (mode) {
		mode = 0;
	} else {
		mode = 1;
	}
}

void display_orientation(struct Orientation *orientation) {
	update_orientation(orientation);
	printf("%f\t%f\n", orientation->moving_average_pitch.average, orientation->moving_average_roll.average);
	if (!mode) {
		uint32_t new_led_intensities[4];
		if (orientation->moving_average_roll.average >= 0) {
			new_led_intensities[2] = orientation->moving_average_roll.average * MAX_PWM_INTENSITY / 90;
			new_led_intensities[0] = 0;
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
		update_led_intensities(new_led_intensities, sizeof(new_led_intensities)/sizeof(new_led_intensities[0]));
	}
}

void TIM2_IRQHandler(void)																									//
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)												//
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);															//
		tim2_interrupt++;																												//
  }
}

void TIM5_IRQHandler(void)																									//
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)												//
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);															//
		tim5_interrupt++;																												//
  }
}

void EXTI1_IRQHandler(void)																									//
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)																	//
  {
    EXTI_ClearITPendingBit(EXTI_Line1);																			//
		tap_interrupt++;
  }
}
