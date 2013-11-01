#include "stm32f4xx.h"

#include "lab3_init.h"

void init_TIM2() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//create NVIC struct for holding parameters
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//create TIM struct for holding timer parameters
	
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
	
	// Preemption Priority = used to determine if an interrupt that occurs after can overtake
	// previous interrupt that is currently being serviced
	// SubPriority = used to determine priority if two interrupts occur at the same time
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;																		//enable interrupt request channel
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;												//set priority of pre-emption interrupt
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;																//set sub priority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																			//enable the IRQ channel specified by nvic
  NVIC_Init(&NVIC_InitStructure);																											//pass struct to NVIC, initialize
}

void init_sample_rate_test() {
	GPIO_InitTypeDef gpio_init_s;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 															//Enable clock to LEDs
	
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Pin = GPIO_Pin_0; 																									//Initialize 4 LEDs for use, attached to these pins
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;																							//we want these pins to be outputs
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;																					//clock freq to pin
	gpio_init_s.GPIO_OType = GPIO_OType_PP;																							//push-pull, instead of open drain
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL;																						//sets pulldown resistors to be inactive
	GPIO_Init(GPIOD, &gpio_init_s);
}
