#define RS485_EN_H()        GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define RS485_EN_L()        GPIO_ResetBits(GPIOB, GPIO_Pin_12)  
         
void rs485_init(void);
void rs485_buffer_reset(void);
void rs485_send_byte(u8 dat);
void rs485_send_string(u8 *pBuf);
void rs485_send_buffer(u8* pBuf, u16 len);
void rs485_buffer_write1(u8 dat);
void rs485_buffer_write2(void);