#include "multiOS.h"
#include "device_driver.h"

UINT32 gaucAPP[MAX_APP_NUM];
UINT8 gucAppNum = 0;
struct T_Context gstRN[MAX_APP_NUM];

void debugPrint(UINT8 x)
{
    Uart_Printf("========Flag : %d=========\n", x);
    UINT8 i = 0;
    for (i = 0; i < 16; i++)
    {
        Uart_Printf("R[%.2d] : %.8X    %.8X\n", i, gstRN[0].RN[i], gstRN[1].RN[i]);
    }

}

void setAppNum(UINT8 num)
{
    gucAppNum = num;
}
UINT8 getAppNum()
{
    return gucAppNum;
}

void saveCurrentContext(UINT8 ucCurrentAppNum)
{
    saveContext(&gstRN[ucCurrentAppNum]);
}

void loadNextContext(UINT8 ucNextAppNum)
{
    loadContext(&gstRN[ucNextAppNum]);
}

void jump2App(UINT8 ucAppNum)
{
    switch(ucAppNum)
    {
        case 0:
			SetTransTable(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP0, RW_WBWA);
			SetTransTable(STACK_LIMIT_APP0, STACK_BASE_APP1-1, STACK_LIMIT_APP0, RW_WBWA);
			CoInvalidateMainTlb();
			setAppNum(NUM_APP0);
            Jump_App(RAM_APP0, STACK_BASE_APP0);
            break;
        case 1:
            SetTransTable(RAM_APP0, (RAM_APP0+SIZE_APP1-1), RAM_APP1, RW_WBWA);
			SetTransTable(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA);
			CoInvalidateMainTlb();
			setAppNum(NUM_APP1);
            Jump_App(RAM_APP0, STACK_BASE_APP1);
            break;
        default:
            Uart_Printf("\nError Occur!\n");
            break;

    }
}

void API_App_Change(void)
{
    UINT8 ucCurrentAppNum = getAppNum();
    UINT8 ucNextAppNum = (ucCurrentAppNum + 1) % MAX_APP_NUM;

    debugPrint(0);

    saveCurrentContext(ucCurrentAppNum); // save

    debugPrint(1);

    setAppNum(ucNextAppNum);

    loadNextContext(ucNextAppNum);		// load

    debugPrint(2);
}

