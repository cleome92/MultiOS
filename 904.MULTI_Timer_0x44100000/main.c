#include "Device_Driver.h"
#define DELAY	100
#define PDELAY	1000
int i = 0;
//extern void SVC_Uart1_Print_6_Paremeter(int, int, int, int, int ,int);
extern void SVC_Uart_Printf(const char *fmt,...);
//extern void SVC_setTimerStart(unsigned char op);
extern void SVC_setTimerEnd(unsigned char op);
extern unsigned char SVC_getTimerStart(void);
extern unsigned char SVC_getTimerEnd(void);
extern unsigned char SVC_getTimerMin(void);
extern unsigned char SVC_getTimerSec(void);

void print_time_ascii(int minute, int second);
//extern unsigned int SVC_getTimerMSec(void);

void makeVoidSpace(void)
{
	SVC_Uart_Printf("\033[H\033[J");
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
	unsigned int count = 0;
	for(;;)
	{
		count++;
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

		if (count > 5000)
		{
			count = 0;
			if (gameStart == 1)
			{	// Game Start Print
				makeVoidSpace();
				min = (LocaltotalTime - OStotalTime)/60;
				sec = (LocaltotalTime - OStotalTime)%60;
				recordMin = min;
				recordSsec = sec;
				SVC_Uart_Printf("\n\n\n\n\n\n\n\n");
				SVC_Uart_Printf(" _____                                   _    \n");
				SVC_Uart_Printf("|  ___|                                 | |   \n");
				SVC_Uart_Printf("| |__   ___   ___   __ _  _ __    ___   | |   \n");
				SVC_Uart_Printf("|  __| / __| / __| / _` || '_ \\  / _ \\  | |   \n");
				SVC_Uart_Printf("| |___ \\__ \\| (__ | (_| || |_) ||  __/  |_|   \n");
				SVC_Uart_Printf("\\____/ |___/ \\___| \\__,_|| .__/  \\___|  (_)   \n");
				SVC_Uart_Printf("                         | |                  \n");
				SVC_Uart_Printf("                         |_|         \n");
//				SVC_Uart_Printf("   %02d : %02d\n\n\n\n", min, sec);
				SVC_Uart_Printf("\n\n");
				print_time_ascii(min, sec);
			}
			else if (gameStart == 2)
			{	// Game Record Print

				makeVoidSpace();
				SVC_Uart_Printf("\n\n\n\n\n\n\n\n");
				SVC_Uart_Printf("______  _____  _____  _____ ______ ______    \n");
				SVC_Uart_Printf("| ___ \\|  ___|/  __ \\|  _  || ___ \\|  _  \\   \n");
				SVC_Uart_Printf("| |_/ /| |__  | /  \\/| | | || |_/ /| | | |   \n");
				SVC_Uart_Printf("|    / |  __| | |    | | | ||    / | | | |   \n");
				SVC_Uart_Printf("| |\\ \\ | |___ | \\__/\\\\ \\_/ /| |\\ \\ | |/ /    \n");
				SVC_Uart_Printf("\\_| \\_|\\____/  \\____/ \\___/ \\_| \\_||___/     \n");

//				SVC_Uart_Printf("Record   %02d : %02d\n\n\n\n", recordMin, recordSsec);
				SVC_Uart_Printf("\n\n");
				print_time_ascii(recordMin, recordSsec);
			}
			else
			{	// Game Wait
				makeVoidSpace();
				SVC_Uart_Printf("\n\n\n\n\n\n\n\n");
				SVC_Uart_Printf("______                            ______         _    _   \n");
				SVC_Uart_Printf("| ___ \\                           | ___ \\       | |  | |   \n");
				SVC_Uart_Printf("| |_/ / _ __   ___  ___  ___      | |_/ / _   _ | |_ | |_   ___   _ __   \n");
				SVC_Uart_Printf("|  __/ | '__| / _ \\/ __|/ __|     | ___ \\| | | || __|| __| / _ \\ | '_ \\   \n");
				SVC_Uart_Printf("| |    | |   |  __/\\__ \\\\__ \\     | |_/ /| |_| || |_ | |_ | (_) || | | |   \n");
				SVC_Uart_Printf("\\_|    |_|    \\___||___/|___/     \\____/  \\__,_| \\__| \\__| \\___/ |_| |_|   \n");
				SVC_Uart_Printf("\n\n");
				recordMin = 0;
				recordSsec = 0;
				SVC_Uart_Printf("\n");
			}
		}
	}
}

const char *asciiTable[11][7] =
{
	   {
			"  ####   ",
			" #    #  ",
			" #   ##  ",
			" # ## #  ",
			" ##   #  ",
			" #    #  ",
			"  ####   ",
		},
		{
			"    #    ",
			"   ##    ",
			"  # #    ",
			"    #    ",
			"    #    ",
			"    #    ",
			"  #####  ",
		},
		{
			"  ####   ",
			" #    #  ",
			"      #  ",
			"    ##   ",
			"  ##     ",
			" #       ",
			" ######  ",
		},
		{
			"  ####   ",
			" #    #  ",
			"      #  ",
			"   ###   ",
			"      #  ",
			" #    #  ",
			"  ####   ",
		},
		{
			"     #   ",
			"    ##   ",
			"   # #   ",
			"  #  #   ",
			" ######  ",
			"     #   ",
			"     #   ",
		},
		{
			" ######  ",
			" #       ",
			" ####    ",
			"     #   ",
			"      #  ",
			" #   #   ",
			"  ###    ",
		},
		{
			"   ###   ",
			"  #      ",
			" #       ",
			" #####   ",
			" #    #  ",
			" #    #  ",
			"  ####   ",
		},
		{
			" ######  ",
			" #    #  ",
			"     #   ",
			"    #    ",
			"   #     ",
			"   #     ",
			"   #     ",
		},
		{
			"  ####   ",
			" #    #  ",
			" #    #  ",
			"  ####   ",
			" #    #  ",
			" #    #  ",
			"  ####   ",
		},
		{
			"  ####   ",
			" #    #  ",
			" #    #  ",
			"  #####  ",
			"      #  ",
			"     #   ",
			"  ###    ",
		},
		{
			"         ",
			"         ",
			"    #    ",
			"         ",
			"         ",
			"    #    ",
			"         ",
		},
};
void print_time_ascii(int minute, int second) {
	int row= 0;
	int i = 0;
	int digits_to_print[5] = {
        minute / 10, minute % 10,
        10, // 콜론 자리
        second / 10, second % 10
    };

    for (row = 0; row < 7; ++row) {
        for ( i = 0; i < 5; ++i) {
        	SVC_Uart_Printf("%s", asciiTable[digits_to_print[i]][row]);
            if (i < 4) {
            	SVC_Uart_Printf(" "); // 숫자 간의 간격을 위한 공백
            }
        }
        SVC_Uart_Printf("\n");
    }
}
