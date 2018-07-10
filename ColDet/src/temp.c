#include "stm32f2xx.h"
#include "temp.h"
#include "adc.h"
#include "func.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
u16 valueBuf1[N2+1];
u16 valueBuf2[N2+1];
u16 valueBuf3[N2+1];
u16 valueBuf4[N2+1];
u16 valueBuf5[N2+1];
static u16 buf1In = 0;
static u16 buf2In = 0;
static u16 buf3In = 0;
static u16 buf4In = 0;
static u16 buf5In = 0;
// ADC value = 4095*Rv/Rv+Rm, 
// Rm is divider resistance,
// Rv is NTC thermistor resistors,
// Thermistor: MF52E-103F3435FB,
// Temperature range: -40~125,		
// Rm = 7.5K,
const u16 NTCTAB[166] =
{
    3956,3948,3939,3930,3920,3911,3900,3890,
    3878,3867,3854,3842,3828,3815,3800,3786,
    3770,3754,3738,3721,3703,3685,3666,3647,
    3627,3606,3585,3563,3540,3517,3493,3469,
    3444,3418,3392,3365,3337,3309,3280,3251,
    3221,3191,3160,3128,3096,3063,3030,2997,
    2963,2929,2894,2859,2823,2787,2751,2715,
    2678,2641,2604,2566,2529,2491,2453,2415,
    2377,2340,2302,2264,2226,2188,2150,2113,
    2076,2038,1988,1951,1914,1877,1841,1805,
    1769,1734,1699,1665,1631,1597,1564,1531,
    1499,1467,1435,1405,1375,1345,1316,1287,
    1259,1231,1204,1178,1152,1126,1101,1076,
    1052,1028,1005, 982, 960, 938, 917, 914,
    893,  873, 853, 834, 815, 797, 779, 761,
    744,  727, 711, 694, 679, 663, 648, 634,
    619,  606, 592, 578, 566, 553, 540, 528,
    516,  504, 493, 482, 471, 461, 451, 441,
    431,  421, 412, 403, 394, 385, 377, 369,
    360,  353, 345, 338, 330, 323, 316, 309,
    303,  296, 290, 284, 278, 272,
};	 
/********************************************************\
* @brief  Search table
* @param  dat - Target data 
* @param  TABLE - Table pointer 
* @param  aptr - Storage pointer
* @retval None
* @note   aptr[0]; aptr[2] <= data < aptr[1]
\********************************************************/
void lookup_tab(u16 dat, const u16 *TABLE, u16 *aptr)
{
    u16 i;
    u16 *eptr = (u16 *)&TABLE[TableSize-1];	
    u16 *sptr = (u16 *)TABLE;			
    u16 *ptr;							
    for (i = 0; i < 8; i++)     // Search table					
    {		
        // Bisearch
        ptr = (u16  *)(sptr+((eptr-sptr)>>1));
        if (*ptr > dat)	
        sptr = ptr;
        else if (*ptr < dat)	
        eptr = ptr;
        else				
        {
            aptr[1] = *ptr;			
            aptr[2] = *(ptr+1);		
            aptr[0] = (u16)(ptr - TABLE);
            break;
        }

        if (eptr - sptr == 1)	
        {
            aptr[1] = *sptr;		
            aptr[2] = *eptr;		
            aptr[0] = (u16)(sptr - TABLE);
            break;
        }
    }
}
/********************************************************\
* @brief  linear insert 
* @param  dat - Current data
* @param  TABLE - Table pointer 
* @retval Temperature value
* @note   Reference lookup_tab()
\********************************************************/
signed int linear_insert(u16 dat,const u16 *TABLE)
{
    u16 Array[3];	
    signed int temp;     
    if (dat  > Data_Lowest)                // Exceeding Lowest Temperature
    {				
        if (dat < Data_Highest)            // Exceeding Highest Temperature
        {
            lookup_tab(dat,TABLE,Array);    // Lookup data														
            if (dat == Array[2])	
            temp = (signed int)Array[0] + ZERO_S;
            else 
            {   
                // Calculate temperature decimal part,
                // Keep the 1 bit behind the decimal point, 
                // For example: 255 = 25.5 degrees.
                temp = (signed int)((float)(dat-Array[2])/(Array[1]-Array[2])*10);
                // Add temperature integer part
                temp = temp + ((signed int)Array[0] + ZERO_S)*10;
            }
        }
        else
        {
           temp = 0x7FFF;                   // Mark Underflow
        }
    }
    else	
    {
        temp = 0xFFFF;						// Mark Overflow
    }

    return temp;
}		
/********************************************************\
* @brief  Temperature 1 measure
* @param  convVal - AD conversion value
* @retval Temperature value
* @note   None
\********************************************************/
signed int temp1_measure(u16 convVal)
{
    u32 i,sum,aver;	   
    s16 temp;
               
#if 1
    //-----------------------------------------------------    
    // The recursive average filtering algorithm method two  
    //-----------------------------------------------------
    valueBuf1[buf1In++] = convVal;
    if (buf1In == N2) buf1In = 0;
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        sum += valueBuf1[i];
    }
    
    aver = sum/N2; 
    // End
#endif    

