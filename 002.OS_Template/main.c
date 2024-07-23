#include "device_driver.h"
#include "multiOS.h"

extern WIN_INFO_ST ArrWinInfo[5];
extern UINT32 pageEntry[MAX_PAGE_NUM][5];
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
	UINT32 uPageVA, uPagePA;
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
	int i;
	for (i = 0; i < MAX_PAGE_NUM; i++)
	{
		pageEntry[i][PA] = 0x44b00000 + 0x1000*i;
	}

	gpaunContextAddress[0] = &gstRN[NUM_APP0];
	gpaunContextAddress[1] = &gstRN[NUM_APP1];

#if 0
	// TTLB0 占썬끉��
	setAppNum(NUM_APP0);
	SetTransTable_MultiOS(RAM_APP0, (RAM_APP0+SIZE_APP0-1), RAM_APP0, RW_NCNB | (1<<17));
	SetTransTable_MultiOS(STACK_LIMIT_APP0, STACK_BASE_APP0-1, STACK_LIMIT_APP0, RW_WBWA | (1<<17));
	CoInvalidateMainTlb();
	// TTLB1 占썬끉��
	setAppNum(NUM_APP1);
	SetTransTable_MultiOS(RAM_APP0, (RAM_APP0+SIZE_APP1-1), RAM_APP1, RW_NCNB | (1<<17));
	SetTransTable_MultiOS(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA | (1<<17));
	CoInvalidateMainTlb();
#endif

#if 1
	setAppNum(NUM_APP0);
	uPageVA = VARAM;
	uPagePA = RAM_APP0;

	// uPageVA : 0x3000
	//  RAM_APP1 : 0x44100000 + -----
	while (uPageVA != VARAM + 0x400000)
	{
		SetTransTable_SinlgePage(uPageVA, uPagePA, RW_WBWA_PAGE1, RW_WBWA_PAGE2_NOACCESS);
		uPageVA += 0x1000;
		uPagePA += 0x1000;
	}

	SetTransTable_MultiOS(STACK_LIMIT_APP0, STACK_BASE_APP0-1, STACK_LIMIT_APP0, RW_WBWA | (1<<17));

//	CoInvalidateMainTlb();

	// TTLB1 �ㅼ젙
	setAppNum(NUM_APP1);
	uPageVA = VARAM;
	uPagePA = RAM_APP1;
	while (uPageVA != VARAM + 0x400000)
	{
		SetTransTable_SinlgePage(uPageVA, uPagePA, RW_WBWA_PAGE1, RW_WBWA_PAGE2_NOACCESS);
		uPageVA += 0x1000;
		uPagePA += 0x1000;
	}

	SetTransTable_MultiOS(STACK_LIMIT_APP1, STACK_BASE_APP1-1, STACK_LIMIT_APP1, RW_WBWA | (1<<17));

//	debugPrintNum(VA_2_PA(0x44109C8C));
//	CoInvalidateMainTlb();


#endif
#if 0 // SD Loading
	{
		extern volatile unsigned int sd_insert_flag;
		SDHC_Init();
		SDHC_ISR_Enable(1);
		if(!sd_insert_flag) Uart_Printf("SD 移대뱶 �쎌엯 �붾쭩!\n");
		while(!sd_insert_flag);
		SDHC_Card_Init();

		Uart_Printf("APP0 Loading!\n");
		App_Read(SECTOR_APP0, SIZE_APP0, RAM_APP0);
		Uart_Printf("APP1 Loading!\n");
		App_Read(SECTOR_APP1, SIZE_APP0, RAM_APP1);
	}
#endif

	memset(&gstRN, 0, 2*sizeof(struct T_Context));
	for(;;)
	{
		gstRN[NUM_APP0].RN[SP] = STACK_BASE_APP0;
		gstRN[NUM_APP1].RN[SP] = STACK_BASE_APP1;
		gstRN[NUM_APP0].RN[LR] = VARAM + 4;
		gstRN[NUM_APP1].RN[LR] = VARAM + 4;


		setAppNum(NUM_APP0);
//		setAppNum(NUM_APP1);
		Timer0_Int_Delay(ENABLE, 10);
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
