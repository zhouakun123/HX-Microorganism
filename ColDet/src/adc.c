#include "stm32f2xx.h"
#include "global.h"
#include "adc.h"
u16 adValue1;
u16 adValue2;
u16 adValue3;
u16 adValue4;
u16 adValue5;
u16 adValue6;
u16 ADC1Buffer[N1][7];
A_FLAG a_flag;
void adc1_value_handle(void);
/********************************************************\
* @brief  ADC1 config
* @param  None
* @retval None
* @note   None	
\********************************************************/
void adc1_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;      
    NVIC_InitTypeDef NVIC_InitStructure;      

    // Enable DMA2 clock 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    // Enable ADC1 clock     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);       

    // Config PA.0(ADC1 Channel 0) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;    
    GPIO_Init(GPIOA, &GPIO_InitStructure);   

    // Config PA.1(ADC1 Channel 1) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);        

    // Config PA.4(ADC1 Channel 4) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   

    // Config PA.5(ADC1 Channel 5) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);      

    // Config PA.6(ADC1 Channel 6) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    // Config PA.7(ADC1 Channel 7) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
    
    // Config PC.4(ADC1 Channel 14) as analog input 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);      
    
    // DMA2 Stream0 channel0 configuration 
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = sizeof(ADC1Buffer)/2;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);      
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);      
    
    // ADC Common Init 
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);    

    // ADC1 Init 
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 7;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // ADC1 regular channel0 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_56Cycles);  
    // ADC1 regular channel1 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_56Cycles);
    // ADC1 regular channel4 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_56Cycles);
    // ADC1 regular channel5 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 4, ADC_SampleTime_56Cycles);
    // ADC1 regular channel6 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_56Cycles);
    // ADC1 regular channel7 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 6, ADC_SampleTime_56Cycles);
    // ADC1 regular channel14 configuration 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 7, ADC_SampleTime_56Cycles);    

    // Enable DMA request after last transfer (Single-ADC mode) 
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);    
    
    // Enable ADC1 DMA 
    ADC_DMACmd(ADC1, ENABLE);

    // Enable ADC1 
    ADC_Cmd(ADC1, ENABLE);      
    
    // Enable DMA2_Stream0 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/********************************************************\
* @brief  ADC1 DMA2 start
* @param  None
* @retval None
* @note   None
\********************************************************/
void adc1_dma2_start(void)
{
	DMA_Cmd(DMA2_Stream0, DISABLE);
	DMA2_Stream0->M0AR = (uint32_t)ADC1Buffer;
	DMA2_Stream0->NDTR = sizeof(ADC1Buffer)/2;
	DMA_Cmd(DMA2_Stream0, ENABLE);
}
/********************************************************\
* @brief  ADC1 DMA conversion start
* @param  None
* @retval None
* @note   None
\********************************************************/
void adc1_dma_conv(void)
{  
    adc1_dma2_start();
    ADC_Cmd(ADC1, ENABLE);        
    ADC_SoftwareStartConv(ADC1);     
}
/********************************************************\
* @brief  This function handles DMA2_Stream0 interrupt request
* @param  None
* @retval None
* @note   None
\********************************************************/
void DMA2_Stream0_IRQHandler(void)
{	
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);  
    adc1_value_handle();
}
/********************************************************\
* @brief  ADC1 value handle
* @param  None
* @retval None
* @note   None
\********************************************************/
void adc1_value_handle(void)
{
    u32 i,j,sum,aver;	 
    u16 temp,tempBuf[N1];            
    // Disables ADC1 
    ADC_Cmd(ADC1, DISABLE);    
    
    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][0];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue1 = aver;
    a_flag.bits.adValue1Ok = 1; 
    
    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][1];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue2 = aver;     
    a_flag.bits.adValue2Ok = 1;    
    
    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][2];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue3 = aver;    
    a_flag.bits.adValue3Ok = 1;    
    
    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][3];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue4 = aver;    
    a_flag.bits.adValue4Ok = 1;    
    
    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][4];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue5 = aver; 
    a_flag.bits.adValue5Ok = 1;    

    //---------------------------------------------
    // The median value average filtering algorithm
    //---------------------------------------------
    
    // The value in the array to a temporary array
    for (i = 0; i < N1; i++)
    {
        tempBuf[i] = ADC1Buffer[i][5];
    }
    
    // Sampling values from small to large (bubble)
    for (j = 1; j < N1; j++)
    {
        for (i = 0; i < N1-j; i++)
        {
            if (tempBuf[i] > tempBuf[i+1])
            {
                temp = tempBuf[i];
                tempBuf[i] = tempBuf[i+1];
                tempBuf[i+1] = temp;
            }
        }
    }

    sum = 0;   
    // Remove maximum and minimum    
    for (i = 1; i < N1-1; i++)  
    sum += tempBuf[i];
    
    aver = sum/(N1-2);      
    // End    
    
    adValue6 = aver;     
    a_flag.bits.adValue6Ok = 1;
    
    a_flag.bits.adc1ConvOk = 1;	
}
