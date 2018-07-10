#include "stm32f2xx.h"
#include "delay.h"                                
static u8  fac_us = 0;  // us time delay multiplier		   
static u16 fac_ms = 0;  // ms time delay multiplier
/********************************************************\
* @brief  Delay init
* @param  SYSCLK - System Clock
* @retval None
* @note   SYSTICK clock is fixed to the 1/8 clock HCLK		
\********************************************************/
void delay_init(u8 SYSCLK)
{
    // SYSTICK uses an external clock source
    SysTick->CTRL&=~(1<<2);					
    fac_us = SYSCLK/8;				
    // Represents each MS  required for the number of systick clocks
    fac_ms = (u16)fac_us*1000;				
}
/********************************************************\
* @brief  Delay nus
* @param  nus - us number to delay
* @retval None
* @note   None	
\********************************************************/
void delay_us(u16 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD = nus*fac_us; // Time loading  		 
	SysTick->VAL = 0x00;    // Clear Counter
	SysTick->CTRL = 0x01;  // Start countdown 
	do
	{
		temp = SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));    // Waiting time to arrive
	SysTick->CTRL = 0x00;   // Off Counter
	SysTick->VAL = 0X00;    // Clear Counter	 
}
/********************************************************\
* @brief  Delay nms
* @param  nms - ms number to delay
* @retval None
* @note   Be careful range of nms
* @note   SysTick->LOAD for 24 bit registers,So,the maximum delay is:
* @note   nms <= 0xffffff*8*1000/SYSCLK
* @note   SYSCLK units for the Hz,nms units for the MS
* @note   Under the 72M condition, nms <= 1864 	
\********************************************************/
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD = (u32)nms*fac_ms;    // Time loading (SysTick->LOAD?24bit)
	SysTick->VAL = 0x00;    // Clear Counter
	SysTick->CTRL = 0x01;   // Start countdown 
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	// Waiting time to arrive  
	SysTick->CTRL = 0x00;   // Off Counter
	SysTick->VAL  = 0X00;   // Clear Counter	   	    
} 
	

