#include "stm32f2xx.h"
#include "rs485.h"
#include <string.h>
#include <stdio.h>
u8 buffer485[300];
u16 buf485End = sizeof(buffer485) - 1;
u16 buf485In = 0;
u16 buf485Out = 0;
u16 buf485Cnt = 0;	
/********************************************************\
* @brief  RS485 init
* @param  None
* @retval None
* @note   None
\********************************************************/
void rs485_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;	
 
   	// Set PB.12 output, PB.12 Control 485_EN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;												  		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);     
    
	// Connect USART3 pins to AF7 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);     

    // Set USART3 Tx Pin(PB.10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Set USART3 Rx Pin(PB.11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
    
	USART_DeInit(USART3);      
    
    // Enable USART3 Clock
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE);    

    // USART3 config
    USART_InitStructure.USART_BaudRate = 19200;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure); 
	
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
    // Enable USART3 RX Interrupt
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
    USART_Cmd(USART3, ENABLE);          

    // Enable the USART3 Interrupt 
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	    
    
    rs485_send_byte(0x00);
	rs485_buffer_reset();						
    RS485_EN_L();   // RS485 receive enable, Send disable	    
}
/********************************************************\
* @brief  RS485 buffer reset
* @param  None
* @retval None
* @note   None
\********************************************************/
void rs485_buffer_reset(void)
{		
    memset(buffer485, 0, sizeof(buffer485));
	buf485End = sizeof(buffer485) - 1;
	buf485In = 0; buf485Out = 0;	 
	buf485Cnt = 0;	   
}
/********************************************************\
* @brief  Send a byte, Polling mode
* @param  dat - Send data
* @retval None
* @note   None
\********************************************************/
void rs485_send_byte(u8 dat)
{
	USART_SendData(USART3, dat);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  	
}
/********************************************************\
* @brief  Send a string, Polling mode
* @param  pBuf - String pointer
* @retval None
* @note   None
\********************************************************/
void rs485_send_string(u8 *pBuf)
{	 								 
	RS485_EN_H();   // RS485 send enable, Receive disable       

    while ((*pBuf) != '\0')
    {
        USART_SendData(USART3, *pBuf);
        // Loop until the end of transmission
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);     			
        pBuf++;
    }    
	
    RS485_EN_L();   // RS485 receive enable, Send disable		
}
/********************************************************\
* @brief  Send data in buffer
* @param  pBuf - Buffer pointer
* @param  len - Data length
* @retval None
* @note   None	
\********************************************************/
void rs485_send_buffer(u8* pBuf, u16 len)
{
    u32 i = 0;    
    RS485_EN_H();   // RS485 send enable, Receive disable     
    
    while (i < len)
    {
        USART_SendData(USART3, pBuf[i++]);
        // Loop until the end of transmission
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);     			
    }
    
    RS485_EN_L();   // RS485 receive enable, Send disable	    
}
/********************************************************\
* @brief  Save a byte data received
* @param  dat - Data received
* @retval None
* @note   None
\********************************************************/
void rs485_buffer_write1(u8 dat)
{
	if (buf485In == buf485End) 
		return;	
		
	buffer485[buf485In] = dat;
	
	buf485In++;
	if (buf485In >= sizeof(buffer485)) 
    buf485In = 0;		
    
	buf485Cnt++;    
}
/********************************************************\
* @brief  Save a byte data received
* @param  None
* @retval None
* @note   None
\********************************************************/
void rs485_buffer_write2(void)
{
	if (buf485In == buf485End) 
		return;	
		
	buffer485[buf485In] = USART_ReceiveData(USART3);

	buf485In++;
	if (buf485In >= sizeof(buffer485))
    buf485In = 0;	

	buf485Cnt++;		
}	
/********************************************************\
* @brief  Read 1 bytes from the receive buffer
* @param  pBuf - Buffer pointer
* @retval 1 - Read success
* @retval 0 - No data read
* @note   None
\********************************************************/
u8 rs485_buffer_read(u8* pBuf)
{
	u8 sta = 0;	
	
	if (buf485Out == buf485In)
	{
		sta = 0;
	}
	else
	{		
		sta = 1;	
		*pBuf = buffer485[buf485Out];
        
		buf485Out++;		
		if (buf485Out >= sizeof(buffer485)) 
        buf485Out = 0;	
        
		if (buf485Out == 0) 
        buf485End = sizeof(buffer485) - 1;	
		else buf485End = buf485Out - 1;
        
		if (buf485Cnt > 0) buf485Cnt--;					
	}	
	
	return sta;
}
/********************************************************\
* @brief  This function handles USART3 global interrupt request
* @param  None
* @retval None
* @note	  None
\********************************************************/
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}

    // Read once, Clear abnormal interrupt flag
	USART_ReceiveData(USART3);	
}
