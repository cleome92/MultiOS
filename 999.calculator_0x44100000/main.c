#include "Device_Driver.h"

extern void SVC_Uart_Printf(const char *fmt,...);
extern int SVC_Uart1_GetIntNum(void);
extern char SVC_Uart1_Get_Pressed(void);

#define NOINPUT ((char)(0xFF))
int Main(void)
{
	// calculator app
	// SVC handler 이용하여 다음 system call 호출
	// 1. Uart_Printf() void Uart1_Printf(const char *fmt,...) -> 0번
	// 2. Uart1_GetIntNum() -> 1번

	SVC_Uart_Printf(">>Caculator APP\n");
	int op1 = NOINPUT, op2 = NOINPUT, operator = NOINPUT;

	for(;;)
	{
		SVC_Uart_Printf("======================\n");
		SVC_Uart_Printf("Enter operator. (1 : add, 2 : sub, 3 : mul, 4 : div)>> ");

#if 0
		while (operator == NOINPUT)
		{
			operator = SVC_Uart1_Get_Pressed();
		}

		SVC_Uart_Printf("Enter two operands.>> ");
		while (op1 == NOINPUT)
		{
			op1 = SVC_Uart1_Get_Pressed();
		}
		while (op2 == NOINPUT)
		{
			op2 = SVC_Uart1_Get_Pressed();
		}
#endif
#if 1
		operator = Uart1_GetIntNum();
		SVC_Uart_Printf("Enter two operands.>> ");
		op1 = Uart1_GetIntNum();
		op2 = Uart1_GetIntNum();
#endif
		SVC_Uart_Printf("Result >> ");
		if(operator == 1)SVC_Uart_Printf("%d\n", op1 + op2);
		else if(operator == 2) SVC_Uart_Printf("%d\n", op1 - op2);
		else if(operator == 3) SVC_Uart_Printf("%d\n", op1 * op2);
		else if(operator == 4)
		{
			if(op2 == 0)
			{
				SVC_Uart_Printf("Divisor shouldn't be 0.\n");
			}
			else SVC_Uart_Printf("%d\n", op1 / op2);
		}
		else SVC_Uart_Printf("Invalid operand.\n");
		SVC_Uart_Printf("\n");
#if 0
		operator = NOINPUT;
		op1 = NOINPUT;
		op2 = NOINPUT;
#endif
	}

	return 0;
}
