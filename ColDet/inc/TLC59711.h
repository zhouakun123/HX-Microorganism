#define SCK_H()        GPIO_SetBits(GPIOC, GPIO_Pin_5)
#define SCK_L()        GPIO_ResetBits(GPIOC, GPIO_Pin_5) 
#define SDT_H()        GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define SDT_L()        GPIO_ResetBits(GPIOE, GPIO_Pin_7) 

extern const u16 TLC59711_GS1[12];
extern const u16 TLC59711_GS2[12];

void tlc59711_init(void);
void write_tlc59711(u16 *pBuf);