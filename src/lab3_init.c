#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"

uint16_t PrescalerValue;

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
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);														//GPIO_AF_TIM4: Connect TIM4 pins to AF2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
}






void init_accel() {
	LIS302DL_InitTypeDef LIS302DL_InitStruct; 																											//Struct for initialization
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct; 																			//struct for interrupt configuration
	
	//Accelerometer Configuration
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; 																	//Either lowpower on or off
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100; 																		//options are 100Hz or 400Hz, we are sampling at 25Hz, no sense using faster freq
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE; 		//Turn on all 3 axes
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3; 																				//Define typical range of measured values, doubtfull we'll hit 9g
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL; 																			//Off, otherwise apply actuation force, sensors change their DC levels (way to make sure everything is working)
	LIS302DL_Init(&LIS302DL_InitStruct);
	
	//Accel Interrupt Config
	//(We'll be using the single tap interrupt later, doing config/calibration for now.)
//	LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
//  LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_SINGLECLICKINTERRUPT_Z_ENABLE;
//  LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);
	
	
	
	
}




void init_TIM() {
	//NVIC always little endian
  NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 																		//Enable clock to TIM4
	
	PrescalerValue = (uint16_t) (SystemCoreClock/something) - 1 = 

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;																					//specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 															//indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 																			//subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 																				// enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 
	TIM_TimeBaseStructure.TIM_CounterMode = 
	TIM_TimeBaseStructure.TIM_Period =
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalarValue;
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = ;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//To get TIM3 counter clock at 2 KHz, the prescaler is computed as follows:
	//Prescaler = (TIM4CLK / TIM1 counter clock) - 1
	//Prescaler = (168 MHz/(2 * 2 KHz)) - 1 = 41999
	
}











