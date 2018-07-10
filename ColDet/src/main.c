/********************************************************\
* @file   :main.c
* @author :James
* @time   :2017/10/13/10/55
* @note   :Use HSE 120Mhz clock, MCU STM32F207VC
\********************************************************/
#include "stm32f2xx.h"
#include "TB6560AHQ.h"
#include "tcs3200.h"
#include "tlc59711.h"
#include "rs485.h"
#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "iwdg.h"
#include "func.h"
#include "adc.h"
#include <string.h>
#include <stdio.h>
void int_config(void);
void rcc_config(void);
void gpio_config(void);
/********************************************************\
* @brief  Time task
* @param  None
* @retval None
* @note   None
\********************************************************/
void time_task(void)
{
    func_time_task();
}
/********************************************************\
* @brief  Main
* @param  None
* @retval None
* @note   None
\********************************************************/
int main(void)
{	 
    rcc_config();						
    int_config();						
    gpio_config();
    delay_init(120);
    rs485_init();
    //IWDG_Config();
    adc1_config();
    usart_config();	
    timer_config();
    TB6560_init();
    tcs3200_init();
    tlc59711_init();
    func_init();
    //IWDG_Config();    
    while(1)
    {	         
        time_task();
        func_task();
        measure_freq();
        IWDG_ReloadCounter(); 
    }
}
/********************************************************\
* @brief  External clock config
* @param  None
* @retval None
* @note	  Use HSE
\********************************************************/
void rcc_config(void)			    	
{ 
	
}
/********************************************************\
* @brief  Internal clock config
* @param  None
* @retval None
* @note	Use HSI
\********************************************************/
#if 0
void rcc_config(void)						
{

}
#endif
/********************************************************\
* @brief  Interrupt config
* @param  None
* @retval None
* @note	None
\********************************************************/
void int_config(void)
{
    // Set the Vector Table base address at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH|0x00000, 0x00);
    // Set preemption priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	
}
/********************************************************\
* @brief  GPIO config
* @param  None
* @retval None
* @note	None
\********************************************************/ 
void gpio_config(void)
{		
    // GPIOA,GPIOB,GPIOC,GPIOD,GPIOE Periph clock enable 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);   
}
