/********************************************************\
* @file   :TB6560AHQ.c
* @author :James
* @time   :2017/10/13/10/55
* @note   :The stepper motor driver chip is TB6560AHQ.
* @note   :The stepper motor model is 57H5605.
\********************************************************/
#include "stm32f2xx.h"
#include "TB6560AHQ.h"
uint16_t CCR2_Val = 1000;
uint16_t CCR3_Val = 1000;
uint16_t PrescalerValue = 0;
uint32_t stepCh1Sta;
uint32_t stepCh2Sta;
uint32_t stepCycle;
void set_enable_sta(u8 ch, u8 sta);
void set_step_dir(u8 ch, u8 dir);
void set_step_current(u8 ch, u8 value);
void set_decay_mode(u8 ch, u8 mode);
void set_step_mode(u8 ch, u8 mode);
void tim3_pwm_ctrl(u32 cycle, u32 pulseNum);
/********************************************************\
* @brief  TB6560AHQ gpio init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void TB6560_gpio_init(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;  
   	// Set PB.7 output, PB.7 Control DIR_1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;												  		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);   
    
   	// Set PB.8 output, PB.8 Control TQ1_1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;												  		
	GPIO_Init(GPIOB, &GPIO_InitStructure);   
    
   	// Set PB.9 output, PB.9 Control TQ2_1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;												  		
	GPIO_Init(GPIOB, &GPIO_InitStructure);     

   	// Set PD.2 output, PD.2 Control DIR_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;												  		
	GPIO_Init(GPIOD, &GPIO_InitStructure);        

   	// Set PD.3 output, PD.3 Control TQ1_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;												  		
	GPIO_Init(GPIOD, &GPIO_InitStructure);    
    
   	// Set PD.4 output, PD.4 Control TQ2_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;												  		
	GPIO_Init(GPIOD, &GPIO_InitStructure);     
}
/********************************************************\
* @brief  TB6560 PWM init
* @param  None
* @retval None
* @note   None
\********************************************************/
void TB6560_pwm_init(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    // TIM3 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // TIM5 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);       

    // GPIOC clock enable 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  

    // GPIOC Configuration: TIM3 CH2 (PC7), TIM3 CH3 (PC8)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 

    // Connect TIM3 pins to AF2   
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3); 
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);   

    /* -----------------------------------------------------------------------
    TIM3 Configuration: generate 2 PWM signals with 2 the Same duty cycles.

    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM3 counter clock at 20 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /20 MHz) - 1
                                              
    To get TIM3 output clock at 10 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 1999
                  
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 50%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 50%

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
    ----------------------------------------------------------------------- */   

    // Compute the prescaler value 
    PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 20000000) - 1;  

    // Time base configuration 
    TIM_TimeBaseStructure.TIM_Period = 1999;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;   // 0.05us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // PWM1 Mode configuration: Channel2 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    
    // Power on Channel2 PWM output disable    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);      

    // Enable TIM3 peripheral Preload register on CCR2
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);    
    
    // PWM1 Mode configuration: Channel3 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    
    // Power on Channel3 PWM output disable    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);   
    
    // Enable TIM3 peripheral Preload register on CCR3
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);     

    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // TIM3 enable counter 
    TIM_Cmd(TIM3, ENABLE);      
    
    
    // TIM5 control pulse number
    /* -----------------------------------------------------------------------
    TIM5 Configuration: Output Compare Timing Mode:

    In this example TIM5 input clock (TIM5CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM5CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM5CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM5 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM5CLK / TIM5 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /1 MHz) - 1
    ----------------------------------------------------------------------- */ 
    TIM_TimeBaseStructure.TIM_Period = 0;    				
    TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock / 2) / 1000000) - 1;    // 1us  	  					
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  	
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);					
    TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);  
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);    
    TIM_Cmd(TIM5, DISABLE);	    

    // Enable TIM5 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);         
}
/********************************************************\
* @brief  TIM3 PWM output control
* @param  cycle - PWM cycle, Unit us
* @retval pulseNum - Pulse number
* @note   None
\********************************************************/
void tim3_pwm_ctrl(u32 cycle, u32 pulseNum)
{ 
    uint32_t temp;    
    // Set PWM frequency
    // cycle/0.05 = cycle*20 
    temp = cycle*20; // 0.05us 
    // TIM3 counter clock at 20 MHz
    TIM_SetAutoreload(TIM3, temp);    
    // TIM5 counter clock at 1 MHz
    TIM_SetAutoreload(TIM5, cycle*pulseNum);
}
/********************************************************\
* @brief  This function handles TIM5 global interrupt request
* @param  None
* @retval None
* @note	  None
\********************************************************/
void TIM5_IRQHandler(void)						
{    
    if (stepCh1Sta == 1)
    set_step_stop(STEP_CH1);
    if (stepCh2Sta == 1)
    set_step_stop(STEP_CH2);   
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);    
    if (stepCh1Sta == 0 && stepCh2Sta == 0)    
    TIM_Cmd(TIM5, DISABLE);
}
/********************************************************\
* @brief  TB6560 init
* @param  None
* @retval None
* @note   None	
\********************************************************/
void TB6560_init(void)
{ 
    TB6560_gpio_init(); 
    // 50% Current
    set_step_current(STEP_CH1, 3);
    // Forward stepping 
    set_step_dir(STEP_CH1, FORWARD); 
    // 100% Current    
    set_step_current(STEP_CH2, 1);
    // Forward stepping 
    set_step_dir(STEP_CH2, FORWARD);
    TB6560_pwm_init();      
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2);      
    // Pulse period, Or step frequency, Or step speed
    stepCycle = 100; // Unit us                 
}
/********************************************************\
* @brief  Delay a little time
* @param  None
* @retval None
* @note   None
\********************************************************/
void delay_LT(void)
{
    asm("nop"); asm("nop");
    asm("nop"); asm("nop");
    asm("nop"); asm("nop");      
}
/********************************************************\
* @brief  Set outputs enable status
* @param  ch - Step channel
* @param  dir - Enable status
* @retval None
* @note   None
\********************************************************/
void set_enable_sta(u8 ch, u8 sta)
{
    if (ch == STEP_CH1)
    {
        // Enable
  //       if (sta == 0)  
        // Disable
  //       else     
        delay_LT();
    }
    else if (ch == STEP_CH2)
    {
        // Enable 
  //      if (sta == 0) 
        // Disable
  //      else 
        delay_LT();
    }      
}
/********************************************************\
* @brief  Sets the direction of stepping
* @param  ch - Step channel
* @param  dir - Stepping direction
* @retval None
* @note   None
\********************************************************/
void set_step_dir(u8 ch, u8 dir)
{
    if (ch == STEP_CH1)
    {
        if (dir == FORWARD) 
        DIR_1_L(); 
        else DIR_1_H(); 
        delay_LT();
    }
    else if (ch == STEP_CH2)
    {
        if (dir == FORWARD) 
        DIR_2_L(); 
        else DIR_2_H(); 
        delay_LT();
    }       
}
/********************************************************\
* @brief  Set the step current
* @param  ch - Step channel
* @param  value - Current value
* @retval None
* @note   None
\********************************************************/
void set_step_current(u8 ch, u8 value)
{
    if (ch == STEP_CH1)
    {
        if (value == 1)     // 100%
        {
            TQ2_1_L();
            TQ1_1_L();
        }
        else if (value == 2)    // 75%    
        {
            TQ2_1_L();
            TQ1_1_H();          
        }
        else if (value == 3)    // 50%          
        {
            TQ2_1_H();
            TQ1_1_L();          
        }
        else if (value == 4)    // 20% 
        {
            TQ2_1_H();
            TQ1_1_H();          
        }        
    }
    else
    {
        if (value == 1)     // 100%
        {
            TQ2_2_L();
            TQ1_2_L();            
        }
        else if (value == 2)    // 75%    
        {
            TQ2_2_L();
            TQ1_2_H();           
        }
        else if (value == 3)    // 50%          
        {
            TQ2_2_H();
            TQ1_2_L();           
        }
        else if (value == 4)    // 20% 
        {
            TQ2_2_H();
            TQ1_2_H();           
        }         
    }
}
/********************************************************\
* @brief  Set decay mode
* @param  ch - Step channel
* @param  mode - Decay mode
* @retval None
* @note   None
\********************************************************/
void set_decay_mode(u8 ch, u8 mode)
{
    if (ch == STEP_CH1)
    {
        if (mode == 0) 
        {    
            delay_LT();
        }
        else if (mode == 1) 
        {
            delay_LT();
        }
        else if (mode == 2)
        {
            delay_LT();         
        }
        else if (mode == 3)
        {
            delay_LT();         
        }        
    }
    else if (ch == STEP_CH2)
    {
        if (mode == 0) 
        {   
            delay_LT();
        }
        else if (mode == 1) 
        {
            delay_LT();
        }
        else if (mode == 2)
        {
            delay_LT();           
        }
        else if (mode == 3)
        {
            delay_LT();           
        }        
    }       
}
/********************************************************\
* @brief  Set the step mode
* @param  ch - Step channel
* @param  mode - Microstep mode
* @retval None
* @note   None
\********************************************************/
void set_step_mode(u8 ch, u8 mode)
{
    if (ch == STEP_CH1)
    { 
        if (mode == MICROSTEPS_1)
        {        
            delay_LT();
        }
        else if (mode == MICROSTEPS_2)
        {   
            delay_LT();
        }  
        else if (mode == MICROSTEPS_8)
        {      
            delay_LT();
        } 
        else if (mode == MICROSTEPS_16)
        {
            delay_LT();
        }         
    }
    else if (ch == STEP_CH2)
    {
        if (mode == MICROSTEPS_1)
        { 
            delay_LT();
        }
        else if (mode == MICROSTEPS_2)
        {   
            delay_LT();
        }  
        else if (mode == MICROSTEPS_8)
        { 
            delay_LT();
        } 
        else if (mode == MICROSTEPS_16)
        {  
            delay_LT();
        }       
    }        
}
/********************************************************\
* @brief  Set stepping stop
* @param  ch - Step channel
* @retval None
* @note   None
\********************************************************/
void set_step_stop(u8 ch)
{
    if (ch == STEP_CH1)
    {   
        // Channel3 PWM output disable        
        TIM3->CCER &= ~(TIM_OutputState_Enable<<8); 
        stepCh1Sta = 0;
                 
        // Disable interrupt 
        TIM5->DIER &= (uint16_t)~TIM_IT_Update;              
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update;         
        // Disable counter 
        TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;        
        
        // 20% Current
        set_step_current(STEP_CH1, 4);       
    }
    else if (ch == STEP_CH2)
    {
        // Channel2 PWM output disable     
        TIM3->CCER &= ~(TIM_OutputState_Enable<<4);  
        stepCh2Sta = 0;        
               
        // Disable interrupt 
        TIM5->DIER &= (uint16_t)~TIM_IT_Update;              
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update;         
        // Disable counter 
        TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;   
        
        // 20% Current    
        set_step_current(STEP_CH2, 4);         
    }
}
/********************************************************\
* @brief  Set stepping forward
* @param  ch - Step channel
* @param  stepNum - Step number
* @retval None
* @note   57H5605 the fixed step angle: 1.8 degree;
* @note   TB6560 the step mode: 16 microstep/step;
* @note   57H5605 the actual step angle: 1.8/16 = 0.1125 degree; 
* @note   So 57H5605 number of step per turn(step/turn):
* @note   360/0.1125 = 3200 step, That requires 3200 pulses;
* @note   Step motor maximum rotational speed: 300R/MIN; 
* @note   So 57H5605 maximum number of steps per second:
* @note   3200*300/60 = 960000/60 = 16000 step;
* @note   So TB6560 maximum step frequency: 16000 = 16Khz;
* @note   If the step motor has a load, The step frequency 
* @note   should be set lower than the maximum step frequency.
\********************************************************/
void set_step_forward(uint8_t ch, uint32_t stepNum)
{
    u32 temp;
    if (ch == STEP_CH1)
    {
        // 50% Current
        set_step_current(STEP_CH1, 3);      
      
        if (stepNum > 0)
        tim3_pwm_ctrl(stepCycle, stepNum);      
        else    // 0, The motor keeps turning.
        {
            // Set 0xffffffff - 1 step
            temp = 0xffffffff/stepCycle - 1;
            tim3_pwm_ctrl(stepCycle, temp);
        }
        
        // Forward stepping
        set_step_dir(STEP_CH1, FORWARD);
        // Channel3 PWM output enable
        TIM3->CCER |= (TIM_OutputState_Enable<<8);        
        stepCh1Sta = 1;
        
        TIM5->CNT = 0;        
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update;   
        // Enable interrupt         
        TIM5->DIER |= TIM_IT_Update; 
        // Enable counter         
        TIM5->CR1 |= TIM_CR1_CEN;         
    }
    else if (ch == STEP_CH2)
    { 
        // 100% Current
        set_step_current(STEP_CH2, 1);        
      
        if (stepNum > 0)
        tim3_pwm_ctrl(stepCycle, stepNum);      
        else    // 0, The motor keeps turning.
        {
            // Set 0xffffffff - 1 step
            temp = 0xffffffff/stepCycle - 1;
            tim3_pwm_ctrl(stepCycle, temp);
        }
        
        // Forward stepping
        set_step_dir(STEP_CH2, FORWARD);        
        // Channel2 PWM output enable
        TIM3->CCER |= (TIM_OutputState_Enable<<4);  
        stepCh2Sta = 1; 
        
        TIM5->CNT = 0;        
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update; 
        // Enable interrupt         
        TIM5->DIER |= TIM_IT_Update;  
        // Enable counter         
        TIM5->CR1 |= TIM_CR1_CEN;         
    }
}
/********************************************************\
* @brief  Set stepping backward
* @param  ch - Step channel
* @param  stepNum - Step number
* @retval None
* @note   57H5605 the fixed step angle: 1.8 degree;
* @note   TB6560 the step mode: 16 microstep/step;
* @note   57H5605 the actual step angle: 1.8/16 = 0.1125 degree; 
* @note   So 57H5605 number of step per turn(step/turn):
* @note   360/0.1125 = 3200 step, That requires 3200 pulses;
* @note   Step motor maximum rotational speed: 300R/MIN; 
* @note   So 57H5605 maximum number of steps per second:
* @note   3200*300/60 = 960000/60 = 16000 step;
* @note   So TB6560 maximum step frequency: 16000 = 16Khz;
* @note   If the step motor has a load, The step frequency 
* @note   should be set lower than the maximum step frequency.
\********************************************************/
void set_step_backward(uint8_t ch, uint32_t stepNum)
{
    u32 temp;  
    if (ch == STEP_CH1)
    {          
        // 50% Current
        set_step_current(STEP_CH1, 3);         
      
        if (stepNum > 0)
        tim3_pwm_ctrl(stepCycle, stepNum);      
        else    // 0, The motor keeps turning.
        {
            // Set 0xffffffff - 1 step
            temp = 0xffffffff/stepCycle - 1;
            tim3_pwm_ctrl(stepCycle, temp);
        }
        
        // Backward stepping
        set_step_dir(STEP_CH1, BACKWARD);     
        // Channel3 PWM output enable
        TIM3->CCER |= (TIM_OutputState_Enable<<8);
        stepCh1Sta = 1;        
        
        TIM5->CNT = 0;        
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update;   
        // Enable interrupt         
        TIM5->DIER |= TIM_IT_Update; 
        // Enable counter         
        TIM5->CR1 |= TIM_CR1_CEN;         
    }
    else if (ch == STEP_CH2)
    {
        // 100% Current
        set_step_current(STEP_CH2, 1);   
      
        if (stepNum > 0)
        tim3_pwm_ctrl(stepCycle, stepNum);      
        else    // 0, The motor keeps turning.
        {
            // Set 0xffffffff - 1 step
            temp = 0xffffffff/stepCycle - 1;
            tim3_pwm_ctrl(stepCycle, temp);
        }
        
        // Backward stepping
        set_step_dir(STEP_CH2, BACKWARD);        
        // Channel2 PWM output enable
        TIM3->CCER |= (TIM_OutputState_Enable<<4);  
        stepCh2Sta = 1;         
        
        TIM5->CNT = 0;      
        // Clear the IT pending Bit          
        TIM5->SR = (uint16_t)~TIM_IT_Update;    
        // Enable interrupt         
        TIM5->DIER |= TIM_IT_Update;
        // Enable counter         
        TIM5->CR1 |= TIM_CR1_CEN;         
    }
}