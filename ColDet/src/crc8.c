#include "stm32f2xx.h"
#include "crc8.h"
/********************************************************\
* @brief  Calculate CRC8(bit calculation)
* @param  dat - Data needed to be crc
* @param  crc - Previous crc results 
* @param  genpoly - CRC constant
* @retval crc results
* @note   Data high bit first calculation
\********************************************************/
u8 cal_crc8(u8 dat, u8 crc, u8 genpoly)
{
	u8 i;
	
	crc = crc ^ dat;
	for (i = 0; i < 8; i++)
	{
		if ((crc&0x80) != 0x00)                     
		crc = (crc << 1) ^ genpoly;                 
		else crc = crc << 1;                        
	}
	
	return crc;
}
