#include "Device_Driver.h"

int Main(void)
{
	Uart_Printf(">>Caculator APP\n");
	int op1, op2, operator;

	for(;;)
	{
		Uart_Printf("======================\n");
		Uart_Printf("Enter operator. (1 : add, 2 : sub, 3 : mul, 4 : div)>> ");
		operator = Uart1_GetIntNum();

		Uart_Printf("Enter two operands.>> ");
		op1 = Uart1_GetIntNum();
		op2 = Uart1_GetIntNum();

		Uart_Printf("Result >> ");
		if(operator == 1) Uart_Printf("%d\n", op1 + op2);
		else if(operator == 2) Uart_Printf("%d\n", op1 - op2);
		else if(operator == 3) Uart_Printf("%d\n", op1 * op2);
		else if(operator == 4)
		{
			if(op2 == 0)
			{
				Uart_Printf("Divisor shouldn't be 0.\n");
			}
			else Uart_Printf("%d\n", op1 / op2);
		}
		else Uart_Printf("Invalid operand.\n");
		Uart_Printf("\n");
	}
	return 0;
}
