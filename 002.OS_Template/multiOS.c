#include "multiOS.h"
#include "device_driver.h"

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

//void*(void) funcArr[] = {API_App0_Ready, API_App1_Ready};
//void (*API_Init[MAX_APP_NUM])(void);
void API_App0_Ready(void)
{
	Uart_Printf("\nAPP0 RUN 0\n");
	SetTransTable(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP0, RW_WBWA | (1<<17));
	SetTransTable(STACK_LIMIT_APP0, STACK_BASE_APP0-1, STACK_LIMIT_APP0, RW_WBWA | (1<<17));
	CoSetASID(1);
	setAppNum(NUM_APP0);
}
void API_App1_Ready(void)
{
	Uart_Printf("\nAPP1 RUN 1\n");
	SetTransTable(RAM_APP0, (RAM_APP0+SIZE_APP1-1), RAM_APP1, RW_WBWA | (1<<17));
	SetTransTable(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA | (1<<17));
	CoSetASID(2);
	setAppNum(NUM_APP1);
}

#if 0	//	юс╫ц
// Define a structure to hold the context of a task
typedef struct {
	UINT32 r0;
	UINT32 r1;
	UINT32 r2;
    UINT32 r3;
    UINT32 r4;
    UINT32 r5;
    UINT32 r6;
    UINT32 r7;
    UINT32 r8;
    UINT32 r9;
    UINT32 r10;
    UINT32 r11;
    UINT32 r12;
    UINT32 sp;    // Stack pointer (r13)
    UINT32 lr;    // Link register (r14)
    UINT32 pc;    // Program counter (r15)
    UINT32 cpsr;  // CPSR
} context_t;

// Function to perform context switch
void context_switch(context_t *current_task, context_t *next_task) {
    // Save the context of the current task
    asm volatile (
        // Save general-purpose registers r0-r12, sp, lr, pc, cpsr
        "stm %[current]!, {r0-r12, sp, lr, pc, cpsr} \n\t"
        : [current] "+r" (current_task)
        :
        : "memory"
    );

    // Load the context of the next task
    asm volatile (
        // Load general-purpose registers r0-r12, sp, lr, pc, cpsr
        "ldm %[next]!, {r0-r12, sp, lr, pc, cpsr} \n\t"
        : [next] "+r" (next_task)
        :
        : "memory"
    );
}
#endif
