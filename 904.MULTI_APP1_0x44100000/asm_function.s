	.include "4412addr.inc"

	.code	32
	.text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Uart1_Printf(const char *fmt,...)
@ Uart console�� print
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart_Printf
SVC_Uart_Printf:
	SVC 0
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ int Uart1_GetIntNum(void)
@ Uart console�� scanf ���� (int)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart1_GetIntNum
SVC_Uart1_GetIntNum:
	SVC 1
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ int Uart1_GetIntNum(void)
@ Uart console�� scanf ���� (int)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart1_Print_6_Paremeter
SVC_Uart1_Print_6_Paremeter:
	SVC 13
	BX 	LR
