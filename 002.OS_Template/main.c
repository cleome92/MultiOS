#include "device_driver.h"
#include "multiOS.h"

extern WIN_INFO_ST ArrWinInfo[5];

#define BLACK	0x0000
#define WHITE	0xffff
#define BLUE	0x001f
#define GREEN	0x07e0
#define RED		0xf800
#define YELLOW	0xffe0
#define VIOLET	0xf81f


#define DISABLE (0)
#define ENABLE 	(1)

void App_Read(unsigned int sector, unsigned int size, unsigned int addr)
{
	int i;

	for(i=sector; i<(sector+ALIGN_SECTOR(size)); i++)
	{
		SD_Read_Sector(i, 1,(void *)addr);
		addr += SECTOR_SIZE;
	}
}

void Main(void)
{
	CoInitMmuAndL1L2Cache();
	Uart_Init(115200);
	LED_Init();
	Key_Poll_Init();
	Key_ISR_Init();

	Uart_Printf("\nOS Template\n");

	ArrWinInfo[0].bpp_mode = BPPMODE_16BPP_565;
	ArrWinInfo[0].bytes_per_pixel = 2;
	ArrWinInfo[0].p_sizex = 1024;
	ArrWinInfo[0].p_sizey = 600;
	ArrWinInfo[0].v_sizex = 1024;
	ArrWinInfo[0].v_sizey = 600;
	ArrWinInfo[0].posx = (1024 - ArrWinInfo[0].p_sizex) / 2;
	ArrWinInfo[0].posy = (600 - ArrWinInfo[0].p_sizey) / 2;

	Lcd_Init();
	Lcd_Win_Init(0, 1);
	Lcd_Brightness_Control(1);

	Lcd_Select_Display_Frame_Buffer(0, 0);
	Lcd_Select_Draw_Frame_Buffer(0, 0);
	Lcd_Draw_Back_Color(BLUE);

	GIC_CPU_Interface_Enable(0,1);
	GIC_Set_Priority_Mask(0,0xFF);
	GIC_Distributor_Enable(1);
	Key_ISR_Enable(ENABLE);


	gpaunContextAddress[0] = &gstRN[NUM_APP0];
	gpaunContextAddress[1] = &gstRN[NUM_APP1];

	// TTLB0 �ㅼ젙
	setAppNum(NUM_APP0);
	SetTransTable_MultiOS(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP0, RW_WBWA | (1<<17));
	SetTransTable_MultiOS(STACK_LIMIT_APP0, STACK_BASE_APP0-1, STACK_LIMIT_APP0, RW_WBWA | (1<<17));
//	SetTransTable_Page(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP0, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
//	SetTransTable_Page(STACK_LIMIT_APP0, STACK_BASE_APP0-1, STACK_LIMIT_APP0, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
	CoInvalidateMainTlb();
	// TTLB1 �ㅼ젙
	setAppNum(NUM_APP1);
//	SetTransTable_Page(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP1, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
//	SetTransTable_Page(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
	SetTransTable_MultiOS(RAM_APP0, (RAM_APP0+SIZE_APP1-1), RAM_APP1, RW_WBWA | (1<<17));
	SetTransTable_MultiOS(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA | (1<<17));
	CoInvalidateMainTlb();
#if 0 // SD Loading
	{
		extern volatile unsigned int sd_insert_flag;
		SDHC_Init();
		SDHC_ISR_Enable(1);
		if(!sd_insert_flag) Uart_Printf("SD 카드 삽입 요망!\n");
		while(!sd_insert_flag);
		SDHC_Card_Init();

		Uart_Printf("APP0 Loading!\n");
		App_Read(SECTOR_APP0, SIZE_APP0, RAM_APP0);
		Uart_Printf("APP1 Loading!\n");
		App_Read(SECTOR_APP1, SIZE_APP0, RAM_APP1);
	}
#endif

	for(;;)
	{
		gstRN[NUM_APP0].RN[SP] = STACK_BASE_APP0;
		gstRN[NUM_APP1].RN[SP] = STACK_BASE_APP1;
		gstRN[NUM_APP0].RN[LR] = RAM_APP0 + 4;
		gstRN[NUM_APP1].RN[LR] = RAM_APP0 + 4;

//		setAppNum(NUM_APP0);
		setAppNum(NUM_APP1);
		Timer0_Int_Delay(ENABLE,5);
		if(getAppNum() == NUM_APP0)
		{
			Uart_Printf("\nAPP0 RUN\n", NUM_APP0);
			API_App0_Ready();
			Run_App(gstRN[NUM_APP0].RN[LR] - 4 , gstRN[NUM_APP0].RN[SP]);
		}

		if(getAppNum() == NUM_APP1)
		{
			Uart_Printf("\nAPP1 RUN\n", NUM_APP1);
			API_App1_Ready();
			Run_App(gstRN[NUM_APP1].RN[LR] - 4 , gstRN[NUM_APP1].RN[SP]);
		}
	}
}
