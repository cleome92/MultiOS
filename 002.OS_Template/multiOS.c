#include "multiOS.h"
#include "device_driver.h"

//#define RW_WBWA_PAGE2_ACCESS	(1<<11 | 0x5<<6 | 0x3<< 4 | 1<<3 | 1<<1)
//#define RW_WBWA_PAGE2_NOACCESS	(1<<11 | 0x5<<6 | 0x3<< 4 | 1<<3 | 1<<1)
/*
1. APP Switiching 占쏙옙占싱곤옙 占쏙옙 占쌜몌옙 占쏙옙占�占쏙옙占쏙옙占신곤옙庸�Page 占쌓쏙옙트
2. VA to PA 占쌉쇽옙 占쌜쇽옙 占십울옙
3. printf占쏙옙 占쌩곤옙占쌩곤옙 setTansTable 占쏙옙占쏙옙 for loop 占쏙옙咀몌옙庸�占쏙옙占쏙옙占�占쌔븝옙占쏙옙
    setPageAccess(VA, PA)
    setPageNOAccess(VA, PA)
    占쌔댐옙 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 Access 占쏙옙占쏙옙 占싸울옙
    setPageReLocate(VA, PA)
    getPAadress(VA)
*/
#define MAX_PAGE_NUM    (32)
#define SET             (1)
#define CLR             (0)

UINT32 pageInfo[MAX_PAGE_NUM][3];
Page_Info page_entry_info[32]; // page占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占썼열 ex) page_entry_info[0] : 0x44B00000 ~ 0x44B01000 占쏙옙占쏙옙 占쏙옙占쏙옙


int flag = 0;

// VA -> PA 占쏙옙칭占싹댐옙 占쌉쇽옙
// attr_1st : 0 ~ 9占쏙옙 bit
// attr_2nd : 0 ~ 11占쏙옙 bit
// uPaStart : 1占쏙옙 占쏙옙占싱븝옙 占쏙옙占쏙옙 占쏙옙치
// uPaStart2 : 2占쏙옙 占쏙옙占싱븝옙 占쏙옙占쏙옙 占쏙옙치 (占싱곤옙 占식띰옙占쏙옙占�占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙柰占쏙옙占�
// paging 占쏙옙占쏙옙 : 0x44B0000 ~ 0x44B20000 4KB 32占쏙옙???


#define PAGE_TABLE_SIZE 4096
//#define SECTION_SIZE 0x100000
#define PAGE_SIZE 4096
#define NUM_L2_ENTRIES 256


#define TTBR0 0xFFFFC000  // TTBR0占쏙옙 占쏙옙占쏙옙 占쌍쇽옙
#define L1_INDEX_MASK (0xFFF00000)
#define L2_INDEX_MASK (0x000FF000)
#define PAGE_OFFSET_MASK 	(0x00000FFF)
#define L1_SHIFT (20)
#define L2_SHIFT (12)


// page 占쏙옙占쏙옙占싹댐옙 占쌉쇽옙
    /*
    1. VA -> PA 占쏙옙환 (占쌉쇽옙 占쏙옙占�
    2. 占쏙옙占쏙옙 00占싱띰옙占�-> page entry占쏙옙 占쏙옙占쏙옙占쌔억옙 占쏙옙 (memcpy 占싱울옙)
        1) page占쏙옙 RR占쏙옙 占쏙옙占쏙옙
        2) src占쏙옙 占쌍쇽옙 占쏙옙占쏙옙求占�占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙 -> 32占쏙옙占싹깍옙 32占쏙옙 PA 占썼열 占쏙옙占쏙옙占쏙옙 (UINT32)
        3) 占쏙옙占쏙옙 占쏙옙 찼占쌕몌옙 -> 占쏘떤 占신몌옙 占쏙옙占쏙옙 占쌕쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙? (占싱곤옙 占쏙옙占쏙옙 占싹댐옙 占쏙옙 X)
        (+) 占쏙옙체占싹곤옙 占쏙옙占쏙옙 page占쏙옙 占쌈쇽옙 占쌕쏙옙 00占쏙옙占쏙옙 占쌕뀐옙占�占쏙옙
    3. 01占싱몌옙 page entry占쏙옙占쏙옙 찾占싣쇽옙 占쏙옙占�
        1) 32占쏙옙占싹깍옙 占쌓놂옙 占쏙옙체 占쏙옙 찾占쏙옙 (loop)
    */
