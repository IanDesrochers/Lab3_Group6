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

#include <math.h>
#include "stm32f4xx.h"
#include "lab3_pwm.h"

/* Defines ------------------------------------------------------------------*/

#ifndef PI
#define PI 3.14159
#endif

/* Private Functions ---------------------------------------------------------*/

/** @defgroup Private_Functions
  * @{
  */

/**
  * @brief  Initialize LEDs for use with TIM4 OC hardware PWM
	* @param  None
  * @retval None
  */

static void init_LEDS() {
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);																			//Enable clock to GPIOD
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(GPIOD, &GPIO_InitStructure);																										//Pass struct and initialize
 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);																	//Alternate function configuration for GPIO poins, hardware link to TIM4
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4); 
}

/**
  * @brief  Initialize TIM4 as PWM clock, using output compare
	* @param  None
  * @retval None
  */

static void init_TIM4() {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);																			//Clock enable to TIM4
	
	TIM_TimeBaseStructure.TIM_Period = MAX_PWM_INTENSITY;																															//Period = max intensity for pwm
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / (2 * PWM_FREQUENCY * TIM_TimeBaseStructure.TIM_Period);		//Set prescaler for LED function
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;																																			//No clock divison
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;																												//Count up

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;																					//Set Output Compare mode for TIM4
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;															//Enable output
	TIM_OCInitStructure.TIM_Pulse = 0;																												//Set initial CCR value
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;																	//Set output active high

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC2
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC3
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR3
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC4
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR4
	
  TIM_ARRPreloadConfig(TIM4, ENABLE);																												//Enable TIM4 Auto Reload Register 

  TIM_Cmd(TIM4, ENABLE);																																		//Enable TIM4
}

/**
  * @brief  Initialize TIM5 as PWM intensity update clock
	* @param  None
  * @retval None
  */

static void init_TIM5() {
  NVIC_InitTypeDef NVIC_InitStructure;																											//NVIC initialization struct
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																						//Timer initialization struct
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 																			//Enable clock to TIM5
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																																											//No clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;																																					//Counts up
	TIM_TimeBaseStructure.TIM_Period = 0x007F;																																													//Period set for largest dynamic range of standard timer frequencies to be used
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / (2 * PWM_UPDATE_INTENSITY_FREQUENCY * TIM_TimeBaseStructure.TIM_Period);		//Set prescaler to determine frequency
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;																																									//Restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);																																											//Initialize struct parameters to TIM5
	
	TIM_Cmd(TIM5, ENABLE);																																		//Enable specified peripheral
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);																								//Enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;																						//Specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 															//Indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 																		//Subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																					//Enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																														//Initialize NVIC for TIM3 with struct params
}

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */

/**
  * @brief  Perform initialization of PWM driver
	* @param  None
  * @retval None
  */

void init_PWM() {
	init_LEDS();																																							//Initialize LEDs
	init_TIM4();																																							//Initialize TIM4
	init_TIM5();																																							//Initialize TIM5
}

/**
  * @brief  Initializes LED_PWM struct with default values
	* @param  *led_pwm: Pointer to LED_PWM struct to initialize
	* @param  max_pwm_intensity: Number of intensity steps from off to fully on
	* @param  pwm_pulse_speed: Length of time for LED to complete a pulse cycle, off to off (in ms)
	* @param  phase: Initial LED intensity
	* @param  CCR: Capture compare register for output compare
	* @param  TIMx: Timer to connect LED to
  * @retval None
  */

void init_LED_PWM(struct LED_PWM *led_pwm, uint32_t max_pwm_intensity, uint32_t pwm_pulse_speed, uint32_t phase, uint32_t CCR, TIM_TypeDef* TIMx) {		//Initialize LED with parameters
	led_pwm->pwm_pulse_speed = pwm_pulse_speed;																																																					//Pulse speed, defined constant
	led_pwm->pwm_intensity = phase;																																																											//In-sync vs out of sync pwm
	led_pwm->max_pwm_intensity = max_pwm_intensity;																																																			//Defined constant
	led_pwm->pwm_direction = 0;																																																													//Initialize to 0
	led_pwm->CCR = CCR;																																																																	//Gets specified capture&control register
	led_pwm->led_pwm_update_pulse_count = 0;																																																						//Initialize to 0
	led_pwm->TIMx = TIMx;																																																																//LED is on which TIM?
}

/**
  * @brief  Updates intensity of an LED_PWM.
	* @note   This function should be called periodically, at the desired
	*         frequency that the LED should increment/decrement intensity by 1
	* @param  *led_pwm: Pointer to LED_PWM struct to update intensity of
  * @retval None
  */

void update_led_pwm_intensity_pulse(struct LED_PWM *led_pwm) {
	if (led_pwm->pwm_intensity == led_pwm->max_pwm_intensity) {
		led_pwm->pwm_direction = 1;																															//If the LED is fully on, change pulse direction to start dimming
	} else if (led_pwm->pwm_intensity == 0) {
		led_pwm->pwm_direction = 0;																															//If the LED is fully off, change the pulse direction to brighten
	}
	if (led_pwm->pwm_direction) {																															//Increment or decrement LED intensity according to direction we should be going in
		led_pwm->pwm_intensity--;
	} else {
		led_pwm->pwm_intensity++;
	}
	
	uint32_t real_pwm_intensity = led_pwm->max_pwm_intensity * pow(0.5f*(-cos(2*PI*(float)led_pwm->pwm_intensity / led_pwm->max_pwm_intensity)+1), 2); //Smooth PWM intensity function
	
	switch (led_pwm->CCR) {																																		//Update corresponding CCR with the new intensity value
		case 1:
			TIM_SetCompare1(TIM4, real_pwm_intensity);
			break;
		case 2:
			TIM_SetCompare2(TIM4, real_pwm_intensity);
			break;
		case 3:
			TIM_SetCompare3(TIM4, real_pwm_intensity);
			break;
		case 4:
			TIM_SetCompare4(TIM4, real_pwm_intensity);
			break;
		default:
			break;
	}
}

/**
  * @brief  Updates intensities of all LEDs simultaneously
	* @param  led_intensities[]: Array of intensity values to write to LEDs
	* @param  length: Length of led_intensities array
	* @param  TIMx: TIM to write CCR values to
  * @retval None
  */

void update_led_intensities(uint32_t led_intensities[], uint32_t length, TIM_TypeDef* TIMx) {
	if (length == 4)
	{
		TIM_SetCompare1(TIMx, led_intensities[0]);									//set the 4 TIM4 capture compare register values
		TIM_SetCompare2(TIMx, led_intensities[1]);									//based off LED intensity
		TIM_SetCompare3(TIMx, led_intensities[2]);
		TIM_SetCompare4(TIMx, led_intensities[3]);
	}
}

/**
  * @}
  */
