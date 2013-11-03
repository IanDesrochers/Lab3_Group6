/**
  ******************************************************************************
  * @file    lab3_init.h
  * @author  Group 6
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   This file provides initialization functions for the following:
	*           - TIM2 (HW Timer for accelerometer sampling)
	*           - EXTI (External interrupt for tap detection)
	*           - GPIO initialization for measurement of accelerometer sampling rate
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"

/* GPIO Init Public Functions ---------------------------------------------------------*/

/** @defgroup GPIO_Init_Public_Functions
  * @{
  */

void init_TIM2(void);
void init_EXTI(void);
void init_sample_rate_test(void);

/**
  * @}
  */
