#include <math.h>
#include "stm32f4xx.h"

#include "lab3_pwm.h"

#ifndef PI
#define PI 3.14159
#endif

void init_LEDS(void);
void init_TIM4(void);
void init_TIM5(void);

void init_PWM() {
	init_LEDS();
	init_TIM4();
	init_TIM5();
}

void init_LED_PWM(struct LED_PWM *led_pwm, uint32_t max_pwm_intensity, uint32_t pwm_pulse_speed, uint32_t phase, uint32_t CCR) {
	led_pwm->pwm_pulse_speed = pwm_pulse_speed;
	led_pwm->pwm_intensity = phase;
	led_pwm->max_pwm_intensity = max_pwm_intensity;
	led_pwm->pwm_direction = 0;
	led_pwm->CCR = CCR;
	led_pwm->led_pwm_update_pulse_count = 0;
}

void update_led_pwm_intensity_pulse(struct LED_PWM *led_pwm) {
	if (led_pwm->pwm_intensity == led_pwm->max_pwm_intensity) {
		led_pwm->pwm_direction = 1;
	} else if (led_pwm->pwm_intensity == 0) {
		led_pwm->pwm_direction = 0;
	}
	if (led_pwm->pwm_direction) {
		led_pwm->pwm_intensity--;
	} else {
		led_pwm->pwm_intensity++;
	}
	uint32_t real_pwm_intensity = led_pwm->max_pwm_intensity * pow(0.5f*(-cos(2*PI*(float)led_pwm->pwm_intensity / led_pwm->max_pwm_intensity)+1), 2);
	switch (led_pwm->CCR) {
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
	//uint32_t led_intensities[] = {real_pwm_intensity, real_pwm_intensity, real_pwm_intensity, real_pwm_intensity};
	//update_led_intensities(led_intensities, sizeof(led_intensities)/sizeof(led_intensities[0]));
}

void update_led_intensities(uint32_t led_intensities[], uint32_t length) {
	if (length == 4)
	{
		TIM_SetCompare1(TIM4, led_intensities[0]);									//set the 4 TIM4 capture compare register values
		TIM_SetCompare2(TIM4, led_intensities[1]);									//based off LED intensity
		TIM_SetCompare3(TIM4, led_intensities[2]);
		TIM_SetCompare4(TIM4, led_intensities[3]);
	}
}

void init_LEDS() {																							//initialize LEDs
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	//specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;																					//alternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																			//not pull up or pull down
  GPIO_Init(GPIOD, &GPIO_InitStructure);																								//pass struct and initialize
 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);											//Alternate function configuration for GPIO poins, hardware link to TIM4
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4); 
}

void init_TIM4() {																															//Timer for PWM
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;																//timer struct
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				//output compare struct
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);													//clock enable to Tim4
	
	TIM_TimeBaseStructure.TIM_Period = MAX_PWM_INTENSITY;													//period=max intensity for pwm
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / (2 * PWM_FREQUENCY * TIM_TimeBaseStructure.TIM_Period);		//
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;																	//no divisons
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;										//count up to max intensity

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);																		//Pass output compare structure parameters

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);														//Enables TIM4 peripheral Preload register on CCR1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  TIM_Cmd(TIM4, ENABLE);
}

void init_TIM5() {																																		//Timer for scaling down clock for TIM4
  NVIC_InitTypeDef NVIC_InitStructure;																								//nvic struct
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//timer struct
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 																//Enable clock to TIM5
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																											//no clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;																				//counts down
	TIM_TimeBaseStructure.TIM_Period = 0x007F;																													//max period available (2^16-1)
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / (2 * PWM_UPDATE_INTENSITY_FREQUENCY * TIM_TimeBaseStructure.TIM_Period);		//set prescaler
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;																									//restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);				//****************************************		//initialize struct parameters to TIM5
	
	TIM_Cmd(TIM5, ENABLE);																																							//enable specified peripheral
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);																													//enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;																			//specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 												//indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 															//subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																		//enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																											//initialize NVIC for TIM3 with struct params
}
