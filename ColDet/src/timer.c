#include "stm32f2xx.h"
#include "usart.h"
#include "timer.h"
extern u32 recTimeOut;
extern u32 receiveCnt;
void func_time_int(void);
/********************************************************\
* @brief  Timer config
* @param  None
* @retval None
* @note   1ms timing
\********************************************************/
void timer_config(void)
{   
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_DeInit(TIM7);
    // Enable TIM7 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    // TIM7 config
    /* -----------------------------------------------------------------------
    TIM7 Configuration: Output Compare Timing Mode:

    In this example TIM7 input clock (TIM7CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM7CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM7CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM7 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM7CLK / TIM7 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /1 MHz) - 1
    ----------------------------------------------------------------------- */ 
    TIM_TimeBaseStructure.TIM_Period = 1000;    // 1(1us) * 1000 + 1 ~= 1ms 					
    TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock / 2) / 1000000) - 1;   					
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  	
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);					
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);  				
    TIM_Cmd(TIM7, ENABLE);	    

    // Enable TIM7 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);         
}
/********************************************************\
* @brief  This function handles TIM7 global interrupt request
* @param  None
* @retval None
* @note	  1ms Interrupt
\********************************************************/
void TIM7_IRQHandler(void)						
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {	    
        recTimeOut++;
        if (recTimeOut >= 30)   // 30ms
        {
            recTimeOut = 0;
            if (receiveCnt > 0)
            receiveCnt = 0;  
        } 
                
        func_time_int();
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);     
    }		
}
       
