#ifndef __MULTI_OS__
#define __MULTI_OS__

#define MAX_APP_NUM (2)
#define NUM_APP0    (0)
#define NUM_APP1    (1)
#define NUM_NONAPP  (0xFF)

#define 	RAM_APP0			0x44100000                      //
#define 	RAM_APP1			(RAM_APP0+SIZE_APP0)            //
#define 	SIZE_STACK0			(1*1024*1024)
#define 	SIZE_STACK1			(1*1024*1024)
#define 	STACK_LIMIT_APP0	(RAM_APP1+SIZE_APP1)
#define 	STACK_LIMIT_APP1	(STACK_LIMIT_APP0+SIZE_STACK0)
#define 	STACK_BASE_APP0		(STACK_LIMIT_APP0+SIZE_STACK0)
#define 	STACK_BASE_APP1		(STACK_LIMIT_APP1+SIZE_STACK1)

#define 	SIZE_APP0			(4*1024*1024)
#define 	SIZE_APP1			(4*1024*1024)

#define 	SECTOR_APP0			100
#define 	SECTOR_APP1			5000

#define SECTOR_SIZE 		512
#define ALIGN_SECTOR(x)	 	((((x+(SECTOR_SIZE-1))&~(SECTOR_SIZE-1))/SECTOR_SIZE))
typedef unsigned char UINT8;
typedef unsigned int UINT32;

enum Resister
{
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10 = 10,
    R11,
    R12,
    SP,
    LR,
    PC,
    CPSR,
    LR_IRQ
};

struct T_Context
{
    UINT32 RN[18];
};

extern struct T_Context gstRN[];
extern struct T_Context * gpaunContextAddress[];
extern UINT32 Get_User_SP(void);
extern UINT32 VA_2_PA(UINT32);
extern UINT8 getAppNum(void);
extern void setAppNum(UINT8 num);
extern void debugPrint(UINT8 x);
extern void debugPrintNum(UINT32 x);
void API_App0_Ready(void);
void API_App1_Ready(void);
//void (*API_Init[])(void);
#endif
