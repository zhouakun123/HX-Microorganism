    heat1_control(HEAT_ON);
    heat2_control(HEAT_ON);    
    heat3_control(HEAT_ON); 
    heat4_control(HEAT_ON); 
    heat5_control(HEAT_ON); 
    fan_control(FAN_ON);   
    valve_control(VALVE_ON); 
    comp_control(COMP_ON);    
    delay_us(1000);     
    heat1_control(HEAT_OFF);         
    heat2_control(HEAT_OFF);          
    heat3_control(HEAT_OFF);
    heat4_control(HEAT_OFF);
    heat5_control(HEAT_OFF);
    fan_control(FAN_OFF);  
    valve_control(VALVE_OFF);
    comp_control(COMP_OFF);  
    delay_us(1000); 
    
        colorSel++;
        switch(colorSel) {
        case 0:
            COLOR_S2_L();   
            COLOR_S3_L();   // Select red            
            break;
        case 1:
            COLOR_S2_L();   
            COLOR_S3_H();   // Select blue              
            break;
        case 2:
            COLOR_S2_H();   
            COLOR_S3_L();   // Select clear              
            break;
        case 3:
            COLOR_S2_H();   
            COLOR_S3_H();   // Select green  
            colorSel = 0;            
            break;
        default:
            colorSel = 0;
            break;
        }     
    
static u32 TIM1_LastCnt;
static u32 TIM4_LastCnt;
static u32 TIM8_LastCnt;
static u32 TIM9_LastCnt;
static u32 TIM12_LastCnt;    
    
        temp = TIM9->CNT;        
        if (temp > TIM9_LastCnt)    // Color_O1         
        colorFreq[colorSel][0] = (temp - TIM9_LastCnt)*20;
        
        temp = TIM12->CNT;        
        if (temp > TIM12_LastCnt)   // Color_O4            
        colorFreq[colorSel][3] = (temp - TIM12_LastCnt)*20;                
        
        temp = TIM4->CNT;        
        if (temp > TIM4_LastCnt)    // Color_O7        
        colorFreq[colorSel][6] = (temp - TIM4_LastCnt)*20;              
        
        temp = TIM8->CNT;        
        if (temp > TIM8_LastCnt)    // Color_O8        
        colorFreq[colorSel][7] = (temp - TIM8_LastCnt)*20;               
        
        temp = TIM1->CNT;
        if (temp > TIM1_LastCnt)    // Color_O9
        colorFreq[colorSel][8] = (temp - TIM1_LastCnt)*20;     
    
        //TIM9_LastCnt = TIM9->CNT; 
        //TIM12_LastCnt = TIM12->CNT;  
        //TIM4_LastCnt = TIM4->CNT;  
        //TIM8_LastCnt = TIM8->CNT;        
        //TIM1_LastCnt = TIM1->CNT;    

        //else extClockFreq[0] = (0xffff - TIM9_LastCnt + temp)*10;
        //else extClockFreq[3] = (0xffff - TIM12_LastCnt + temp)*10; 
        //else extClockFreq[6] = (0xffff - TIM4_LastCnt + temp)*10; 
        //else extClockFreq[7] = (0xffff - TIM8_LastCnt + temp)*10;  
        //else extClockFreq[8] = (0xffff - TIM1_LastCnt + temp)*10;

        test = test ^ 0xff;
        if (test)
        GPIO_SetBits(GPIOA, GPIO_Pin_9);
        else 
        GPIO_ResetBits(GPIOA, GPIO_Pin_9);  

    // Set PA.9 Output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);          
        
    M1_MODE0_H();                
    M1_MODE1_H();               
    M1_MODE2_H();               
    M1_nENBL_H();                   
    M1_DIR_H();                   
    M1_DECAY_H();   
    
    M1_MODE0_L();                
    M1_MODE1_L();               
    M1_MODE2_L();               
    M1_nENBL_L();                   
    M1_DIR_L();                   
    M1_DECAY_L();     
    
    M2_MODE0_H();                
    M2_MODE1_H();               
    M2_MODE2_H();               
    M2_nENBL_H();                   
    M2_DIR_H();                   
    M2_DECAY_H();    
    
    M2_MODE0_L();                
    M2_MODE1_L();               
    M2_MODE2_L();               
    M2_nENBL_L();                   
    M2_DIR_L();                   
    M2_DECAY_L();
    
    set_step_forward(STEP_CH1);
    set_step_forward(STEP_CH2);
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2);    
    
    set_step_forward(STEP_CH1);    
    set_step_forward(STEP_CH2);    
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2); 
    set_step_backward(STEP_CH1);    
    set_step_backward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2);  
    set_step_forward(STEP_CH1);    
    set_step_forward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2); 
    set_step_backward(STEP_CH1);    
    set_step_backward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2); 
    set_step_forward(STEP_CH1);    
    set_step_forward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2); 
    set_step_backward(STEP_CH1);    
    set_step_backward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2); 
    set_step_forward(STEP_CH1);    
    set_step_forward(STEP_CH2); 
    set_step_stop(STEP_CH1);
    set_step_stop(STEP_CH2);    
    
        temp = temp ^ 0xff;
        if (temp)          
        GPIO_SetBits(GPIOC, GPIO_Pin_10);
        else 
        GPIO_ResetBits(GPIOC, GPIO_Pin_10);    