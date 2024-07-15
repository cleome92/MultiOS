#include "multiOS.h"
#include "device_driver.h"

UINT32 gaucAPP[MAX_APP_NUM];
UINT8 gucAppNum = 0;

void debugPrint(UINT8 x)
{
    Uart_Printf("%d\n", x);
}

void setAppNum(UINT8 num)
{
    gucAppNum = num;
}
UINT8 getAppNum()
{
    return gucAppNum;
}

