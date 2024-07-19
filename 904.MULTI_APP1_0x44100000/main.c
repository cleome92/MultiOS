#include "Device_Driver.h"

#define DELAY	8000
int a,b,c,d,e,f = 0;
int i = 0;
extern SVC_Uart1_Print_6_Paremeter(int, int, int, int, int ,int);
void Main(void)
{
	Uart_Printf(">>APP1 => UART Print, RO-BASE = 0x44100000 \n");
	for(;;)
	{
		a = 1;
		b = 2;
		c = 3;
		d = 4;
		e = 5;
		f = 6;
		
		for (i=0; i<100; i++)
		{
			SVC_Uart1_Print_6_Paremeter(a,b,c,d,e,f);
			a++;
			b++;
			c++;
			d++;
			e++;
			f++;
			Delay(DELAY);
		}
	}
}
