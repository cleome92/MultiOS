#include "Device_Driver.h"
#define DELAY	6000
int i = 0;
//extern void SVC_Uart1_Print_6_Paremeter(int, int, int, int, int ,int);
extern void SVC_Uart_Printf(const char *fmt,...);
//extern void SVC_setTimerStart(unsigned char op);
extern void SVC_setTimerEnd(unsigned char op);
extern unsigned char SVC_getTimerStart(void);
extern unsigned char SVC_getTimerEnd(void);
extern unsigned char SVC_getTimerMin(void);
extern unsigned char SVC_getTimerSec(void);
//extern unsigned int SVC_getTimerMSec(void);

void makeVoidSpace(void)
{
	SVC_Uart_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	SVC_Uart_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	SVC_Uart_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void Main(void)
{
//	Uart_Printf(">>APP1 => UART Print, RO-BASE = 0x44100000 \n");
	unsigned char min = 0;
	unsigned char sec = 0;
	unsigned char OSmin = 0;
	unsigned char OSsec = 0;
	unsigned char recordMin = 0;
	unsigned char recordSsec = 0;
	unsigned char gameStart = 0;
	unsigned int OStotalTime = 0;
	unsigned int LocaltotalTime = 0;
	for(;;)
	{
		OSmin = SVC_getTimerMin();
		OSsec = SVC_getTimerSec();
		LocaltotalTime = OSmin*60 + OSsec;
		Delay(DELAY);

		if (SVC_getTimerStart() == 1 && (gameStart == 0 || gameStart == 2))
		{	// Game Start
			gameStart = 1;
			OStotalTime = OSmin*60 + OSsec;
		}

		if (SVC_getTimerEnd() == 1)
		{	// Game End
			SVC_setTimerEnd(0);
			gameStart = 2;
			min = 0;
			sec = 0;
		}

		if (gameStart == 1)
		{	// Game Start Print

			makeVoidSpace();
			min = (LocaltotalTime - OStotalTime)/60;
			sec = (LocaltotalTime - OStotalTime)%60;
			recordMin = min;
			recordSsec = sec;
			SVC_Uart_Printf("   %02d : %02d\n\n\n\n", min, sec);

		}
		else if (gameStart == 2)
		{	// Game Record Print

			makeVoidSpace();
			SVC_Uart_Printf("Record   %02d : %02d\n\n\n\n", recordMin, recordSsec);
		}
		else
		{	// Game Wait
			recordMin = 0;
			recordSsec = 0;
			makeVoidSpace();
			SVC_Uart_Printf("   %02d : %02d\n\n\n\n", recordMin, recordSsec);
		}
	}
}
//		SVC_Uart_Printf("%02d:%02d:%03d\n\n\n\n", min, sec, msec);