#if 0    
    //-----------------------------------------------------     
    // The recursive average filtering algorithm method one
    //----------------------------------------------------- 
    // The collected data is placed in the highest position
    valueBuf1[N2] = aver;          
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        // All data left shift, low throw
        valueBuf1[i] = valueBuf1[i+1];    
        sum += valueBuf1[i];
    }
    
    aver = sum/N2;   
    // End
#endif       
        
    temp = linear_insert((u16)aver,NTCTAB);
    // Keep the 1 bit behind the decimal point,
    // For example: 255 = 25.5 degrees.
    return temp;
}
/********************************************************\
* @brief  Temperature 1 measure
* @param  convVal - AD conversion value
* @retval Temperature value
* @note   None
\********************************************************/
signed int temp2_measure(u16 convVal)
{
    u32 i,sum,aver;	   
    s16 temp;
               
#if 1
    //-----------------------------------------------------    
    // The recursive average filtering algorithm method two  
    //-----------------------------------------------------
    valueBuf2[buf2In++] = convVal;
    if (buf2In == N2) buf2In = 0;
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        sum += valueBuf2[i];
    }
    
    aver = sum/N2; 
    // End
#endif    

#if 0    
    //-----------------------------------------------------     
    // The recursive average filtering algorithm method one
    //----------------------------------------------------- 
    // The collected data is placed in the highest position
    valueBuf2[N2] = aver;          
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        // All data left shift, low throw
        valueBuf2[i] = valueBuf2[i+1];    
        sum += valueBuf2[i];
    }
    
    aver = sum/N2;   
    // End
#endif       
        
    temp = linear_insert((u16)aver,NTCTAB);
    // Keep the 1 bit behind the decimal point,
    // For example: 255 = 25.5 degrees.
    return temp;
}
/********************************************************\
* @brief  Temperature 1 measure
* @param  convVal - AD conversion value
* @retval Temperature value
* @note   None
\********************************************************/
signed int temp3_measure(u16 convVal)
{
    u32 i,sum,aver;	   
    s16 temp;
               
#if 1
    //-----------------------------------------------------    
    // The recursive average filtering algorithm method two  
    //-----------------------------------------------------
    valueBuf3[buf3In++] = convVal;
    if (buf3In == N2) buf3In = 0;
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        sum += valueBuf3[i];
    }
    
    aver = sum/N2; 
    // End
#endif    

#if 0    
    //-----------------------------------------------------     
    // The recursive average filtering algorithm method one
    //----------------------------------------------------- 
    // The collected data is placed in the highest position
    valueBuf3[N2] = aver;          
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        // All data left shift, low throw
        valueBuf3[i] = valueBuf3[i+1];    
        sum += valueBuf3[i];
    }
    
    aver = sum/N2;   
    // End
#endif       
        
    temp = linear_insert((u16)aver,NTCTAB);
    // Keep the 1 bit behind the decimal point,
    // For example: 255 = 25.5 degrees.
    return temp;
}
/********************************************************\
* @brief  Temperature 1 measure
* @param  convVal - AD conversion value
* @retval Temperature value
* @note   None
\********************************************************/
signed int temp4_measure(u16 convVal)
{
    u32 i,sum,aver;	   
    s16 temp;
               
#if 1
    //-----------------------------------------------------    
    // The recursive average filtering algorithm method two  
    //-----------------------------------------------------
    valueBuf4[buf4In++] = convVal;
    if (buf4In == N2) buf4In = 0;
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        sum += valueBuf4[i];
    }
    
    aver = sum/N2; 
    // End
#endif    

#if 0    
    //-----------------------------------------------------     
    // The recursive average filtering algorithm method one
    //----------------------------------------------------- 
    // The collected data is placed in the highest position
    valueBuf4[N2] = aver;          
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        // All data left shift, low throw
        valueBuf4[i] = valueBuf4[i+1];    
        sum += valueBuf4[i];
    }
    
    aver = sum/N2;   
    // End
#endif       
        
    temp = linear_insert((u16)aver,NTCTAB);
    // Keep the 1 bit behind the decimal point,
    // For example: 255 = 25.5 degrees.
    return temp;
}
/********************************************************\
* @brief  Temperature 1 measure
* @param  convVal - AD conversion value
* @retval Temperature value
* @note   None
\********************************************************/
signed int temp5_measure(u16 convVal)
{
    u32 i,sum,aver;	   
    s16 temp;
               
#if 1
    //-----------------------------------------------------    
    // The recursive average filtering algorithm method two  
    //-----------------------------------------------------
    valueBuf5[buf5In++] = convVal;
    if (buf5In == N2) buf5In = 0;
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        sum += valueBuf5[i];
    }
    
    aver = sum/N2; 
    // End
#endif    

#if 0    
    //-----------------------------------------------------     
    // The recursive average filtering algorithm method one
    //----------------------------------------------------- 
    // The collected data is placed in the highest position
    valueBuf5[N2] = aver;          
    
    sum = 0;
    for (i = 0; i < N2; i++)
    {
        // All data left shift, low throw
        valueBuf5[i] = valueBuf5[i+1];    
        sum += valueBuf5[i];
    }
    
    aver = sum/N2;   
    // End
#endif       
        
    temp = linear_insert((u16)aver,NTCTAB);
    // Keep the 1 bit behind the decimal point,
    // For example: 255 = 25.5 degrees.
    return temp;
}