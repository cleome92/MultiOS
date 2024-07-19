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
	.global SVC_Uart1_GetIntNum
SVC_Uart1_GetIntNum:
	SVC 1
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ char Uart1_Get_Char(void)
@ Uart console에 scanf 역할 (char)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart1_Get_Char
SVC_Uart1_Get_Char:
	SVC 2
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void SVC_Lcd_Clr_Screen(void)
@ LCD clear
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Clr_Screen
SVC_Lcd_Clr_Screen:
	SVC 3
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void SVC_Lcd_Draw_BMP(int x, int y, const unsigned short int *fp)
@ LCD bitmap draw
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Draw_BMP
SVC_Lcd_Draw_BMP:
	SVC 4
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void SVC_Key_Wait_Key_Released(void);
@ Key Released Wait
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Key_Wait_Key_Released
SVC_Key_Wait_Key_Released:
	SVC 5
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void SVC_Key_Wait_Key_Pressed(void);
@ Key Pressed Wait
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Key_Wait_Key_Pressed
SVC_Key_Wait_Key_Pressed:
	SVC 6
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Lcd_Init(void);
@ Lcd Init
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Init
SVC_Lcd_Init:
	SVC 7
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Lcd_Win_Init(int id, int en);
@ Lcd Win Init
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Win_Init
SVC_Lcd_Win_Init:
	SVC 8
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Lcd_Brightness_Control(int level);
@ Lcd Brightness Control
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Brightness_Control
SVC_Lcd_Brightness_Control:
	SVC 9
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Key_Poll_Init(void);
@ Key Polling Init
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Key_Poll_Init
SVC_Key_Poll_Init:
	SVC 10
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ char Uart1_Get_Pressed(void)
@ Get pressed uart
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Uart1_Get_Pressed
SVC_Uart1_Get_Pressed:
	SVC 11
	BX 	LR


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Lcd_Select_Draw_Frame_Buffer(int win_id,int buf_num);
@ Key Polling Init
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Select_Draw_Frame_Buffer
SVC_Lcd_Select_Draw_Frame_Buffer:
	SVC 12
	BX 	LR

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void Lcd_Select_Display_Frame_Buffer(int win_id,int buf_num);
@ Key Polling Init
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global SVC_Lcd_Select_Display_Frame_Buffer
SVC_Lcd_Select_Display_Frame_Buffer:
	SVC 13
	BX 	LR


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int PABT_Falut_Status(void)
@ 명령어 Fault 발생 이유를 리턴
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global  PABT_Falut_Status
PABT_Falut_Status:

	mrc		p15, 0, r0, c5, c0, 1
	bx 		lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int PABT_Falut_Address(void)
@ 데이터 Fault 발생 주소를 리턴
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global  PABT_Falut_Address
PABT_Falut_Address:

	mrc		p15, 0, r0, c6, c0, 2
	bx 		lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int DABT_Falut_Status(void)
@ 데이터 Fault 발생 이유를 리턴
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global  DABT_Falut_Status
DABT_Falut_Status:

	mrc		p15, 0, r0, c5, c0, 0
	bx 		lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int DABT_Falut_Address(void)
@ 데이터 Fault 발생 주소를 리턴
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global  DABT_Falut_Address
DABT_Falut_Address:

	mrc		p15, 0, r0, c6, c0, 0
	bx 		lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int Get_SP(void)
@ 현재 모드의 스택 포인터 값을 리턴
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global Get_SP
Get_SP:

	mov 	r0, sp
	bx 		lr

	.end
