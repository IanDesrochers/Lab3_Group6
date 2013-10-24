#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

#include "lab3_init.h"

static void TIM3_Config(void);

//Define required offsets as well, post calibration


int main()
{
	
	init_TIM();
	init_accel();
	init_leds();
	
	
	while(1) {

		
		
	}
	
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearFlag(TIM2, TIM_IT_Update);
  }
}