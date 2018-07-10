#include "stm32f2xx.h"
#include "TLC59711.h"
const u16 TLC59711_GS1[12] = {
	0x0000,0x0000,0x0000, 
	0x8000,0x8000,0x8000,
	0x8000,0x8000,0x8000,
	0x8000,0x8000,0x8000
};

const u16 TLC59711_GS2[12] = {
	0x0000,0x0000,0x0000, 
	0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000
};
/********************************************************\
* @brief  TLC59711 gpio init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void tlc59711_gpio_init(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;  
   	// Set PC.5 output, PC.5 control SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;												  		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);   
    
   	// Set PE.7 output, PE.7 control SDT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;												  		
	GPIO_Init(GPIOE, &GPIO_InitStructure);       
}
/********************************************************\
* @brief  TLC59711 init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void tlc59711_init(void)
{
    tlc59711_gpio_init();
    write_tlc59711((u16 *)TLC59711_GS2);    
}
/********************************************************\
* @brief  Write TLC59711
* @param  None    
* @retval None
* @note   None
\********************************************************/
void write_tlc59711(u16 *pBuf)
{
	u32 i,j;
    u16 gs;
	u8 bc = 0x7F;
	u8 fc = 0x12;
	u8 cmd = 0x25;
	
	cmd <<= 2;
	fc <<= 3;
	bc <<= 1;
	for (i = 0; i < 6; i++)
	{
		SCK_L();
		if (cmd&0x80)
		SDT_H();
		else SDT_L();
		SCK_H();		
		cmd <<= 1;
	}	
	

	for (i = 0; i < 5; i++)
	{
		SCK_L();
		if(fc&0x80)
		SDT_H();
		else SDT_L();
		SCK_H();			
		fc <<= 1;
	}
	
	for (i = 0; i < 3; i++)
	{
		bc = 0x7F;
		for (j = 0; j < 7; j++)
		{
			SCK_L();
			if(bc&0x80)
			SDT_H();
			else SDT_L();
			SCK_H();	
			SCK_L();
			bc <<= 1;
		}
	}
	
	for (i = 0; i < 12; i++)
	{
		gs = pBuf[i];
		for (j = 0; j < 16; j++)
		{
			SCK_L();
			if (gs&0x8000)
			SDT_H();
			else SDT_L();	
			SCK_H();				
			gs <<= 1;
		}
	}
}