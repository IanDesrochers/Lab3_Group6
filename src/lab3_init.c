#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"

void init_leds() {
	GPIO_InitTypeDef gpio_init_s;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 															//Enable clock to LEDs
	
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 			//Initialize 4 LEDs for use, attached to these pins
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;																							//we want these pins to be outputs
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;																					//clock freq to pin
	gpio_init_s.GPIO_OType = GPIO_OType_PP;																							//push-pull, instead of open drain
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL;																						//sets pulldown resistors to be inactive
	GPIO_Init(GPIOD, &gpio_init_s);																											//Initializes the peripherals with the specified params
	
	/*GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);													//GPIO_AF_TIM4: Connect TIM4 pins to AF2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);*/
}

void init_TIM2() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 																							//Enable clock to TIM2
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																										//no clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;																			//count down
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;																												//max period available (2^16-1)
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/(2*25*TIM_TimeBaseStructure.TIM_Period)-1;	//set prescaler, clock now at sample rate of 25Hz
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;																								//Once counter reaches this value we restart RCR count
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);																										
	
	TIM_Cmd(TIM2, ENABLE);																															//enable TIM2 peripheral
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);																					//enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;																			//specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 												//indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 															//subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																		//enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																											//initialize struct parameters into tim2 nvic
}

void init_TIM3() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 																//Enable clock to TIM3
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																											//no clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;																				//counts down
	TIM_TimeBaseStructure.TIM_Period = 0x00FF;																													//max period available (2^16-1)
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/(2*900*TIM_TimeBaseStructure.TIM_Period))-1;		//set prescaler
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;																									//restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//****************************************		//initialize struct parameters to TIM3
	
	TIM_Cmd(TIM3, ENABLE);																																							//enable specified peripheral
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);																													//enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;																			//specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 												//indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 															//subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																		//enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																											//initialize NVIC for TIM3 with struct params
}

void init_EXTI() {
	GPIO_InitTypeDef GPIO_InitStructure;																								//initialize GPIO structure
	EXTI_InitTypeDef EXTI_InitStructure;																								//initialize external interrupt structure
	NVIC_InitTypeDef NVIC_InitStructure;																								//initialize nested vector interrupt controller structure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);															//enable apb2 clock to syscfg, enable external interrupts
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);																//enable ahb1 clock to gpioe peripherals
	
	GPIO_StructInit(&GPIO_InitStructure);																								//define GPIO struct parameters
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;																						//assign pin 1 of the GPIO set to our struct
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																				//we want these pins to be outputs
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																		//clock freq to pin
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																			//push-pull, instead of open drain
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																		//sets pulldown resistors to be inactive
	GPIO_Init(GPIOE, &GPIO_InitStructure);																							//initialize GPIO with struct parameters

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);												//connect/configure external interrupt on pin1 to be connected with gpioe
	
	EXTI_StructInit(&EXTI_InitStructure);																								//reset external interrupt structure
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;																					//ext interrupt coming in on line 1
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;																	//set mode of the interrupt, external
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  														//ext interrupt triggered on a rising edge
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;																						//enable new state of the interrupt
  EXTI_Init(&EXTI_InitStructure);																											//configure external interrupt with defined structure parameters
	
	/*The Nested Vectored Interrupt Controller (NVIC) offers very fast interrupt handling and provides the vector table as a set of real vectors (addresses).
	-Saves and restores automatically a set of the CPU registers (R0-R3, R12, PC, PSR, and LR).
	-Does a quick entry to the next pending interrupt without a complete pop/push sequence.
	-Serves a complete set of 255 (240 external) interrupts.
	*/
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;																		//enable interrupt request channel
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;												//set priority of pre-emption interrupt
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;																//set sub priority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																			//enable the IRQ channel specified by nvic
  NVIC_Init(&NVIC_InitStructure);																											//
}

void init_LED_PWM() {
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4); 
}

void init_TIM4_PWM() {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 0x63;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00FF;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  TIM_Cmd(TIM4, ENABLE);
}

void init_TIM5() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 																//Enable clock to TIM3
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																											//no clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;																				//counts down
	TIM_TimeBaseStructure.TIM_Period = 0x0FFF;																													//max period available (2^16-1)
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/(2*50*TIM_TimeBaseStructure.TIM_Period))-1;		//set prescaler
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;																									//restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);				//****************************************		//initialize struct parameters to TIM3
	
	TIM_Cmd(TIM5, ENABLE);																																							//enable specified peripheral
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);																													//enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;																			//specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 												//indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 															//subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																		//enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																											//initialize NVIC for TIM3 with struct params
}
