#include "multiOS.h"
#include "device_driver.h"

UINT32 gaucAPP[MAX_APP_NUM];
UINT8 gucAppNum = 0;
struct T_Context gstRN[MAX_APP_NUM];
struct T_Context * gpaunContextAddress[MAX_APP_NUM];


//UINT32 TTBL[MAX_APP_NUM][0x400];

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
	setAppNum(NUM_APP0);
}
void API_App1_Ready(void)
{
	CoSetASID(2);
	CoSetTTBase(TTBL1_CACHE);
	setAppNum(NUM_APP1);
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
            pTT1 = (UINT32 *)TTBL0 + VA_1st_Idx + i;	// Set 1st Entry Info
            pTT2 = (UINT32 *)(TTBL0_PAGE + i*0x400);	//
        }
        else
        {
            pTT1 = (UINT32 *)TTBL1 + VA_1st_Idx + i;
            pTT2 = (UINT32 *)(TTBL1_PAGE + i*0x400);
        }

		*pTT1 = (UINT32)pTT2 | attr_1st;   	 				// Set 1st Descriptor Info
        CleanNInvalid((UINT32)pTT1);
        for (j=0; j<256; j++)
        {
            pTT1 = (UINT32*)pTT2 + j;						// Set 2nd Entry Info
            *pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);	// Set 2nd Descriptor Info
            CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
        }
	}
}
