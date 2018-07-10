#define ADC1_DR_Address     ((uint32_t)0x4001204C)

#define N1                  12
#define N2                  12

typedef __packed union
{
    u32 flag; 
    __packed struct {         
    u32 adc1ConvOk: 1;  
    u32 adValue1Ok: 1;    
    u32 adValue2Ok: 1;
    u32 adValue3Ok: 1;
    u32 adValue4Ok: 1;
    u32 adValue5Ok: 1;
    u32 adValue6Ok: 1;
    u32 adValue7Ok: 1;    
    }bits;    
}A_FLAG;

extern u16 adValue1;
extern u16 adValue2;
extern u16 adValue3;
extern u16 adValue4;
extern u16 adValue5;
extern u16 adValue6;
extern A_FLAG a_flag;

void adc1_config(void);
void adc1_dma_conv(void);
