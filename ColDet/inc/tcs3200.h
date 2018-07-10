#define COLOR_OE_H()        GPIO_SetBits(GPIOD, GPIO_Pin_9)
#define COLOR_OE_L()        GPIO_ResetBits(GPIOD, GPIO_Pin_9)    
#define COLOR_S2_H()        GPIO_SetBits(GPIOD, GPIO_Pin_10)
#define COLOR_S2_L()        GPIO_ResetBits(GPIOD, GPIO_Pin_10)   
#define COLOR_S3_H()        GPIO_SetBits(GPIOD, GPIO_Pin_11)
#define COLOR_S3_L()        GPIO_ResetBits(GPIOD, GPIO_Pin_11)   

extern u32 colorFreq[9];

void tcs3200_init(void);
void measure_freq(void);
