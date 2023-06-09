#include "bsp_SysTick.h"

__IO u32 TimingDelay;
__IO u32 TimingDelay1;
__IO u32 TimingDelay2;
__IO u32 TimingDelay3 = 1000;

 
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms
	 * SystemFrequency / 100000	 10us
	 * SystemFrequency / 1000000 1us
	 */
	if (HAL_SYSTICK_Config(SystemCoreClock / 1000))
	//if(HAL_SYSTICK_Config(SystemCoreClock * 10))
	{ 
		/* Capture error */ 
		while (1);
	}
}

void TimingDelay_Increment(void)
{
	TimingDelay++;
	TimingDelay1++;
	TimingDelay2++;
	TimingDelay3--;
}

/*********************************************END OF FILE**********************/
