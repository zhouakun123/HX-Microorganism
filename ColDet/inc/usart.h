#define USART2_MAXBUF   200

extern u32 receiveIntCnt;

void usart_config(void);	 
void usart2_buffer_reset(void);
void usart2_send_byte(u8 _Byte);
void usart2_send_string(u8* pBuf);
void usart2_send_buffer(u8* pBuf, u16 len);
void usart2_buffer_write1(u8 dat);
void usart2_buffer_write2(void);
u8 usart2_buffer_read(u8* pByte);  
