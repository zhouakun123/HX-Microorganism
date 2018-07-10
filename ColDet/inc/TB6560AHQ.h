#define DIR_1_H()           GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define DIR_1_L()           GPIO_ResetBits(GPIOB, GPIO_Pin_7)   
#define TQ2_1_H()           GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define TQ2_1_L()           GPIO_ResetBits(GPIOB, GPIO_Pin_9)  
#define TQ1_1_H()           GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define TQ1_1_L()           GPIO_ResetBits(GPIOB, GPIO_Pin_8)  

#define DIR_2_H()           GPIO_SetBits(GPIOD, GPIO_Pin_2)
#define DIR_2_L()           GPIO_ResetBits(GPIOD, GPIO_Pin_2)  
#define TQ2_2_H()           GPIO_SetBits(GPIOD, GPIO_Pin_4)
#define TQ2_2_L()           GPIO_ResetBits(GPIOD, GPIO_Pin_4)  
#define TQ1_2_H()           GPIO_SetBits(GPIOD, GPIO_Pin_3)
#define TQ1_2_L()           GPIO_ResetBits(GPIOD, GPIO_Pin_3)  

#define STEP_CH1            0
#define STEP_CH2            1

#define FORWARD             0
#define BACKWARD            1

#define MICROSTEPS_1        0   // Full step         
#define MICROSTEPS_2        1   // 1/2 step    
#define MICROSTEPS_8        2   // 1/8 step
#define MICROSTEPS_16       3   // 1/16 step     

extern uint32_t stepCh1Sta;
extern uint32_t stepCh2Sta;

void TB6560_init(void);
void set_step_stop(u8 ch);
void set_step_forward(u8 ch, uint32_t stepNum);
void set_step_backward(u8 ch, uint32_t stepNum);