// src 占쌍쇽옙 占쏙옙占�-> OS占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쌔쇽옙 占쏙옙占쏙옙占싹깍옙 src 占쌍쇽옙 (PA, NA), 占쏙옙占�app占쏙옙占쏙옙

UINT32 VA_2_PA(UINT32 VA) // VA 占쏙옙占쌘뤄옙 占쌨아쇽옙 PA 占쏙옙占쏙옙占싹댐옙 占쌉쇽옙
{
	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));
	PA = (*pTT2 & ~PAGE_OFFSET_MASK) | VA_Page_Idx;
	Uart_Printf("[VA_2_PA] VA[%0.8X], PA[%0.8X]\n", VA, PA);
	return PA;
}
UINT32* VA_2_pTT2(UINT32 VA) // VA 占쏙옙占쌘뤄옙 占쌨아쇽옙 PA 占쏙옙占쏙옙占싹댐옙 占쌉쇽옙
{
//	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
//	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
//	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));
//	PA = (*pTT2 & ~PAGE_OFFSET_MASK) | VA_Page_Idx;
//	Uart_Printf("[VA_2_PA] VA[%0.8X], PA[%0.8X]\n", VA, PA);
	return pTT2;
}

// option占쏙옙 set/clr占쏙옙 占쏙옙占쏙옙占싹울옙 占쌔댐옙 VA占쏙옙 占쏙옙占쏙옙占쏙옙 占싸울옙占싹거놂옙 占쏙옙占쏙옙占싼댐옙.
void set_VA_Access(UINT32 VA, UINT32 option) // option占쏙옙 1占싱몌옙 10, 0占싱몌옙 00占쏙옙占쏙옙
{
//	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
//	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
//	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));


	(*pTT2) &= ~0x3;		// [0:1] BIT Clear
    if (option == SET)
	{
		(*pTT2) |= 0x2;	// Access Approve
	}
	else if (option == CLR)
	{
		(*pTT2) |= 0x0;		// Access Deny
	}
//    Uart_Printf("set_VA_Access [%0.8X] -> [%0.8X]\n", VA, *pTT2);

}
UINT32 get_VA_Access(UINT32 VA) // 占싱곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 1 return
{	//占쏙옙占쌕깍옙占쏙옙 占쏙옙占쏙옙 占쏙옙占�
	Uart_Printf("[get_VA_Access]\n");

//    UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
//	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
//	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		Uart_Printf("[get_VA_Access] App0\n");
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		Uart_Printf("[get_VA_Access] App1\n");
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));

    UINT32 tmp_mask = 0x3;

    // 00 占싱몌옙 0 return
    if((tmp_mask & *pTT2) == 0)
    {
    	Uart_Printf("[get_VA_Access] Access is denied..\n");
    	return 0;
    }
    // 10 占싱몌옙 1 return
    else if((tmp_mask & *pTT2) == 0x2)
    {
    	Uart_Printf("[get_VA_Access] Access is permitted..\n");
    	return 1;
    }

    Uart_Printf("[get_VA_Access] Error..\n");
	return -1; // 占싱거몌옙 error

}

void set_Page_Info(UINT32 PA, UINT32 VA, UINT32 App_num, UINT32 idx) // page_entry_info update
{
    page_entry_info[idx].PA = PA;
    page_entry_info[idx].VA = VA;
    page_entry_info[idx].App_num = App_num; // 占쎄가 2占쏙옙 占쏙옙占쏙옙獵占�占쏙옙占쏙옙
    Uart_Printf("[set_Page_Info] PA[%0.8X] VA[%0.8X] App_num[%d]\n", page_entry_info[idx].PA, page_entry_info[idx].VA, page_entry_info[idx].App_num);
}

