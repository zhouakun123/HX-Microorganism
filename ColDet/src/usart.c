#include "stm32f2xx.h"
#include "usart.h"
#include <string.h>
u8 usart2Buffer[USART2_MAXBUF];
u16 usart2BufEnd = sizeof(usart2Buffer) - 1;
u16 usart2BufIn = 0;
u16 usart2BufOut = 0;
u16 usart2BufCnt = 0;
u32 receiveIntCnt;     // Receive interval time count
void usart_receive_step(void);
/********************************************************\
* @brief  USART config
* @param  None
* @retval None
* @note   None	
\********************************************************/
void usart_config(void)
{		
    GPIO_InitTypeDef GPIO_InitStructure;	
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;	    
    
	// Connect USART2 pins to AF7 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);    

    // Set USART2 Tx Pin(PA.2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
    GPIO_Init(GPIOA , &GPIO_InitStructure);

    // Set USART2 Rx Pin(PA.3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA , &GPIO_InitStructure); 
    
	USART_DeInit(USART2);    
    
    // Enable USART2 Clock
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);     

    // USART2 config
    USART_InitStructure.USART_BaudRate = 19200;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); 
    // Enable USART2 RX Interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	
    USART_Cmd(USART2, ENABLE);          

    // Enable the USART2 Interrupt 
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	    
    
    usart2_send_byte(0x00);
    usart2_buffer_reset();
}
/********************************************************\
* @brief  USART2 buffer reset
* @param  None
* @retval None
* @note   None	
\********************************************************/
void usart2_buffer_reset(void)
{
	memset(usart2Buffer, 0, sizeof(usart2Buffer));
	usart2BufEnd = sizeof(usart2Buffer) - 1;
	usart2BufIn = 0;
	usart2BufOut = 0;
	usart2BufCnt = 0;   
}
/********************************************************\
* @brief  USART2 send a byte 
* @param  _Byte - Data to be sent
* @retval None
* @note   None	
\********************************************************/
void usart2_send_byte(u8 Byte)
{
    USART_SendData(USART2, Byte);
    // Loop until the end of transmission
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  		
}
/********************************************************\
* @brief  USART2 send a string 
* @param  _pBuf - String pointer
* @retval None
* @note   None	
\********************************************************/
void usart2_send_string(u8* pBuf)
{
    while ((*pBuf) != '\0')
    {
        USART_SendData(USART2, *pBuf);
        // Loop until the end of transmission
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);     			
        pBuf++;
    }
}
/********************************************************\
* @brief  USART2 send data in buffer
* @param  pBuf - Buffer pointer
* @param  len - Data length
* @retval None
* @note   None	
\********************************************************/
void usart2_send_buffer(u8* pBuf, u16 len)
{
    u32 i = 0;
    while (i < len)
    {
        USART_SendData(USART2, pBuf[i++]);
        // Loop until the end of transmission
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);     			
    }
}
/********************************************************\
* @brief  Save a byte data received
* @param  dat - Data received
* @retval None
* @note   None
\********************************************************/
void usart2_buffer_write1(u8 dat)
{
	if (usart2BufIn == usart2BufEnd) 
        return;
    
	usart2Buffer[usart2BufIn] = dat;
    
	usart2BufIn++;
	if (usart2BufIn >= sizeof(usart2Buffer)) 
    usart2BufIn = 0;    
    
    usart2BufCnt++;
    receiveIntCnt = 0;
}
/********************************************************\
* @brief  Received one byte Save to buffer
* @param  None
* @retval None
* @note   None	
\********************************************************/
void usart2_buffer_write2(void)
{
	if (usart2BufIn == usart2BufEnd) 
        return;
    
	usart2Buffer[usart2BufIn] = USART_ReceiveData(USART2);
    
	usart2BufIn++;
	if (usart2BufIn >= sizeof(usart2Buffer)) 
    usart2BufIn = 0;    
    
    usart2BufCnt++;
    receiveIntCnt = 0;
}
/********************************************************\
* @brief  Read one bytes from buffer
* @param  pByte - Save data pointer
* @retval 1 - Success, 0 - Fail
* @note   None
\********************************************************/
u8 usart2_buffer_read (u8* pByte)
{
	u8 sta = 0;	
    
	if (usart2BufOut == usart2BufIn)
	{
		sta = 0;
	}
	else
	{
        sta = 1;	
        *pByte = usart2Buffer[usart2BufOut];

        usart2BufOut++;
        if (usart2BufOut >= sizeof(usart2Buffer))
        usart2BufOut = 0;	

        if (usart2BufOut == 0) 
        usart2BufEnd = sizeof(usart2Buffer) - 1;	
        else usart2BufEnd = usart2BufOut - 1;

        if (usart2BufCnt > 0)
        usart2BufCnt--;		
	}	
	
	return sta;    
}
/********************************************************\
* @brief  This function handles USART2 global interrupt request
* @param  None
* @retval None
* @note	  None
\********************************************************/
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
        usart_receive_step();
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

    // Read once, Clear abnormal interrupt flag        
	USART_ReceiveData(USART2);  
}

