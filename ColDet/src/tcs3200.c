#include "stm32f2xx.h"
#include "global.h"
#include "tcs3200.h"
#include "crc8.h"
u32 colorFreq[9];
u32 receiveCnt;
u32 recTimeOut;
u32 dataLength;
u32 getColorFreq;
u8 receiveBuf[100];
u32 measureFreqSta;
/********************************************************\
* @brief  TCS3200 gpio init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void tcs3200_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
   	// Set PD.9 output, PD.9 control Color_OE
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;												  		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
   	// Set PD.10 output, PD.10 control Color_S3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;												  		
	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
   	// Set PD.11 output, PD.11 control Color_S2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;												  		
	GPIO_Init(GPIOD, &GPIO_InitStructure);      
}
/********************************************************\
* @brief  TIM external clock count init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void tim_cnt_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_DeInit(TIM1);     
    TIM_DeInit(TIM4);    
    TIM_DeInit(TIM6);    
    TIM_DeInit(TIM8);
    TIM_DeInit(TIM9);   
    TIM_DeInit(TIM12);      
    // Enable TIM1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);      
    // Enable TIM4 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  
    // Enable TIM6 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);    
    // Enable TIM8 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);     
    // Enable TIM9 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);         
    // Enable TIM12 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);             
    
    // PE5 config: TIM9 CH1 (PE5 --> Color_O1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);        
    
    // PB14 config: TIM12 CH1 (PB14 --> Color_O4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);              

    // PD12 config: TIM4 CH1 (PD12 --> Color_O7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOD, &GPIO_InitStructure);         
    
    // PC6 config: TIM8 CH1 (PC6 --> Color_O8)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
    // PE9 config: TIM1 CH1 (PE9 --> Color_O9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);     
    
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);    
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);    
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);         
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);                       
    
    // TIM1 config 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // Auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    // External clock source   
    TIM_TIxExternalClockConfig(TIM1, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0); 
    // TIM1 enable counter 
    TIM_Cmd(TIM1, ENABLE);
    
    // TIM4 config 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // Auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    // External clock source   
    TIM_TIxExternalClockConfig(TIM4, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0);    
    // TIM4 enable counter 
    TIM_Cmd(TIM4, ENABLE);
    
    // TIM8 config 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // Auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    // External clock source   
    TIM_TIxExternalClockConfig(TIM8, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0);  
    // TIM8 enable counter 
    TIM_Cmd(TIM8, ENABLE);    
    
    // TIM9 config 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // Auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    // External clock source   
    TIM_TIxExternalClockConfig(TIM9, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0); 
    // TIM9 enable counter 
    TIM_Cmd(TIM9, ENABLE);          
    
    // TIM12 config 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // Auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
    // External clock source   
    TIM_TIxExternalClockConfig(TIM12, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0);   
    // TIM12 enable counter 
    TIM_Cmd(TIM12, ENABLE);               	   
    
    // TIM6 config
    /* -----------------------------------------------------------------------
    TIM6 Configuration: Output Compare Timing Mode:

    In this example TIM6 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM6CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM6CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM6 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM6CLK / TIM6 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /1 MHz) - 1
    ----------------------------------------------------------------------- */ 
    TIM_TimeBaseStructure.TIM_Period = 50000;    // 1(1us) * 50000 + 1 ~= 50ms 							
    TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock / 2) / 1000000) - 1;   					
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  	
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);					
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);  				
    TIM_Cmd(TIM6, ENABLE);	      

    // Enable TIM6 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}
/********************************************************\
* @brief  TIM external clock count reset
* @param  None    
* @retval None
* @note   None
\********************************************************/
void tim_cnt_reset(void)
{
    TIM_SetCounter(TIM2, 0);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);    
}
/********************************************************\
* @brief  This function handles TIM6 global interrupt request
* @param  None
* @retval None
* @note	  50ms Interrupt
\********************************************************/ 
void TIM6_DAC_IRQHandler(void)						
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        // Color_O1         
        colorFreq[0] = TIM9->CNT;
        TIM9->CNT = 0;        
                
        // Color_O4            
        colorFreq[3] = TIM12->CNT;
        TIM12->CNT = 0;        
                
        // Color_O7        
        colorFreq[6] = TIM4->CNT; 
        TIM4->CNT = 0;        
        
        // Color_O8        
        colorFreq[7] = TIM8->CNT;
        TIM8->CNT = 0;        
        
        // Color_O9
        colorFreq[8] = TIM1->CNT;  
        TIM1->CNT = 0;        
        
        measureFreqSta = 1;           
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }		
}
/********************************************************\
* @brief  TCS3200 init
* @param  None
* @retval None
* @note   None	
\********************************************************/
void tcs3200_init(void)
{ 
    tcs3200_gpio_init();
    COLOR_OE_L();   // Enable frequency output
    COLOR_S2_L();   
    COLOR_S3_L();   // Select red
    tim_cnt_init();
}
/********************************************************\
* @brief  Measure color frequency 
* @param  None
* @retval None
* @note   None	
\********************************************************/
void measure_freq(void)
{ 
	u8 i,crc;    
    if (getColorFreq == 1)
    {
        getColorFreq = 0;        
        crc = 0;
        for (i = 0; i < 16; i++)
        crc = cal_crc8(receiveBuf[i], crc, GENP0LY8);
        if (receiveBuf[16] == crc) 
        {
            colorFreq[1] = receiveBuf[4] + (u32)(receiveBuf[5]<<8) + (u32)(receiveBuf[6]<<16);
            colorFreq[2] = receiveBuf[7] + (u16)(receiveBuf[8]<<8) + (u32)(receiveBuf[9]<<16);
            colorFreq[4] = receiveBuf[10] + (u16)(receiveBuf[11]<<8) + (u32)(receiveBuf[12]<<16);
            colorFreq[5] = receiveBuf[13] + (u16)(receiveBuf[14]<<8) + (u32)(receiveBuf[15]<<16); 
        }        
    }
}
/********************************************************\
* @brief  USART receive step
* @param  None
* @retval None
* @note   None
\********************************************************/
void usart_receive_step(void)
{	
	u8 temp;		      
    
    if (getColorFreq == 1)
        return;
    
    recTimeOut = 0;
    temp = USART_ReceiveData(USART2);     
	receiveCnt++;    
	switch (receiveCnt) {
	case 1:		
        receiveBuf[0] = temp;
		if (temp != SOH) 
		receiveCnt = 0;	
		break;
        
	case 2:     // Cmd
        receiveBuf[1] = temp;  
        break;
        
	case 3:     // Status       
        receiveBuf[2] = temp;        
		break;	
        
	case 4:     // Length
        receiveBuf[3] = temp;          
        dataLength = temp;
        if (dataLength != 16)
		receiveCnt = 0;	            
		break;	
        	
	case 17:    // Check       		
        if (dataLength < sizeof(receiveBuf))
        receiveBuf[dataLength] = temp; 
		break;
        
	case 18:    // EOT       		
        if (dataLength + 1 < sizeof(receiveBuf))
        receiveBuf[dataLength + 1] = temp; 
        receiveCnt = 0;        
		if (temp == EOT)             	
        getColorFreq = 1;
		break;        
        
	default:	
        if (receiveCnt < sizeof(receiveBuf))      
        receiveBuf[receiveCnt - 1] = temp;
        if (receiveCnt > dataLength)
		receiveCnt = 0;			 
		break;
	}		
}