#define LED1_H()        GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define LED1_L()        GPIO_ResetBits(GPIOC, GPIO_Pin_10) 
#define LED2_H()        GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define LED2_L()        GPIO_ResetBits(GPIOA, GPIO_Pin_15) 
#define LED3_H()        GPIO_SetBits(GPIOA, GPIO_Pin_12)
#define LED3_L()        GPIO_ResetBits(GPIOA, GPIO_Pin_12) 

#define CHECK1          PEin(8)   // (GPIOE->IDR & GPIO_Pin_8)
#define CHECK2          PEin(10)
#define CHECK3          PEin(11)
#define CHECK4          PEin(12)
#define CHECK5          PEin(13)
#define CHECK6          PEin(14)
#define CHECK7          PEin(15)
#define G_SWITCH        PDin(14)
#define KEY_UP          PDin(13)
#define KEY_DOWN        PDin(8)
#define KEY_STOP        PBin(15)
#define DOOR            PBin(13)
#define KEY1            PAin(8)
#define KEY2            PCin(9)
#define KEY3            PBin(1)
#define KEY4            PBin(0)
#define HEAT1_CTRL      PEout(2)
#define HEAT2_CTRL      PEout(3)
#define HEAT3_CTRL      PEout(4)
#define HEAT4_CTRL      PEout(6)
#define HEAT5_CTRL      PCout(13)
#define FAN_CTRL        PCout(1)
#define VALVE_CTRL      PCout(2)
#define COMP_CTRL       PCout(3)

#define HEAT_ON         0x55
#define HEAT_OFF        0xaa

#define FAN_ON          0x55
#define FAN_OFF         0xaa

#define VALVE_ON        0x55
#define VALVE_OFF       0xaa

#define COMP_ON         0x55
#define COMP_OFF        0xaa

typedef __packed union
{
    u32 sta; 
    __packed struct {     
    u32 check1Sta : 1;
    u32 check2Sta : 1;
    u32 check3Sta : 1;
    u32 check4Sta : 1;
    u32 check5Sta : 1;
    u32 check6Sta : 1;
    u32 check7Sta : 1;   
    u32 gSwitchSta: 1;
    u32 keyUpSta : 1;
    u32 keyDownSta: 1;   
    u32 keyStopSta: 1; 
    u32 doorSta: 1;       
    u32 boardCheck: 1;
    }bits;    
}INPUT_STA;

typedef __packed union
{
    u32 flag; 
    __packed struct {         
    u32 boardCheck: 1;  
    }bits;    
}F_FLAG;

typedef struct PID {
	s16 SetPoint;     // setting target(Desired value)
	u16 Proportion;   // Proportional Const
	u16 Integral;     // Integral Const
	u16 Derivative;   // Derivative Const
	s16 LastError;    // Error[-1]	
	s16 PrevError;    // Error[-2]
	s16 SumError;     // Sums of Errors
} PID;

void func_init(void);
void func_task(void);
void func_time_task(void);
void color_freq_printf(void);