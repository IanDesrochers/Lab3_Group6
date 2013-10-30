#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"
#include "lab3_accelerometer.h"
#include "lab3_filter.h"

#define THRESHOLD 4

//Define required offsets as well, post calibration
static uint32_t tim2_interrupt = 0;
static uint32_t tim3_interrupt = 0;
static uint32_t tim5_interrupt = 0;
static uint32_t exti_interrupt = 0;
static uint32_t tap_interrupt = 0;
static uint32_t mode = 0;
static uint32_t is_initialized = 1;

int main()
{
	//Struct to hold orientation data from accelerometer
	struct Orientation orientation;
	struct Moving_Average moving_average_pitch;
	struct Moving_Average moving_average_roll;
	
	//Variables
	uint8_t x_positive_speed, x_negative_speed, y_positive_speed, y_negative_speed;
	uint8_t x_positive_count, x_negative_count, y_positive_count, y_negative_count;
	uint8_t tim3_interrupt_count;
	uint32_t pwm_intensity, pwm_direction, max_pwm_intensity = 100;
	
	//Calls to initialize used peripherals
	init_TIM2();
	init_TIM3();
	init_accel();
	init_leds();
	init_EXTI();
	init_TIM4_PWM();
	init_TIM5();
	
	//Struct Initialization
	init_moving_average(&moving_average_pitch, MOVING_AVERAGE_FILTER_SIZE);
	init_moving_average(&moving_average_roll, MOVING_AVERAGE_FILTER_SIZE);
	
	while(1) {
		//if (!mode) {
		if (tap_interrupt!=0) {
			tap_interrupt = 0;
			if (mode) {
				mode = 0;
				init_leds();
			} else {
				mode = 1;
				init_LED_PWM();
				//init_TIM4_PWM();
			}
		}
		if (!mode) {
			if (!is_initialized) {
				init_leds();
				is_initialized = 1;
			}
			if (tim2_interrupt!=0) {
				tim2_interrupt=0;
				getOrientation(&orientation);
				insert_value(&moving_average_pitch,orientation.pitch);
				insert_value(&moving_average_roll,orientation.roll);

				calculate_average(&moving_average_pitch);
				calculate_average(&moving_average_pitch);
				
				printf("%f\t%f\t%f\n", orientation.pitch, orientation.roll, orientation.yaw);
			}
			
			if (tim3_interrupt!=0) {
				tim3_interrupt=0;
				if (x_positive_count==x_positive_speed) {
					GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
				}
				if (x_negative_count==x_negative_speed) {
					GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
				}
				if (y_positive_count==y_positive_speed) {
					GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
				}
				if (y_negative_count==y_negative_speed) {
					GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				}
				x_positive_count++;
				x_negative_count++;
				y_positive_count++;
				y_negative_count++;
				
				if (tim3_interrupt_count==89) {
					tim3_interrupt_count=0;
					x_positive_count=0;
					x_negative_count=0;
					y_positive_count=0;
					y_negative_count=0;
					if (orientation.pitch>=0) {
						x_positive_speed=90-orientation.pitch;
						x_negative_speed=90;
					} else {
						x_positive_speed=90;
						x_negative_speed=90+orientation.pitch;
					}
					if (orientation.roll>=0) {
						y_positive_speed=90-orientation.roll;
						y_negative_speed=90;
					} else {
						y_positive_speed=90;
						y_negative_speed=90+orientation.roll;
					}
					GPIO_Write(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				} else {
					tim3_interrupt_count++;
				}
			}
			if (exti_interrupt!=0) {																								//
				uint8_t data;																													//
				LIS302DL_Read(&data, 0x3C, 1);																				//
				exti_interrupt=0;																											//
				printf("Tap!\t%x\n", data);																						//
			}
		} else {
			if (tim5_interrupt!=0) {
				tim5_interrupt = 0;
				printf("A\n");
				if (pwm_intensity == max_pwm_intensity) {
					pwm_direction = 1;
				} else if (pwm_intensity == 0) {
					pwm_direction = 0;
				}
				if (pwm_direction) {
					pwm_intensity--;
				} else {
					pwm_intensity++;
				}
				TIM4->CCR1 = pwm_intensity;
				TIM4->CCR2 = pwm_intensity;
				TIM4->CCR3 = pwm_intensity;
				TIM4->CCR4 = pwm_intensity;
			}
		}			
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

void TIM3_IRQHandler(void)																									//
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)												//
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);															//
		tim3_interrupt++;																												//
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

