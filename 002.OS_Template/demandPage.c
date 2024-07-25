#include "multiOS.h"
#include "device_driver.h"


UINT32 pageEntry[MAX_PAGE_NUM][6];


void API_DemandPage(UINT32 faultAddrress, UINT32 mode)
{
//	UINT8 ucReturn = 0;
	UINT32 i = 0;
	static UINT8 victim = 0;

	i = victim;
#if 1
	if (pageEntry[i][isEmpty] == SET)
	{
		pageEntry[i][isEmpty] = CLR;
		SetTransTable_SinlgePage(pageEntry[i][HDD], pageEntry[i][HDD], RW_WBWA_PAGE1, RW_WBWA_PAGE2_NOACCESS);
		CoInvalidateMainTlbVA(pageEntry[i][VA]);
		if (pageEntry[i][Mode] == 0)
		{
			memcpy((UINT32*)(pageEntry[i][PA]), (UINT32*)(pageEntry[i][HDD]), 0x1000);
		}
	}
	debugPrintNum(victim);
	pageEntry[i][isEmpty] = SET;
	pageEntry[i][VA] = (faultAddrress & (~0xfff)) | (getAppNum() + 1);
	pageEntry[i][APP_NUM] = getAppNum();
	pageEntry[i][Mode] = mode;
	if (pageEntry[i][APP_NUM] == NUM_APP0)
	{
		pageEntry[i][HDD] = ((faultAddrress - VARAM) + RAM_APP0)&(~0xFFF);
	}
	else
	{
		pageEntry[i][HDD] = ((faultAddrress - VARAM) + RAM_APP1)&(~0xFFF);
	}
	SetTransTable_SinlgePage(pageEntry[i][VA], pageEntry[i][PA], RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
	memcpy((UINT32*)(pageEntry[i][PA]), (UINT32*)(pageEntry[i][HDD]), 0x1000);
	CoInvalidateMainTlbVA(pageEntry[i][VA] | (getAppNum()+1));
	CoInvalidateICache();
//	Uart_Printf("%.8x   %.8x   %0.8x   %0.8x\n", faultAddrress, VA_2_PA(faultAddrress), (pageEntry[i][PA]), (pageEntry[i][HDD]));

#endif
#if 0
//	SetTransTable_SinlgePage(pageEntry[i][VA], pageEntry[i][HDD], RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
	SetTransTable_SinlgePage(faultAddrress, VA_2_PA(faultAddrress), RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
#endif

	victim = (victim + 1)%MAX_PAGE_NUM;

//	ucReturn = CLR;

//	return ucReturn;
}

//UINT32 load_page_2(UINT32 VA, int app_num, int mode)
//{
//	UINT32 src = ((app_num == 0) ? 0x44100000 : 0x44500000) + ((VA & ~0xFFF) - 0x30000000);
//	UINT32 dst = 0x44b00000 + (victim * 0x1000);
////	Key_Wait_Key_Pressed();
////	Key_Wait_Key_Released();
//
////	Uart_Printf("VA = %p, src = %p, dst = %p\n", VA, src, dst);
//	if (org[victim]){
//		debugPrintNum(victim);
//		debugPrintNum(org[victim]);
//		//table_2nd[victim] = VA_2_pTT2(org[victim]);
//		*(table_2nd[victim]) = 0;
////		debugPrintNum(5);
//		if (org_mode[victim] == 0){
//			debugPrintNum(0);
//			memcpy((UINT32*)org[victim], (UINT32*)dst, 0x1000);
//		}
//	}
//
//	org_mode[victim] = mode;
//	org[victim] = src;
//	debugPrintNum(1);
//	table_2nd[victim] = VA_2_pTT2(VA);
//	debugPrintNum(2);
//	flag = 1;
//	//Uart1_Get_Char();
//	SetTransTable_SinlgePage(VA, dst, RW_WBWA_PAGE1, RW_WBWA_PAGE2_ACCESS);
//	flag = 0;
//	debugPrintNum(3);
////	Uart_Printf("table_2nd[victim] = %x, *table_2nd[victim] = %x\n", table_2nd[victim], *table_2nd[victim]);
//	//VA에 해당하는 2차 TT 수정 (dst로 접근하도록 수정) 캐쉬정책 inner WT, outter WBWA, nG
//	memcpy((UINT32*)dst, (UINT32*)src, 0x1000);
//	CoInvalidateMainTlbVA((VA & ~0xFFF) | (app_num+1));
//	CoInvalidateICache();
//	victim = (victim + 1) % page_entry_num;
////	Key_Wait_Key_Pressed();
////	Key_Wait_Key_Released();
//	return 0;
//}
////	Uart_Printf("Page Load? [1] : Yes, [0] : No \n");
////	x = Uart1_Get_Char();
//
////	if (x == '1') ucReturn = 1;
////	else if (x == '0') return 0;
