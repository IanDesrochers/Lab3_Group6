/**
  ******************************************************************************
  * @file    lab3_pwm.c
  * @author  Group 6
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   Provides driver functions for hardware PWM control using
	*          output compare on TIM4
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"

/* Defines ------------------------------------------------------------------*/

#ifndef MAX_PWM_INTENSITY
#define MAX_PWM_INTENSITY 500																													//Number of steps between fully off and fully on
#endif

#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY 500																															//Driving PWM frequency (Hz)
#endif

#ifndef PWM_PULSE_SPEED
#define PWM_PULSE_SPEED 2000																													//Time of a complete pulse cycle, off to off (in ms)
#endif

#ifndef PWM_UPDATE_INTENSITY_FREQUENCY
#define PWM_UPDATE_INTENSITY_FREQUENCY 5*MAX_PWM_INTENSITY														//Frequency at which to update intensity
#endif

/* Exported Types ---------------------------------------------------------*/

/** @defgroup Structs
  * @{
  */

#ifndef LED_PWM_STRUCT
#define LED_PWM_STRUCT

struct LED_PWM {
	uint32_t pwm_pulse_speed;
	uint32_t pwm_intensity;
	uint32_t max_pwm_intensity;
	uint32_t pwm_direction;
	uint32_t led_pwm_update_pulse_count;
	uint32_t CCR;
	TIM_TypeDef* TIMx;
};

#endif

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */ 

void init_PWM(void);
void init_LED_PWM(struct LED_PWM *led_pwm, uint32_t max_pwm_intensity, uint32_t pwm_pulse_speed, uint32_t phase, uint32_t CCR, TIM_TypeDef* TIMx);
void update_led_pwm_intensity_pulse(struct LED_PWM *led_pwm);
void update_led_intensities(uint32_t led_intensities[], uint32_t length, TIM_TypeDef* TIMx);

/**
  * @}
  */
