#define T_1MS			1
#define T_10MS			10
#define T_30MS			30
#define T_60MS			60
#define T_80MS			80
#define T_100MS         100
#define T_120MS         120
#define T_300MS         300
#define T_500MS         500
#define T_1S			1000
#define T_2S			2000
#define T_3S            3000
#define T_5S            5000
#define T_10S           10000
#define T_20S           20000
#define T_30S           T_1S*30
#define T_120S          T_1S*120
#define T_1MIN          T_1S*60
#define T_20MIN         T_1MIN*20

typedef struct {  
    u8 ftick10ms;                       
    u8 ftick500ms;               			
    u8 ftick1s;    					
    u8 f10msI; 
    u8 f1minI;     
    u8 f1hourI;      
}T1_FLAG;

extern u32 tick1ms;
extern u32 tick10ms;
extern u32 tick500ms;
extern T1_FLAG t1_flag;     

void time_base(void);
void timer_config(void);
u32 timerGetTicks(void);
u32 timer_get_delay(u32 TicksHome, u32 TicksDly);