int victim = 0;
UINT32 org[256];			// HDD에 되돌릴 주소
UINT32 * table_2nd[256];	//
UINT32 org_mode[256];		// 0 : DABT 1: PABT
UINT32 org_va[256];
UINT32 load_page(UINT32 VA, int app_num, int mode) // parameter - app_num : 현재 app 번호, mode : 현재 abort 종류
{
	UINT32 src = ((app_num == 0) ? 0x44100000 : 0x44500000) + ((VA & ~0xFFF) - 0x30000000); // App PA 계산
	UINT32 dst = 0x44b00000 + (victim * 0x1000); // 페이지 엔트리에 해당되는 PA 계산

	if (org[victim]){ // 페이지 테이블 엔트리를 저장한 RAM이 가득찬 경우
		*(table_2nd[victim]) = 0;
		CoInvalidateMainTlbVA(org_va[victim]);
		if (org_mode[victim] == 0){
			memcpy((UINT32*)org[victim], (UINT32*)dst, 0x1000);
		}
	}

	org_mode[victim] = mode; // 0 : DABT 1: PABT
	org[victim] = src; // App PA
	org_va[victim] = (VA & ~0xFFF) | (app_num+1); // VA 저장
	table_2nd[victim] = VA_2_pTT2(VA); // 2차 페이지 테이블 엔트리 주소 저장
	//VA에 해당하는 2차 TT 수정 (dst로 접근하도록 수정) 캐쉬정책 inner WT, outter WBWA, nG
	SetTransTable_SinlgePage(VA, dst, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
	memcpy((UINT32*)dst, (UINT32*)src, 0x1000); // App PA를 페이지 테이블 엔트리 저장 RAM에 로드
	CoInvalidateMainTlbVA((VA & ~0xFFF) | (app_num+1));
	CoInvalidateICache();
	victim = (victim + 1) % page_entry_num; // 페이지 엔트리 번호 업데이트
	return 0;
}





void SetTransTable_MultiOS(unsigned int uVaStart, unsigned int uVaEnd, unsigned int uPaStart, unsigned int attr)
{
	int i;
	unsigned int* pTT = 0x0;
	unsigned int nNumOfSec;

	uPaStart &= ~0xfffff;
	uVaStart &= ~0xfffff;
	if (getAppNum() == NUM_APP0)
	{
		pTT = (unsigned int *)TTBL0+(uVaStart>>20);
	}
	else if (getAppNum() == NUM_APP1)
	{
		pTT = (unsigned int *)TTBL1+(uVaStart>>20);
	}
	else
	{
		;
	}
	if (pTT != 0x0)
	{
		nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;
		for(i=0; i<=nNumOfSec; i++)
		{
			*pTT++ = attr|(uPaStart+(i<<20));
		}
	}
}

// 吏꾩꽦 異붽�
void SetTransTable_MultiOS1(unsigned int uVaStart, unsigned int uVaEnd, unsigned int uPaStart, unsigned int attr)
{
	int i;
	unsigned int* pTT = 0x0;
	unsigned int nNumOfSec;
	unsigned int* spTT = 0;

	uPaStart &= ~0xfffff;
	uVaStart &= ~0xfffff;
	if (getAppNum() == NUM_APP0)
	{
		pTT = (unsigned int *)TTBL0+(uVaStart>>20);
		spTT = (unsigned int *)TTBL0;
	}
	else if (getAppNum() == NUM_APP1)
	{
		pTT = (unsigned int *)TTBL1+(uVaStart>>20);
		spTT = (unsigned int *)TTBL1;
	}
	else
	{
		;
	}
	if (pTT != 0x0)
	{
		nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;
		for(i=0; i<=nNumOfSec; i++)
		{
			//Uart_Printf("BEFORE : %d\n",i);
			*pTT++ = attr|(uPaStart+(i<<20));
			//Uart_Printf("AFTER : %d\n",i);
		}
	}

	for(i=0;i<4*1024*1024 ;i++)
	{
		*(spTT++) = 0x0;
	}
}

UINT32 gaucAPP[MAX_APP_NUM];
UINT8 gucAppNum = 0;
struct T_Context gstRN[MAX_APP_NUM];
struct T_Context * gpaunContextAddress[MAX_APP_NUM];

void debugPrint(UINT8 x)
{
    Uart_Printf("========Flag : %d=========\n", x);
    UINT8 i = 0;
    for (i = 0; i < 18; i++)
    {
        Uart_Printf("R[%.2d] : %.8X    %.8X\n", i, gstRN[0].RN[i], gstRN[1].RN[i]);
    }

}
void debugPrintNum(UINT32 x)
{
//    Uart_Printf("%.8X  =========\n", x);
}

void debugPrintNum1(UINT32 x)
{
    Uart_Printf("%.8X  =========\n", x);
}

void setAppNum(UINT8 num)
{
    gucAppNum = num;
}
UINT8 getAppNum()
{
    return gucAppNum;
}
UINT8 getNextAppNum()
{
    return (gucAppNum + 1) % MAX_APP_NUM;
}


void API_App0_Ready(void)
{
   CoSetASID(1);
   CoSetTTBase(TTBL0_CACHE);
   CoInvalidateMainTlb();
   setAppNum(NUM_APP0);
   CoInvalidateICache();
}
void API_App1_Ready(void)
{
   CoSetASID(2);
   CoSetTTBase(TTBL1_CACHE);
   CoInvalidateMainTlb();
   setAppNum(NUM_APP1);
   CoInvalidateICache();
}

void SetTransTable_Page(UINT32 uVaStart, UINT32 uVaEnd, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
   UINT32 i,j;
   UINT32* pTT1;
    UINT32* pTT2;

    UINT32 PA_1st_Idx;
    UINT32 VA_1st_Idx;

//    UINT32 PA_2st_Idx;
//    UINT32 VA_2st_Idx;

    UINT32 nNumOfSec;

   PA_1st_Idx = (uPaStart & ~0xfffff) >> 20;
   VA_1st_Idx = (uVaStart & ~0xfffff) >> 20;

   nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;

   for(i=0; i<=nNumOfSec; i++)
   {
        if (getAppNum() == NUM_APP0)
        {
            pTT1 = (UINT32 *)TTBL0 + VA_1st_Idx + i;   // Set 1st Entry Info
            pTT2 = (UINT32 *)(TTBL0_PAGE + i*0x400);   //
        }
        else
        {
            pTT1 = (UINT32 *)TTBL1 + VA_1st_Idx + i;
            pTT2 = (UINT32 *)(TTBL1_PAGE + i*0x400);
        }

      *pTT1 = (UINT32)pTT2 | attr_1st;                   // Set 1st Descriptor Info
        CleanNInvalid((UINT32)pTT1);
        for (j=0; j<256; j++)
        {
            pTT1 = (UINT32*)pTT2 + j;                  // Set 2nd Entry Info
            *pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);   // Set 2nd Descriptor Info
            CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
        }
   }
}

