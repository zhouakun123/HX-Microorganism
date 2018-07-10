#include "stm32f2xx.h"
/********************************************************\
* @brief  IWDG Config
* @param  None
* @retval None
* @note   Counter Reload Value = 2000ms/IWDG counter clock period
* @note                        = 2000ms / (32/LSI)
* @note                        = 2s / (32/LSI)
* @note                        = LSI/(32 * 2)
* @note                        = LSI/64
* @note   				 LSI = 40000Hz = 40KHz 
\********************************************************/
void IWDG_Config(void)
{ 
	// Enable write access to IWDG_PR and IWDG_RLR registers 
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	
	// IWDG counter clock: LSI/32 = 1.25KHz
   	IWDG_SetPrescaler(IWDG_Prescaler_32);			
	//Set counter reload value to obtain 1000ms IWDG TimeOut
  	IWDG_SetReload(40000/64);
	// Reload IWDG counter 
   	IWDG_ReloadCounter();             		
	// Enable IWDG (the LSI oscillator will be enabled by hardware)	
   	IWDG_Enable();                       			
}
