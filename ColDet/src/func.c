#include "stm32f2xx.h"
#include "global.h"
#include "tcs3200.h"
#include "TB6560AHQ.h"
#include "TLC59711.h"
#include "rs485.h"
#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "func.h"
#include "adc.h"
#include "temp.h"
#include "crc8.h"
#include <string.h>
#include <stdio.h>
u8 tempBuf1[1000];
u32 inputCheckCnt;
u32 check1Cnt;
u32 check2Cnt;
u32 check3Cnt;
u32 check4Cnt;
u32 check5Cnt;
u32 check6Cnt;
u32 check7Cnt;
u32 gSwitchCnt;
u32 keyUpCnt;
u32 keyDownCnt;
u32 keyStopCnt;
u32 doorCnt;
INPUT_STA inputSta;
u32 heat1CtrlSta;
u32 heat2CtrlSta;
u32 heat3CtrlSta;
u32 heat4CtrlSta;
u32 heat5CtrlSta;
u32 fanCtrlSta;
u32 valveCtrlSta;
u32 compCtrlSta;
u32 freqBuffer[72][4];  // 72 dot 4 colors frequency
u32 boardCheckStep;
u32 boardCheckCnt;
u32 boardCheckIntCnt;
u32 sampleIntCnt;
F_FLAG f_flag;
PID t1PID = {350,10,5,4,0,0,0};
s16 vOut;	// PID Response (Output)
s16 vIn;	// PID Feedback (Input)
uint16_t t1CCR2_Val;
u32 temp1CheckStep;
u32 temp1CheckCnt;
void input_check(void);
void heat1_control(u32 sta);
void heat2_control(u32 sta);
void heat3_control(u32 sta);
void heat4_control(u32 sta);
void heat5_control(u32 sta);
void fan_control(u32 sta);
void valve_control(u32 sta);
void comp_control(u32 sta);
void one_board_check(void);
void temp1_control(void);
void PIDInit(PID *pp);
void temp_pwm_init(void);
/********************************************************\
* @brief  Function gpio init
* @param  None
* @retval None
* @note   None	
\********************************************************/
void func_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
	// Set PA.8 input, PA.8 check Key1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 			
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);       
    
   	// Set PC.1 output, PC.1 control Fan_Ctrl
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;												  		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);   
    
   	// Set PC.2 output, PC.2 control Valve_Ctrl
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;												  		
	GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
   	// Set PC.3 output, PC.3 control Comp_Ctrl
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;												  		
	GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
   	// Set PC.10 output, PC.10 control LED1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;												  		
	GPIO_Init(GPIOC, &GPIO_InitStructure);        
    
   	// Set PA.12,PA.15 output, PA.12,PA.15 control LED3,LED2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;												  		
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
    
   	// Set PC.13 output, PC.13 control Heat5_Ctrl
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;												  		
	GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
	// Set PB.13 input, PB.13 Check DOOR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 			
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);   
    
	// Set PD.8 input, PD.8 Check Key_Down
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		      
	GPIO_Init(GPIOD, &GPIO_InitStructure);	    
    
	// Set PD.13 input, PD.13 Check Key_Up
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		      
	GPIO_Init(GPIOD, &GPIO_InitStructure);	     
    
	// Set PD.14 input, PD.14 Check G_switch
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		      
	GPIO_Init(GPIOD, &GPIO_InitStructure);	     
    
	// Set PE.8,PE.10 input, PE.8,PE.10 Check Check1,Check2 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);
    
	// Set PE.11,PE.12 input, PE.11,PE.12 Check Check3,Check4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);	    
    
	// Set PE.13,PE.14 input, PE.11,PE.12 Check Check5,Check6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);	

	// Set PE.15 input, PE.15 Check Check7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);	    
    
	// Set PE.2 output, PE.2 control Heat1_Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;										
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    
	GPIO_Init(GPIOE, &GPIO_InitStructure);    
    
	// Set PE.3 output, PE.3 control Heat2_Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);       
    
	// Set PE.4 output, PE.4 control Heat3_Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);     
    
	// Set PE.6 output, PE.6 control Heat4_Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  
	GPIO_Init(GPIOE, &GPIO_InitStructure);       
}
/********************************************************\
* @brief  Function init
* @param  None    
* @retval None
* @note   None
\********************************************************/
void func_init(void)
{   
    func_gpio_init();
    inputSta.sta = 0;
    check1Cnt = 0;
    check2Cnt = 0;
    check3Cnt = 0;
    check4Cnt = 0;
    check5Cnt = 0;
    check6Cnt = 0;    
    check7Cnt = 0;  
    gSwitchCnt = 0;
    temp1CheckStep = 0;
    PIDInit(&t1PID);
    temp_pwm_init();
}
/********************************************************\
* @brief  Function related time update
* @param  None
* @retval None
* @note   This function is called in the timer interrupt	
\********************************************************/
void func_time_int(void)
{
    boardCheckIntCnt++;  
    inputCheckCnt++;
    if (sampleIntCnt > 0)
    sampleIntCnt--;  
    if (boardCheckCnt > 0)
    boardCheckCnt--;   
}
/********************************************************\
* @brief  Function time task
* @param  None
* @retval None
* @note   None
\********************************************************/
void func_time_task(void)
{
    if (boardCheckIntCnt >= T_20MIN)
    {
        boardCheckIntCnt = 0;
        f_flag.bits.boardCheck = 1;
    }     
    
    if (sampleIntCnt <= T_1MS)
    {
        sampleIntCnt = T_100MS;
        adc1_dma_conv();    // Start conversion once
    }    
}
/********************************************************\
* @brief  Function task
* @param  None
* @retval None
* @note   None	
\********************************************************/
void func_task(void)
{
    input_check();        
    one_board_check();    
    temp1_control();
    
    if (KEY1 == 0)
    {
        boardCheckIntCnt = 0;
        f_flag.bits.boardCheck = 1;        
    }     
}
/********************************************************\
* @brief  Check Check1-7, G_switch input
* @param  None
* @retval None
* @note   None
\********************************************************/
void input_check(void)
{
    if (inputCheckCnt < T_10MS) // 10ms
        return;
    
    inputCheckCnt = 0;               
    if (CHECK1 == 1)
    {
        if (check1Cnt < 10)    // 100ms    
        {
            check1Cnt++;
            if (check1Cnt >= 10)
            inputSta.bits.check1Sta = 1;   
        }        
    }
    else
    {
        if (check1Cnt > 0)
        {
            check1Cnt--;  
            if (check1Cnt < 1)
            inputSta.bits.check1Sta = 0;  
        }       
    }
         
    if (CHECK2 == 1)
    {
        if (check2Cnt < 10)    // 100ms    
        {
            check2Cnt++;
            if (check2Cnt >= 10)
            inputSta.bits.check2Sta = 1;   
        }        
    }
    else
    {
        if (check2Cnt > 0)
        {
            check2Cnt--;  
            if (check2Cnt < 1)
            inputSta.bits.check2Sta = 0;  
        }       
    }
  
    if (CHECK3 == 1)
    {
        if (check3Cnt < 10)    // 100ms     
        {
            check3Cnt++;
            if (check3Cnt >= 10)
            inputSta.bits.check3Sta = 1;   
        }        
    }
    else
    {
        if (check3Cnt > 0)
        {
            check3Cnt--;  
            if (check3Cnt < 1)
            inputSta.bits.check3Sta = 0;  
        }       
    }
   
    if (CHECK4 == 1)
    {
        if (check4Cnt < 10)    // 100ms       
        {
            check4Cnt++;
            if (check4Cnt >= 10)
            inputSta.bits.check4Sta = 1;   
        }        
    }
    else
    {
        if (check4Cnt > 0)
        {
            check4Cnt--;  
            if (check4Cnt < 1)
            inputSta.bits.check4Sta = 0;  
        }       
    }

    if (CHECK5 == 1)
    {
        if (check5Cnt < 10)    // 100ms     
        {
            check5Cnt++;
            if (check5Cnt >= 10)
            inputSta.bits.check5Sta = 1;   
        }        
    }
    else
    {
        if (check5Cnt > 0)
        {
            check5Cnt--;  
            if (check5Cnt < 1)
            inputSta.bits.check5Sta = 0;  
        }       
    }

    if (CHECK6 == 1)
    {
        if (check6Cnt < 10)    // 100ms     
        {
            check6Cnt++;
            if (check6Cnt >= 10)
            inputSta.bits.check6Sta = 1;   
        }        
    }
    else
    {
        if (check6Cnt > 0)
        {
            check6Cnt--;  
            if (check6Cnt < 1)
            inputSta.bits.check6Sta = 0;  
        }       
    }    
      
    if (CHECK7 == 1)
    {
        if (check7Cnt < 10)    // 100ms    
        {
            check7Cnt++;
            if (check7Cnt >= 10)
            inputSta.bits.check7Sta = 1;   
        }        
    }
    else
    {
        if (check7Cnt > 0)
        {
            check7Cnt--;  
            if (check7Cnt < 1)
            inputSta.bits.check7Sta = 0;  
        }       
    }  
    
    if (G_SWITCH == 0)
    {
        if (gSwitchCnt < 10)    // 100ms    
        {
            gSwitchCnt++;
            if (gSwitchCnt >= 10)
            inputSta.bits.gSwitchSta = 1;   
        }        
    }
    else
    {
        if (gSwitchCnt > 0)
        {
            gSwitchCnt--;  
            if (gSwitchCnt < 1)
            inputSta.bits.gSwitchSta = 0;  
        }       
    } 

    if (KEY_UP == 0)
    {
        if (keyUpCnt < 10)     // 100ms     
        {
            keyUpCnt++;
            if (keyUpCnt >= 10)
            inputSta.bits.keyUpSta = 1;   
        }        
    }
    else
    {
        if (keyUpCnt > 0)
        {
            keyUpCnt--;  
            if (keyUpCnt < 1)
            inputSta.bits.keyUpSta = 0;  
        }       
    } 
    
    if (KEY_DOWN == 0)
    {
        if (keyDownCnt < 10)    // 100ms     
        {
            keyDownCnt++;
            if (keyDownCnt >= 10)
            inputSta.bits.keyDownSta = 1;   
        }        
    }
    else
    {
        if (keyDownCnt > 0)
        {
            keyDownCnt--;  
            if (keyDownCnt < 1)
            inputSta.bits.keyDownSta = 0;  
        }       
    }    
    
    if (KEY_STOP == 0)
    {
        if (keyStopCnt < 10)    // 100ms      
        {
            keyStopCnt++;
            if (keyStopCnt >= 10)
            inputSta.bits.keyStopSta = 1;   
        }        
    }
    else
    {
        if (keyStopCnt > 0)
        {
            keyStopCnt--;  
            if (keyStopCnt < 1)
            inputSta.bits.keyStopSta = 0;  
        }       
    }   

    if (DOOR == 0)
    {
        if (doorCnt < 10)      // 100ms    
        {
            doorCnt++;
            if (doorCnt >= 10)
            inputSta.bits.doorSta = 1;   
        }        
    }
    else
    {
        if (doorCnt > 0)
        {
            doorCnt--;  
            if (doorCnt < 1)
            inputSta.bits.doorSta = 0;  
        }       
    }     
}
/********************************************************\
* @brief  Initialize PID Structure
* @param  None
* @retval None
* @note   None	
\********************************************************/
void PIDInit(PID *pp)
{
	memset(pp, 0, sizeof(struct PID));
    t1PID.SetPoint = 350;
	t1PID.Proportion = 10;	// Set PID Coefficients
	t1PID.Integral = 5;
	t1PID.Derivative = 4;    
}
/********************************************************\
* @brief  PID calculation
* @param  None
* @retval None
* @note   None	
\********************************************************/
s16 PIDCalc(PID *pp, u16 NextPoint)
{
	u16 dError,Error,pError; 
	// Incremental method calculation formula: 
	// Pdt = Kp*[E(t)-E(t-1)]+Ki*E(t)+Kd*[E(t)-2*E(t-1)+E(t-2)]
	Error = pp->SetPoint - NextPoint;   // Deviation: E(t)
	pError = Error - pp->LastError;     // E(t)-E(t-1)
	dError = Error - 2*pp->LastError + pp->PrevError; // E(t)-2*E(t-1)+E(t-2)
	pp->PrevError = pp->LastError;
	pp->LastError = Error;
	
	return 
	(
		pp->Proportion * pError		    // Proportion item
		+ pp->Integral * Error		    // Integral term
		+ pp->Derivative * dError	    // Differential term
	);	
}
/********************************************************\
* @brief  temperature pwm init
* @param  None
* @retval None
* @note   None	
\********************************************************/
void temp_pwm_init(void)
{ 
    uint16_t PrescalerValue = 0;  
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    t1CCR2_Val = 1000;

    // TIM2 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);     

    // GPIOB clock enable 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  

    // GPIOB Configuration: TIM2 CH2 (PB3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    // Connect TIM2 pins to AF2   
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);   

    /* -----------------------------------------------------------------------
    TIM2 Configuration: generate 1 PWM signals.

    In this example TIM2 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM2CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM2CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM2 counter clock at 20 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /20 MHz) - 1
                                              
    To get TIM2 output clock at 10 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 2000
                  
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR)* 100 = 50%

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
    ----------------------------------------------------------------------- */   

    // Compute the prescaler value 
    PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 20000000) - 1;  

    // Time base configuration 
    TIM_TimeBaseStructure.TIM_Period = 1999;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;   // 0.05us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // PWM1 Mode configuration: Channel2 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    
    // Power on Channel2 PWM output enable    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    
    TIM_OCInitStructure.TIM_Pulse = t1CCR2_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);      

    // Enable TIM3 peripheral Preload register on CCR2
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);       

    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // TIM2 enable counter 
    TIM_Cmd(TIM2, ENABLE);           
}
/********************************************************\
* @brief  temperature 1 control
* @param  None
* @retval None
* @note   None	
\********************************************************/
void temp1_control(void)
{
    s16 temp;
    switch (temp1CheckStep) {
    case 0:
        if (a_flag.bits.adValue1Ok == 1)
        {
            a_flag.bits.adValue1Ok = 0;
            temp = temp1_measure(adValue1); 
            if (t1PID.SetPoint > temp) 
            {
                if (t1PID.SetPoint - temp > 50)
                {                  
                    // PWM output is high, duty cycle is the biggest, Full speed heating    
                    TIM2->CCR2 = 2000;
                    
                }
                else
                {
                    temp1CheckStep++;
                    temp1CheckCnt = 0;
                }
            }
            // The target temperature is less than the actual temperature, 
            // the PWM output is low, the duty cycle is highest, and the heating is turned off.
            else if (t1PID.SetPoint <= temp)
            {
                TIM2->CCR2 = 0;
            }
        }
        break;
    case 1:
        if (a_flag.bits.adValue1Ok == 1)
        {
            a_flag.bits.adValue1Ok = 0;      
            vIn = temp1_measure(adValue1);  
            // The PID increment output, The PID increment output range: 0-255, 
            // Matching the range of PWM value.
            vOut = PIDCalc(&t1PID, vIn); 
            // PID increment control in 5 degree range, 10 cycles, T=10 in PID integral formula.
            temp1CheckCnt++;            
            if (temp1CheckCnt >= 10)    
            {                           
                if (vOut >= 1999) vOut = 1999;	                
                if (vOut <= 1) vOut = 1;                
                //TIM2->CCR2 = 2000 - vOut;    
                TIM2->CCR2 = vOut;       
                temp1CheckStep = 0;                
            }
        }          
        break;
    default:
        temp1CheckStep = 0;
        break;
    }
}
/********************************************************\
* @brief  Heat1 control
* @param  None
* @retval None
* @note   None
\********************************************************/
void heat1_control(u32 sta)
{
    if (sta == HEAT_ON)
    {
        HEAT1_CTRL = 1;
        heat1CtrlSta = 1;
    }  
    else
    {
        HEAT1_CTRL = 0;
        heat1CtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Heat2 control
* @param  None
* @retval None
* @note   None
\********************************************************/
void heat2_control(u32 sta)
{
    if (sta == HEAT_ON)
    {
        HEAT2_CTRL = 1;
        heat2CtrlSta = 1;
    }  
    else
    {
        HEAT2_CTRL = 0;
        heat2CtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Heat3 control
* @param  None
* @retval None
* @note   None
\********************************************************/
void heat3_control(u32 sta)
{
    if (sta == HEAT_ON)
    {
        HEAT3_CTRL = 1;
        heat3CtrlSta = 1;
    }  
    else
    {
        HEAT3_CTRL = 0;
        heat3CtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Heat4 control
* @param  None
* @retval None
* @note   None
\********************************************************/
void heat4_control(u32 sta)
{
    if (sta == HEAT_ON)
    {
        HEAT4_CTRL = 1;
        heat4CtrlSta = 1;
    }  
    else
    {
        HEAT4_CTRL = 0;
        heat4CtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Heat5 control
* @param  None
* @retval None
* @note   None
\********************************************************/
void heat5_control(u32 sta)
{
    if (sta == HEAT_ON)
    {
        HEAT5_CTRL = 1;
        heat5CtrlSta = 1;
    }  
    else
    {
        HEAT5_CTRL = 0;
        heat5CtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Fan control
* @param  None
* @retval None
* @note   None
\********************************************************/
void fan_control(u32 sta)
{
    if (sta == FAN_ON)
    {
        FAN_CTRL = 1;
        fanCtrlSta = 1;
    }  
    else
    {
        FAN_CTRL = 0;
        fanCtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Valve control
* @param  None
* @retval None
* @note   None
\********************************************************/
void valve_control(u32 sta)
{
    if (sta == VALVE_ON)
    {
        VALVE_CTRL = 1;
        valveCtrlSta = 1;
    }  
    else
    {
        VALVE_CTRL = 0;
        valveCtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Compressor control
* @param  None
* @retval None
* @note   None
\********************************************************/
void comp_control(u32 sta)
{
    if (sta == COMP_ON)
    {
        COMP_CTRL = 1;
        compCtrlSta = 1;
    }  
    else
    {
        COMP_CTRL = 0;
        compCtrlSta = 0;        
    }
}
/********************************************************\
* @brief  Color frequency printf
* @param  None
* @retval None
* @note   None
\********************************************************/
void color_freq_printf(void)
{	
    u32 i;  
    u8 crc;    
    memset(tempBuf1, 0, sizeof(tempBuf1));
    // Message head
    tempBuf1[0] = SOH;              
    // Cmd(Send color frequency)
    tempBuf1[1] = 0x80;       
    tempBuf1[2] = 0x00;  // Status
    tempBuf1[3] = (869>>8)&0xff;    // Length high byte   
    tempBuf1[4] = 869&0xff;     // Length low byte      
    for (i = 0; i < 72; i++)
    {
        // Color_O1+N red frequency 
        tempBuf1[i*12+5] = (u8)(freqBuffer[i][0]&0xff);     
        tempBuf1[i*12+6] = (u8)((freqBuffer[i][0]>>8)&0xff);
        tempBuf1[i*12+7] = (u8)((freqBuffer[i][0]>>16)&0xff); 
        
        // Color_O1+N blue frequency             
        tempBuf1[i*12+8] = (u8)(freqBuffer[i][1]&0xff);     
        tempBuf1[i*12+9] = (u8)((freqBuffer[i][1]>>8)&0xff);
        tempBuf1[i*12+10] = (u8)((freqBuffer[i][1]>>16)&0xff); 
        
        // Color_O1+N clear frequency             
        tempBuf1[i*12+11] = (u8)(freqBuffer[i][2]&0xff);     
        tempBuf1[i*12+12] = (u8)((freqBuffer[i][2]>>8)&0xff);
        tempBuf1[i*12+13] = (u8)((freqBuffer[i][2]>>16)&0xff); 
        
        // Color_O1+N green frequency             
        tempBuf1[i*12+14] = (u8)(freqBuffer[i][3]&0xff);     
        tempBuf1[i*12+15] = (u8)((freqBuffer[i][3]>>8)&0xff);
        tempBuf1[i*12+16] = (u8)((freqBuffer[i][3]>>16)&0xff);             
    }
           
    crc = 0;
    for (i = 0; i < 5+72*4*3; i++)
    crc = cal_crc8(tempBuf1[i], crc, GENP0LY8);  
    tempBuf1[5+72*4*3] = crc;  // CRC check
    tempBuf1[5+72*4*3+1] = EOT;  // Message tail    
    rs485_send_buffer(tempBuf1, 5+72*4*3+2);     
}
/********************************************************\
* @brief  One board check handle, One board has 
* @brief  72 points to be detected. 
* @param  None
* @retval None
* @note   None
\********************************************************/
void one_board_check(void)
{	
    if (f_flag.bits.boardCheck == 0)
        return;
    
    switch(boardCheckStep) {
    case 0:  
        boardCheckCnt = T_10S;            
        write_tlc59711((u16 *)TLC59711_GS1);    // LED on
        boardCheckStep++;          
        break;
    case 1:  
        if (boardCheckCnt <= T_1MS)
        boardCheckStep++;            
        break;        
    case 2:
        boardCheckStep++;
        if (inputSta.bits.check3Sta == 0)
        // Move from top to bottom
        set_step_forward(STEP_CH2, 0);           
        break;
    case 3:
        if (inputSta.bits.check3Sta == 1) 
        {
            boardCheckStep++;       
            set_step_stop(STEP_CH2);
        }
        break;        
    // Take a board from board groove.
    case 4:  
        // Step number parameter setting 0,
        // Means that the step motor keeps turning.
        set_step_forward(STEP_CH1, 0);
        boardCheckStep++;
        break;
    case 5:  
        // Move to stop at specified position,
        // Position detection using groove photoelectric switches.
        // Locate the first row checkpoint position.
        if (inputSta.bits.check1Sta == 1)
        {
            set_step_stop(STEP_CH1);
            boardCheckStep = 10;
        }
        break;          
        
    // First line: 1,3,5,7,9 
    case 10:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 11:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;
    case 12:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[0][0] = colorFreq[0];
            freqBuffer[2][0] = colorFreq[2];
            freqBuffer[4][0] = colorFreq[4];
            freqBuffer[6][0] = colorFreq[6];     
            freqBuffer[8][0] = colorFreq[8];                       
            boardCheckStep++;            
        }
        break;        
    case 13:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 14:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;        
    case 15:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[0][1] = colorFreq[0];
            freqBuffer[2][1] = colorFreq[2];
            freqBuffer[4][1] = colorFreq[4];
            freqBuffer[6][1] = colorFreq[6];     
            freqBuffer[8][1] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;         
    case 16:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_120MS;
        boardCheckStep++;
        break;           
    case 17:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[0][2] = colorFreq[0];
            freqBuffer[2][2] = colorFreq[2];
            freqBuffer[4][2] = colorFreq[4];
            freqBuffer[6][2] = colorFreq[6];     
            freqBuffer[8][2] = colorFreq[8];             
            boardCheckStep++;            
        }
        break;        
    case 18:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;         
    case 19:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 20:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[0][3] = colorFreq[0];
            freqBuffer[2][3] = colorFreq[2];
            freqBuffer[4][3] = colorFreq[4];
            freqBuffer[6][3] = colorFreq[6];     
            freqBuffer[8][3] = colorFreq[8];            
            boardCheckStep = 25;            
        }
        break;   
    
    // Board move 10.5mm
    case 25:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 26:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 30;
        break;
        
    // First line: 2,4,6,8  
    // Second line: 1,3,5,7,9          
    case 30:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;   
    case 31:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 32:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1][0] = colorFreq[1];
            freqBuffer[3][0] = colorFreq[3];
            freqBuffer[5][0] = colorFreq[5];
            freqBuffer[7][0] = colorFreq[7];              
            freqBuffer[1*9+0][0] = colorFreq[0];
            freqBuffer[1*9+2][0] = colorFreq[2];
            freqBuffer[1*9+4][0] = colorFreq[4];
            freqBuffer[1*9+6][0] = colorFreq[6];     
            freqBuffer[1*9+8][0] = colorFreq[8];                      
            boardCheckStep++;            
        }
        break;        
    case 33:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;     
    case 34:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 35:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1][1] = colorFreq[1];
            freqBuffer[3][1] = colorFreq[3];
            freqBuffer[5][1] = colorFreq[5];
            freqBuffer[7][1] = colorFreq[7];              
            freqBuffer[1*9+0][1] = colorFreq[0];
            freqBuffer[1*9+2][1] = colorFreq[2];
            freqBuffer[1*9+4][1] = colorFreq[4];
            freqBuffer[1*9+6][1] = colorFreq[6];     
            freqBuffer[1*9+8][1] = colorFreq[8];             
            boardCheckStep++;            
        }
        break;         
    case 36:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;         
    case 37:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 38:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1][2] = colorFreq[1];
            freqBuffer[3][2] = colorFreq[3];
            freqBuffer[5][2] = colorFreq[5];
            freqBuffer[7][2] = colorFreq[7];              
            freqBuffer[1*9+0][2] = colorFreq[0];
            freqBuffer[1*9+2][2] = colorFreq[2];
            freqBuffer[1*9+4][2] = colorFreq[4];
            freqBuffer[1*9+6][2] = colorFreq[6];     
            freqBuffer[1*9+8][2] = colorFreq[8];              
            boardCheckStep++;            
        }
        break;        
    case 39:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;      
    case 40:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 41:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1][3] = colorFreq[1];
            freqBuffer[3][3] = colorFreq[3];
            freqBuffer[5][3] = colorFreq[5];
            freqBuffer[7][3] = colorFreq[7];              
            freqBuffer[1*9+0][3] = colorFreq[0];
            freqBuffer[1*9+2][3] = colorFreq[2];
            freqBuffer[1*9+4][3] = colorFreq[4];
            freqBuffer[1*9+6][3] = colorFreq[6];     
            freqBuffer[1*9+8][3] = colorFreq[8];              
            boardCheckStep = 45;            
        }
        break;
        
    // Board move 10.5mm
    case 45:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 46:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 50;
        break;        
        
    // Second line: 2,4,6,8
    // Third line: 1,3,5,7,9         
    case 50:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;       
    case 51:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 52:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1*9+1][0] = colorFreq[1];
            freqBuffer[1*9+3][0] = colorFreq[3];
            freqBuffer[1*9+5][0] = colorFreq[5];
            freqBuffer[1*9+7][0] = colorFreq[7];           
            freqBuffer[2*9+0][0] = colorFreq[0];
            freqBuffer[2*9+2][0] = colorFreq[2];
            freqBuffer[2*9+4][0] = colorFreq[4];
            freqBuffer[2*9+6][0] = colorFreq[6];     
            freqBuffer[2*9+8][0] = colorFreq[8];                        
            boardCheckStep++;            
        }
        break;        
    case 53:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;         
    case 54:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }        
        break;         
    case 55:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1*9+1][1] = colorFreq[1];
            freqBuffer[1*9+3][1] = colorFreq[3];
            freqBuffer[1*9+5][1] = colorFreq[5];
            freqBuffer[1*9+7][1] = colorFreq[7];            
            freqBuffer[2*9+0][1] = colorFreq[0];
            freqBuffer[2*9+2][1] = colorFreq[2];
            freqBuffer[2*9+4][1] = colorFreq[4];
            freqBuffer[2*9+6][1] = colorFreq[6];     
            freqBuffer[2*9+8][1] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;         
    case 56:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;           
    case 57:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 58:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1*9+1][2] = colorFreq[1];
            freqBuffer[1*9+3][2] = colorFreq[3];
            freqBuffer[1*9+5][2] = colorFreq[5];
            freqBuffer[1*9+7][2] = colorFreq[7];           
            freqBuffer[2*9+0][2] = colorFreq[0];
            freqBuffer[2*9+2][2] = colorFreq[2];
            freqBuffer[2*9+4][2] = colorFreq[4];
            freqBuffer[2*9+6][2] = colorFreq[6];     
            freqBuffer[2*9+8][2] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;        
    case 59:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;         
    case 60:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 61:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[1*9+1][3] = colorFreq[1];
            freqBuffer[1*9+3][3] = colorFreq[3];
            freqBuffer[1*9+5][3] = colorFreq[5];
            freqBuffer[1*9+7][3] = colorFreq[7];           
            freqBuffer[2*9+0][3] = colorFreq[0];
            freqBuffer[2*9+2][3] = colorFreq[2];
            freqBuffer[2*9+4][3] = colorFreq[4];
            freqBuffer[2*9+6][3] = colorFreq[6];     
            freqBuffer[2*9+8][3] = colorFreq[8];             
            boardCheckStep = 65;            
        }
        break;    
        
    // Board move 10.5mm
    case 65:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 66:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 70;
        break;        
        
    // Third line: 2,4,6,8
    // Fourth line: 1,3,5,7,9          
    case 70:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 71:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 72:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[2*9+1][0] = colorFreq[1];
            freqBuffer[2*9+3][0] = colorFreq[3];
            freqBuffer[2*9+5][0] = colorFreq[5];
            freqBuffer[2*9+7][0] = colorFreq[7];           
            freqBuffer[3*9+0][0] = colorFreq[0];
            freqBuffer[3*9+2][0] = colorFreq[2];
            freqBuffer[3*9+4][0] = colorFreq[4];
            freqBuffer[3*9+6][0] = colorFreq[6];     
            freqBuffer[3*9+8][0] = colorFreq[8];                        
            boardCheckStep++;            
        }
        break;        
    case 73:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;   
    case 74:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 75:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[2*9+1][1] = colorFreq[1];
            freqBuffer[2*9+3][1] = colorFreq[3];
            freqBuffer[2*9+5][1] = colorFreq[5];
            freqBuffer[2*9+7][1] = colorFreq[7];           
            freqBuffer[3*9+0][1] = colorFreq[0];
            freqBuffer[3*9+2][1] = colorFreq[2];
            freqBuffer[3*9+4][1] = colorFreq[4];
            freqBuffer[3*9+6][1] = colorFreq[6];     
            freqBuffer[3*9+8][1] = colorFreq[8];             
            boardCheckStep++;            
        }
        break;         
    case 76:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;    
    case 77:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 78:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[2*9+1][2] = colorFreq[1];
            freqBuffer[2*9+3][2] = colorFreq[3];
            freqBuffer[2*9+5][2] = colorFreq[5];
            freqBuffer[2*9+7][2] = colorFreq[7];            
            freqBuffer[3*9+0][2] = colorFreq[0];
            freqBuffer[3*9+2][2] = colorFreq[2];
            freqBuffer[3*9+4][2] = colorFreq[4];
            freqBuffer[3*9+6][2] = colorFreq[6];     
            freqBuffer[3*9+8][2] = colorFreq[8];           
            boardCheckStep++;            
        }
        break;        
    case 79:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_120MS;
        boardCheckStep++;
        break;          
    case 80:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 81:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[2*9+1][3] = colorFreq[1];
            freqBuffer[2*9+3][3] = colorFreq[3];
            freqBuffer[2*9+5][3] = colorFreq[5];
            freqBuffer[2*9+7][3] = colorFreq[7];          
            freqBuffer[3*9+0][3] = colorFreq[0];
            freqBuffer[3*9+2][3] = colorFreq[2];
            freqBuffer[3*9+4][3] = colorFreq[4];
            freqBuffer[3*9+6][3] = colorFreq[6];     
            freqBuffer[3*9+8][3] = colorFreq[8];              
            boardCheckStep = 85;            
        }
        break;   
        
    // Board move 10.5mm
    case 85:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;   
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 86:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 90;
        break;        
        
    // Fourth line: 2,4,6,8
    // Fifth line: 1,3,5,7,9          
    case 90:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 91:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 92:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[3*9+1][0] = colorFreq[1];
            freqBuffer[3*9+3][0] = colorFreq[3];
            freqBuffer[3*9+5][0] = colorFreq[5];
            freqBuffer[3*9+7][0] = colorFreq[7];          
            freqBuffer[4*9+0][0] = colorFreq[0];
            freqBuffer[4*9+2][0] = colorFreq[2];
            freqBuffer[4*9+4][0] = colorFreq[4];
            freqBuffer[4*9+6][0] = colorFreq[6];     
            freqBuffer[4*9+8][0] = colorFreq[8];                         
            boardCheckStep++;            
        }
        break;        
    case 93:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;   
    case 94:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 95:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[3*9+1][1] = colorFreq[1];
            freqBuffer[3*9+3][1] = colorFreq[3];
            freqBuffer[3*9+5][1] = colorFreq[5];
            freqBuffer[3*9+7][1] = colorFreq[7];          
            freqBuffer[4*9+0][1] = colorFreq[0];
            freqBuffer[4*9+2][1] = colorFreq[2];
            freqBuffer[4*9+4][1] = colorFreq[4];
            freqBuffer[4*9+6][1] = colorFreq[6];     
            freqBuffer[4*9+8][1] = colorFreq[8];              
            boardCheckStep++;            
        }
        break;         
    case 96:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 97:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 98:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[3*9+1][2] = colorFreq[1];
            freqBuffer[3*9+3][2] = colorFreq[3];
            freqBuffer[3*9+5][2] = colorFreq[5];
            freqBuffer[3*9+7][2] = colorFreq[7];           
            freqBuffer[4*9+0][2] = colorFreq[0];
            freqBuffer[4*9+2][2] = colorFreq[2];
            freqBuffer[4*9+4][2] = colorFreq[4];
            freqBuffer[4*9+6][2] = colorFreq[6];     
            freqBuffer[4*9+8][2] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;        
    case 99:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;           
    case 100:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 101:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[3*9+1][3] = colorFreq[1];
            freqBuffer[3*9+3][3] = colorFreq[3];
            freqBuffer[3*9+5][3] = colorFreq[5];
            freqBuffer[3*9+7][3] = colorFreq[7];            
            freqBuffer[4*9+0][3] = colorFreq[0];
            freqBuffer[4*9+2][3] = colorFreq[2];
            freqBuffer[4*9+4][3] = colorFreq[4];
            freqBuffer[4*9+6][3] = colorFreq[6];     
            freqBuffer[4*9+8][3] = colorFreq[8];            
            boardCheckStep = 105;            
        }
        break;
        
    // Board move 10.5mm
    case 105:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;   
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 106:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 110;
        break;        
        
    // Fifth line: 2,4,6,8 
    // Sixth line: 1,3,5,7,9         
    case 110:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 111:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 112:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[4*9+1][0] = colorFreq[1];
            freqBuffer[4*9+3][0] = colorFreq[3];
            freqBuffer[4*9+5][0] = colorFreq[5];
            freqBuffer[4*9+7][0] = colorFreq[7];          
            freqBuffer[5*9+0][0] = colorFreq[0];
            freqBuffer[5*9+2][0] = colorFreq[2];
            freqBuffer[5*9+4][0] = colorFreq[4];
            freqBuffer[5*9+6][0] = colorFreq[6];     
            freqBuffer[5*9+8][0] = colorFreq[8];                         
            boardCheckStep++;            
        }
        break;        
    case 113:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;     
    case 114:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 115:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[4*9+1][1] = colorFreq[1];
            freqBuffer[4*9+3][1] = colorFreq[3];
            freqBuffer[4*9+5][1] = colorFreq[5];
            freqBuffer[4*9+7][1] = colorFreq[7];          
            freqBuffer[5*9+0][1] = colorFreq[0];
            freqBuffer[5*9+2][1] = colorFreq[2];
            freqBuffer[5*9+4][1] = colorFreq[4];
            freqBuffer[5*9+6][1] = colorFreq[6];     
            freqBuffer[5*9+8][1] = colorFreq[8];              
            boardCheckStep++;            
        }
        break;         
    case 116:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;      
    case 117:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 118:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[4*9+1][2] = colorFreq[1];
            freqBuffer[4*9+3][2] = colorFreq[3];
            freqBuffer[4*9+5][2] = colorFreq[5];
            freqBuffer[4*9+7][2] = colorFreq[7];           
            freqBuffer[5*9+0][2] = colorFreq[0];
            freqBuffer[5*9+2][2] = colorFreq[2];
            freqBuffer[5*9+4][2] = colorFreq[4];
            freqBuffer[5*9+6][2] = colorFreq[6];     
            freqBuffer[5*9+8][2] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;        
    case 119:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;   
    case 120:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 121:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[4*9+1][3] = colorFreq[1];
            freqBuffer[4*9+3][3] = colorFreq[3];
            freqBuffer[4*9+5][3] = colorFreq[5];
            freqBuffer[4*9+7][3] = colorFreq[7];            
            freqBuffer[5*9+0][3] = colorFreq[0];
            freqBuffer[5*9+2][3] = colorFreq[2];
            freqBuffer[5*9+4][3] = colorFreq[4];
            freqBuffer[5*9+6][3] = colorFreq[6];     
            freqBuffer[5*9+8][3] = colorFreq[8];            
            boardCheckStep = 125;            
        }
        break;
        
    // Board move 10.5mm
    case 125:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 126:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 130;
        break;        
        
    // Sixth line: 2,4,6,8
    // Seventh line: 1,3,5,7,9          
    case 130:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 131:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 132:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[5*9+1][0] = colorFreq[1];
            freqBuffer[5*9+3][0] = colorFreq[3];
            freqBuffer[5*9+5][0] = colorFreq[5];
            freqBuffer[5*9+7][0] = colorFreq[7];           
            freqBuffer[6*9+0][0] = colorFreq[0];
            freqBuffer[6*9+2][0] = colorFreq[2];
            freqBuffer[6*9+4][0] = colorFreq[4];
            freqBuffer[6*9+6][0] = colorFreq[6];     
            freqBuffer[6*9+8][0] = colorFreq[8];                        
            boardCheckStep++;            
        }
        break;        
    case 133:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;     
    case 134:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 135:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[5*9+1][1] = colorFreq[1];
            freqBuffer[5*9+3][1] = colorFreq[3];
            freqBuffer[5*9+5][1] = colorFreq[5];
            freqBuffer[5*9+7][1] = colorFreq[7];            
            freqBuffer[6*9+0][1] = colorFreq[0];
            freqBuffer[6*9+2][1] = colorFreq[2];
            freqBuffer[6*9+4][1] = colorFreq[4];
            freqBuffer[6*9+6][1] = colorFreq[6];     
            freqBuffer[6*9+8][1] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;         
    case 136:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;      
    case 137:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 138:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[5*9+1][2] = colorFreq[1];
            freqBuffer[5*9+3][2] = colorFreq[3];
            freqBuffer[5*9+5][2] = colorFreq[5];
            freqBuffer[5*9+7][2] = colorFreq[7];           
            freqBuffer[6*9+0][2] = colorFreq[0];
            freqBuffer[6*9+2][2] = colorFreq[2];
            freqBuffer[6*9+4][2] = colorFreq[4];
            freqBuffer[6*9+6][2] = colorFreq[6];     
            freqBuffer[6*9+8][2] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;        
    case 139:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;    
    case 140:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 141:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[5*9+1][3] = colorFreq[1];
            freqBuffer[5*9+3][3] = colorFreq[3];
            freqBuffer[5*9+5][3] = colorFreq[5];
            freqBuffer[5*9+7][3] = colorFreq[7];           
            freqBuffer[6*9+0][3] = colorFreq[0];
            freqBuffer[6*9+2][3] = colorFreq[2];
            freqBuffer[6*9+4][3] = colorFreq[4];
            freqBuffer[6*9+6][3] = colorFreq[6];     
            freqBuffer[6*9+8][3] = colorFreq[8];             
            boardCheckStep = 145;            
        }
        break;  
        
    // Board move 10.5mm
    case 145:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 146:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 150;
        break;        
        
    // Seventh line: 2,4,6,8 
    // Eighth line: 1,3,5,7,9         
    case 150:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;     
    case 151:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;          
    case 152:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[6*9+1][0] = colorFreq[1];
            freqBuffer[6*9+3][0] = colorFreq[3];
            freqBuffer[6*9+5][0] = colorFreq[5];
            freqBuffer[6*9+7][0] = colorFreq[7];            
            freqBuffer[7*9+0][0] = colorFreq[0];
            freqBuffer[7*9+2][0] = colorFreq[2];
            freqBuffer[7*9+4][0] = colorFreq[4];
            freqBuffer[7*9+6][0] = colorFreq[6];     
            freqBuffer[7*9+8][0] = colorFreq[8];                       
            boardCheckStep++;            
        }
        break;        
    case 153:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;     
    case 154:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 155:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[6*9+1][1] = colorFreq[1];
            freqBuffer[6*9+3][1] = colorFreq[3];
            freqBuffer[6*9+5][1] = colorFreq[5];
            freqBuffer[6*9+7][1] = colorFreq[7];            
            freqBuffer[7*9+0][1] = colorFreq[0];
            freqBuffer[7*9+2][1] = colorFreq[2];
            freqBuffer[7*9+4][1] = colorFreq[4];
            freqBuffer[7*9+6][1] = colorFreq[6];     
            freqBuffer[7*9+8][1] = colorFreq[8];            
            boardCheckStep++;            
        }
        break;         
    case 156:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;           
    case 157:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 158:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[6*9+1][2] = colorFreq[1];
            freqBuffer[6*9+3][2] = colorFreq[3];
            freqBuffer[6*9+5][2] = colorFreq[5];
            freqBuffer[6*9+7][2] = colorFreq[7];            
            freqBuffer[7*9+0][2] = colorFreq[0];
            freqBuffer[7*9+2][2] = colorFreq[2];
            freqBuffer[7*9+4][2] = colorFreq[4];
            freqBuffer[7*9+6][2] = colorFreq[6];     
            freqBuffer[7*9+8][2] = colorFreq[8];           
            boardCheckStep++;            
        }
        break;        
    case 159:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;      
    case 160:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 161:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[6*9+1][3] = colorFreq[1];
            freqBuffer[6*9+3][3] = colorFreq[3];
            freqBuffer[6*9+5][3] = colorFreq[5];
            freqBuffer[6*9+7][3] = colorFreq[7];           
            freqBuffer[7*9+0][3] = colorFreq[0];
            freqBuffer[7*9+2][3] = colorFreq[2];
            freqBuffer[7*9+4][3] = colorFreq[4];
            freqBuffer[7*9+6][3] = colorFreq[6];     
            freqBuffer[7*9+8][3] = colorFreq[8];             
            boardCheckStep = 165;            
        }
        break;   
        
    // Board move 10.5mm
    case 165:
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;      
        // Step motor rotate 1 turns and the mechanical motion is 2 mm;
        // The spacing between the two row color sensors is 10.5MM;
        // So the 10.5MM needs to turn 5.25 turns, 5.25*360 = 1890 degrees;
        // So 1890 degrees need 1890/0.1125 steps, 1890/0.1125 = 16800;  
        set_step_forward(STEP_CH1, 16800);
        boardCheckStep++;
        break;
    case 166:        
        // Step motor stop indicating movement completed
        if (stepCh1Sta == 0)    
        boardCheckStep = 170;
        break;        
        
    // Eighth line: 2,4,6,8    
    case 170:        
        COLOR_S2_L();   
        COLOR_S3_L();   // Select red      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;        
    case 171:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 172:  
        if (boardCheckCnt <= T_1MS)
        {            
            freqBuffer[7*9+1][0] = colorFreq[1];
            freqBuffer[7*9+3][0] = colorFreq[3];
            freqBuffer[7*9+5][0] = colorFreq[5];
            freqBuffer[7*9+7][0] = colorFreq[7];             
            boardCheckStep++;            
        }
        break;         
    case 173:
        COLOR_S2_L();   
        COLOR_S3_H();   // Select blue      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;
    case 174:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 175:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[7*9+1][1] = colorFreq[1];
            freqBuffer[7*9+3][1] = colorFreq[3];
            freqBuffer[7*9+5][1] = colorFreq[5];
            freqBuffer[7*9+7][1] = colorFreq[7];               
            boardCheckStep++;            
        }
        break;         
    case 176:
        COLOR_S2_H();   
        COLOR_S3_L();   // Select clear      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;           
    case 177:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 178:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[7*9+1][2] = colorFreq[1];
            freqBuffer[7*9+3][2] = colorFreq[3];
            freqBuffer[7*9+5][2] = colorFreq[5];
            freqBuffer[7*9+7][2] = colorFreq[7];               
            boardCheckStep++;            
        }
        break;        
    case 179:
        COLOR_S2_H();   
        COLOR_S3_H();   // Select green      
        boardCheckCnt = T_10MS;
        boardCheckStep++;
        break;         
    case 180:  
        if (boardCheckCnt <= T_1MS)
        {
            boardCheckCnt = T_120MS;
            boardCheckStep++;            
        }
        break;         
    case 181:  
        if (boardCheckCnt <= T_1MS)
        {
            freqBuffer[7*9+1][3] = colorFreq[1];
            freqBuffer[7*9+3][3] = colorFreq[3];
            freqBuffer[7*9+5][3] = colorFreq[5];
            freqBuffer[7*9+7][3] = colorFreq[7];             
            write_tlc59711((u16 *)TLC59711_GS2);    // LED off
            boardCheckStep = 185;            
        }
        break;       
        
    // Return the board to the board groove.
    case 185:  
        // Step number parameter setting 0,
        // Means that the step motor keeps turning.
        set_step_backward(STEP_CH1, 0);
        boardCheckStep++;
        break;
    case 186:  
        // Move to stop at specified position,
        // Position detection using groove photoelectric switches.
        // Locate the board groove position.
        if (inputSta.bits.check2Sta == 1)
        {
            set_step_stop(STEP_CH1);
            boardCheckStep = 190;
        }
        break;    
    
    // Board move 20cm, Mmove from bottom to top, Move to constant temperature position
    case 190:     
        // 57H5605 the fixed step angle: 1.8 degree;
        // TB6560 the step mode: 16 microstep;
        // 57H5605 the actual step angle: 1.8/16 = 0.1125 degree;          
        // Step motor rotate 1 turns and the mechanical motion is 5 mm;
        // So the 200mm needs to turn 40 turns, 40*360 = 14400 degrees;
        // So 14400 degrees need 14400/0.1125 steps, 14400/0.1125 = 128000;       
        set_step_backward(STEP_CH2, 128000);
        f_flag.bits.boardCheck = 0;
        color_freq_printf();        
        boardCheckStep = 0;
        break;
        
    default:
        boardCheckStep = 0;
        break;
    }
}