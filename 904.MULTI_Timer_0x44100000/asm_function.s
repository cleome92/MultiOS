	.include "4412addr.inc"

	.code	32
	.text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Uart1_Printf(const char *fmt,...)
@ Uart console에 print
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart_Printf
SVC_Uart_Printf:
	SVC 0
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ int Uart1_GetIntNum(void)
@ Uart console에 scanf 역할 (int)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_setTimerStart
SVC_setTimerStart:
	SVC 15
	BX 	LR

	.global SVC_setTimerEnd
SVC_setTimerEnd:
	SVC 16
	BX 	LR

	.global SVC_getTimerStart
SVC_getTimerStart:
	SVC 17
	BX 	LR

	.global SVC_getTimerEnd
SVC_getTimerEnd:
	SVC 18
	BX 	LR

	.global SVC_getTimerMin
SVC_getTimerMin:
	SVC 19
	BX 	LR

	.global SVC_getTimerSec
SVC_getTimerSec:
	SVC 20
	BX 	LR

	.global SVC_getTimerMSec
SVC_getTimerMSec:
	SVC 21
	BX 	LR
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ int Uart1_GetIntNum(void)
@ Uart console에 scanf 역할 (int)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart1_Print_6_Paremeter
SVC_Uart1_Print_6_Paremeter:
	SVC 14
	BX 	LR
