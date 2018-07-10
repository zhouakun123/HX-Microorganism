#define BIT0                (0x00000001)
#define BIT1                (0x00000002)
#define BIT2                (0x00000004)
#define BIT3                (0x00000008)
#define BIT4                (0x00000010)
#define BIT5                (0x00000020)
#define BIT6                (0x00000040)
#define BIT7                (0x00000080)
#define BIT8                (0x00000100)
#define BIT9                (0x00000200)
#define BIT10               (0x00000400)
#define BIT11               (0x00000800)
#define BIT12               (0x00001000)
#define BIT13               (0x00002000)
#define BIT14               (0x00004000)
#define BIT15               (0x00008000)
#define BIT16               (0x00010000)
#define BIT17               (0x00020000)
#define BIT18               (0x00040000)
#define BIT19               (0x00080000)
#define BIT20               (0x00100000)
#define BIT21               (0x00200000)
#define BIT22               (0x00400000)
#define BIT23               (0x00800000)

// Bit operation
// Concrete realization method, Refer to <<CM3 authoritative guide>> Chapter 5 (87 Page ~ 92 Page)
// IO port operation macro define
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
// IO port address mapping
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) 		
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) 		
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) 		
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) 		
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) 		
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) 		
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) 		

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) 		
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) 		
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) 		
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) 		
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) 		
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) 		
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) 		
 
// IO port operation
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  	// Output
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  	// Input

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  	// Output
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  	// Input

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  	// Output
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  	// Input

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  	// Output
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  	// Input

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  	// Output
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  	// Input

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  	// Output
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  	// Input

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  	// Output
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  	// Input	

#define enable_int()    __set_PRIMASK(0)
#define disable_int()   __set_PRIMASK(1)

#define SOH		    0xff
#define EOT         0x0d

#define NULL        0	