// Va : 3000 0000
void SetTransTable_SinlgePage(UINT32 uVaStart, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
//	int i = 0;
   UINT32* pTT1;
   UINT32* pTT2;
//   UINT32* sPTT;

    UINT32 PA_1st_Idx = 0;
   UINT32 PA_2nd_Idx = 0;
    UINT32 VA_1st_Idx = 0;
    UINT32 VA_2nd_Idx = 0;
    UINT32 offsetpIdx = (VARAM & L1_INDEX_MASK) >> L1_SHIFT;
   VA_1st_Idx = (uVaStart & L1_INDEX_MASK) >> L1_SHIFT;
   PA_1st_Idx = (uPaStart & L1_INDEX_MASK) >> L1_SHIFT;
   VA_2nd_Idx = (uVaStart & L2_INDEX_MASK) >> L2_SHIFT;
   PA_2nd_Idx = (uPaStart & L2_INDEX_MASK) >> L2_SHIFT;
   if (flag == 1) debugPrintNum(9);
   if (getAppNum() == NUM_APP0)
   {
      pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));                        // Set 1st Entry Info
      *pTT1 = (TTBL0_PAGE + (VA_1st_Idx - offsetpIdx) * 1024) | attr_1st;      // Set 1st Descriptor Info
   }
   else
   {
      pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2)) ;                     // Set 1st Entry Info
      *pTT1 = (TTBL1_PAGE + (VA_1st_Idx - offsetpIdx) * 1024) | attr_1st;      // Set 1st Descriptor Info
   }

   if (flag == 1) debugPrintNum(10);
   pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
   pTT2 = (UINT32*)(pTT2 + VA_2nd_Idx);                        // Set 2nd Entry Info
   *pTT2 = ((PA_1st_Idx<<L1_SHIFT) | (PA_2nd_Idx<<L2_SHIFT) | attr_2nd);   // Set 2nd Descriptor Info

   if (flag == 1) debugPrintNum(11);

//   if(flag)
//   {
//	   for(i=0 ;i< 1024*4;i++)
//	   {
//		   Uart_Printf("Before SinlgePage[[%0.8X]] : \n", *pTT2);
//		   (*pTT2) |= 0x2;
//		   Uart_Printf("After SinlgePage[[%0.8X]] : \n", *pTT2);
//		   pTT2++;
//	   }
//
//   }
////   if (((UINT32)pTT2 & 0xFFF) == 0)
//   {
////      Uart_Printf("APP : %d  original VA = %0.8X  PA = %0.8X computed PA = %0.8X, Permission PA = %0.8X\n", getAppNum(), uVaStart, uPaStart, VA_2_PA(uVaStart), VA_2_PrintPermission(uVaStart));
////      Uart_Printf("APP : %d   pTT1 = %0.8X   *pTT1 = %0.8X   pTT2 = %0.8X   *pTT2 = %0.8X\n", getAppNum(), pTT1, *pTT1, pTT2, *pTT2);
//   }
}
