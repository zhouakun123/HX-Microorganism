#define TableSize       166
#define Data_Highest    NTCTAB[0]	
#define Data_Lowest     NTCTAB[TableSize-1]	

#define	ZERO_S	        -40     // Zero suppress
#define TEMP_MIN        -40
#define TEMP_MAX        125

signed int temp1_measure(u16 convVal);
signed int temp2_measure(u16 convVal);
signed int temp3_measure(u16 convVal);
signed int temp4_measure(u16 convVal);
signed int temp5_measure(u16 convVal);