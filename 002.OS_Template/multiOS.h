#ifndef __MULTI_OS__
#define __MULTI_OS__

#define MAX_APP_NUM (2)
#define NUM_APP0    (0) // app number, getAppNum()의 반환 값
#define NUM_APP1    (1) // app number
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

#define SECTION_SIZE	(0x4000)
#define TTBL0 		(0x44000000)
#define TTBL0_PAGE 	(TTBL0 + SECTION_SIZE)
#define TTBL1 		(0x44008000)
#define TTBL1_PAGE 	(TTBL1 + SECTION_SIZE)
#define TTBL0_CACHE (TTBL0 | (1<<6) | (1<<3) | (0<<1) | (0<<0))
#define TTBL1_CACHE (TTBL1 | (1<<6) | (1<<3) | (0<<1) | (0<<0))

#define page_entry_base 0x44B00000 // page entry 시작 주소 ~ 0x44B1F000 (32개)
#define page_entry_num 32 // page entry 개수

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

// 박도윤
// page entry 관련
typedef struct Page_Info{
    UINT32 PA;
    UINT32 VA;
    UINT32 App_num;
}Page_Info;

Page_Info page_entry_info[32]; // page에 관한 정보 담은 배열 ex) page_entry_info[0] : 0x44B00000 ~ 0x44B01000 관련 정보

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
void SetTransTable_MultiOS(unsigned int uVaStart, unsigned int uVaEnd, unsigned int uPaStart, unsigned int attr);
void SetTransTable_Page(UINT32 uVaStart, UINT32 uVaEnd, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd);

void set_Page_Info(UINT32 PA, UINT32 VA, UINT32 App_num, UINT32 idx);
void page_entry_init();
UINT32 load_page(UINT32 VA, int app_num);
UINT32 find_page_entry(UINT32 VA, UINT32 PA, int app_num); // page entry에서 찾기

extern void CleanNInvalid(UINT32 addr);
//void (*API_Init[])(void);
#endif
