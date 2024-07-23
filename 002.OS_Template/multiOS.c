#include "multiOS.h"
#include "device_driver.h"


UINT32 pageInfo[MAX_PAGE_NUM][3];
Page_Info page_entry_info[32]; // page에 관한 정보 담은 배열 ex) page_entry_info[0] : 0x44B00000 ~ 0x44B01000 관련 정보

enum mmu
{
    isEmpty = 0,
    VA,
    PA,
    HDD
};



#define PAGE_TABLE_SIZE 4096
//#define SECTION_SIZE 0x100000
#define PAGE_SIZE 4096
#define NUM_L2_ENTRIES 256



void SetTransTable_SinlgePage(UINT32 uVaStart, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
	UINT32* pTT1;
	UINT32* pTT2;

    UINT32 PA_1st_Idx = 0;
	UINT32 PA_2nd_Idx = 0;
    UINT32 VA_1st_Idx = 0;
    UINT32 VA_2nd_Idx = 0;

	VA_1st_Idx = (uVaStart & L1_INDEX_MASK) >> L1_SHIFT;
	PA_1st_Idx = (uPaStart & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (uVaStart & L2_INDEX_MASK) >> L2_SHIFT;
	PA_2nd_Idx = (uPaStart & L2_INDEX_MASK) >> L2_SHIFT;

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));								// Set 1st Entry Info
		*pTT1 = (TTBL0_PAGE + (VA_1st_Idx - 0x441) * 1024) | attr_1st;		// Set 1st Descriptor Info
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2)) ;							// Set 1st Entry Info
		*pTT1 = (TTBL1_PAGE + (VA_1st_Idx - 0x441) * 1024) | attr_1st;		// Set 1st Descriptor Info
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(pTT2 + VA_2nd_Idx);								// Set 2nd Entry Info
	*pTT2 = ((PA_1st_Idx<<L1_SHIFT) | (PA_2nd_Idx<<L2_SHIFT) | attr_2nd);	// Set 2nd Descriptor Info

}


UINT32 VA_2_PA(UINT32 VA) // VA 인자로 받아서 PA 리턴하는 함수
{
	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));
	PA = (*pTT2 & ~PAGE_OFFSET_MASK) | VA_Page_Idx;
	return PA;
}

// 여기에 만들어 주세요 함수 이름만이라도 plz 2nc descriptor 내용 바꿀 수 있도록

void set_VA_Access(UINT32 VA, UINT32 option) // option이 1이면 10, 0이면 00으로
{	// option을 set/clr로 전달하여 해당 VA를 접근을 부여하거나 해제한다.
	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));


	(*pTT2) &= ~0x3;		// [0:1] BIT Clear
    if (option == SET)
	{
		(*pTT2) |= ~0x2;	// Access Approve
	}
	else if (option == CLR)
	{
		(*pTT2) |= 0x0;		// Access Deny
	}

}
UINT32 get_VA_Access(UINT32 VA) // 이거 접근 권한 있으면 1 return
{	//접근권한 상태 출력
    UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;
	UINT32 VA_Page_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;
	VA_Page_Idx = (VA & PAGE_OFFSET_MASK);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));

    UINT32 tmp_mask = 0x3;

    // 00 이면 0 return
    if((tmp_mask & *pTT2) == 0) return 0;
    // 10 이면 1 return
    else if((tmp_mask & *pTT2) == 0x2) return 1;

	return -1; // 이거면 error

}

void set_Page_Info(UINT32 PA, UINT32 VA, UINT32 App_num, UINT32 idx) // page_entry_info update
{
    page_entry_info[idx].PA = PA;
    page_entry_info[idx].VA = VA;
    page_entry_info[idx].App_num = App_num; // 얘가 2면 비어있는 거임
}

void page_entry_init() // page entry 정보 가지고 있는 배열 초기화
{
    UINT32 i;
    for(i = 0; i <= page_entry_num; i++)
    {
        page_entry_info[i].App_num = 2;
    }
}

UINT32 load_page(UINT32 VA, int app_num) // page 적재, 성공하면 0 return
{
    UINT32 i;
    UINT32 full_idx = 0; // 만약 page entry 다 찼다면, 0번부터 순서대로 replacement 수행
    UINT32 page_entry_idx;

    // 일단 PA에서 4, 5bit(AP[1:0]) 00인지 11인지 확인하기
    UINT32 tmp_PA = VA_2_PA(VA);

    // UINT32 tmp_mask = 0x00 | (3 << 4);

    if(!get_VA_Access(VA)) // 2nd descriptor 00 -> page fault
    {
        // 빈칸에 넣기, 32개 전부 확인
        page_entry_idx = -1;
        for(i = 0; i < page_entry_num; i++)
        {
            if(page_entry_info[i].App_num == 2) // 비었으면 여기에 넣자 (RR)
            {
                page_entry_idx = i;
                break;
          }
        }

        // 빈칸 존재한다면 해당 위치(page_entry_idx)에 적재

        // 만약 다 차있다면, 어떤 애를 대체할건지? // 만약 page ent_entry_idx == -1
        if(page_entry_idx == -1)
        {
            page_entry_idx = full_idx;
            set_VA_Access(page_entry_info[full_idx].VA, 0); // replace되는 페이지 접근권한 0으로 수정
            full_idx = (full_idx + 1) % 32;
        }

        UINT32 dest_addr = page_entry_base + page_entry_idx * PAGE_SIZE;
        UINT32 src_addr;
        if(app_num == 0) src_addr = tmp_PA;
        else if(app_num == 1) src_addr = (tmp_PA - RAM_APP0) + RAM_APP1; // VA 가 아니고 PA 이기 때문에 그냥 tmp_PA를 할당하는 게 맞는지???
        else return -1;

        // load page
        memcpy(dest_addr, src_addr, PAGE_SIZE);

        // page entry info update
        page_entry_info[page_entry_idx].PA = tmp_PA;
        page_entry_info[page_entry_idx].VA = VA;
        page_entry_info[page_entry_idx].App_num = app_num;

        // 접근권한 1로 바꾸기
        set_VA_Access(VA, 1);

        // TLB 정리
        CoInvalidateMainTlb();

        // 꺼내 쓰기 (write back) -> 이건 핸들러 정리되고 OS가 다시 접근할 것임 -> 그 떄 자동으로 접근됨

    }
    else if(get_VA_Access(VA)) // 11 -> 이미 있음, 꺼내 쓰기 (write back 어떻게?) -> 실제로는 이 부분 실행될 수 없음, 예외 처리
    {
        // 어디에 있는지 찾기
        page_entry_idx = find_page_entry(VA, tmp_PA, app_num); // page_entry_info 배열의 몇번째 인덱스에 정보 담겨있는지?
        if(page_entry_idx == -1) return -1; // page entry에 없음, 오류

        //CoInvalidateMainTlb();
    }
    else return -1;

    return 0;
}


UINT32 find_page_entry(UINT32 VA, UINT32 PA, int app_num) // page entry에서 찾기
{
    UINT32 i;
    for(i = 0; i < page_entry_num; i++)
    {
        if(page_entry_info[i].App_num == 2) continue;
        if(page_entry_info[i].PA == PA && page_entry_info[i].VA == VA && page_entry_info[i].App_num == app_num)
        {
            return i;
        }
    }
    return -1;
}


void SetTransTable_MultiOS(unsigned int uVaStart, unsigned int uVaEnd, unsigned int uPaStart, unsigned int attr)
{
	int i;
	unsigned int* pTT = 0x0;
	unsigned int nNumOfSec;

	uPaStart &= ~0xfffff;
	uVaStart &= ~0xfffff;
	if (getAppNum() == NUM_APP0)
	{
		pTT = (unsigned int *)TTBL0+(uVaStart>>20);
	}
	else if (getAppNum() == NUM_APP1)
	{
		pTT = (unsigned int *)TTBL1+(uVaStart>>20);
	}
	else
	{
		;
	}
	if (pTT != 0x0)
	{
		nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;
		for(i=0; i<=nNumOfSec; i++)
		{
			*pTT++ = attr|(uPaStart+(i<<20));
		}
	}
}

UINT32 gaucAPP[MAX_APP_NUM];
UINT8 gucAppNum = 0;
struct T_Context gstRN[MAX_APP_NUM];
struct T_Context * gpaunContextAddress[MAX_APP_NUM];


//UINT32 TTBL[MAX_APP_NUM][0x400];

void debugPrint(UINT8 x)
{
    Uart_Printf("========Flag : %d=========\n", x);
    UINT8 i = 0;
    for (i = 0; i < 18; i++)
    {
        Uart_Printf("R[%.2d] : %.8X    %.8X\n", i, gstRN[0].RN[i], gstRN[1].RN[i]);
    }

}
void debugPrintNum(UINT32 x)
{
    Uart_Printf("%.8X  =========\n", x);
}

void setAppNum(UINT8 num)
{
    gucAppNum = num;
}
UINT8 getAppNum()
{
    return gucAppNum;
}
UINT8 getNextAppNum()
{
    return (gucAppNum + 1) % MAX_APP_NUM;
}


void API_App0_Ready(void)
{
   CoSetASID(1);
   CoSetTTBase(TTBL0_CACHE);
   CoInvalidateMainTlb();
   setAppNum(NUM_APP0);
}
void API_App1_Ready(void)
{
   CoSetASID(2);
   CoSetTTBase(TTBL1_CACHE);
   CoInvalidateMainTlb();
   setAppNum(NUM_APP1);
}

void SetTransTable_Page(UINT32 uVaStart, UINT32 uVaEnd, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
   UINT32 i,j;
   UINT32* pTT1;
    UINT32* pTT2;

    UINT32 PA_1st_Idx;
    UINT32 VA_1st_Idx;

//    UINT32 PA_2st_Idx;
//    UINT32 VA_2st_Idx;

    UINT32 nNumOfSec;

   PA_1st_Idx = (uPaStart & ~0xfffff) >> 20;
   VA_1st_Idx = (uVaStart & ~0xfffff) >> 20;

   nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;

   for(i=0; i<=nNumOfSec; i++)
   {
        if (getAppNum() == NUM_APP0)
        {
            pTT1 = (UINT32 *)TTBL0 + VA_1st_Idx + i;   // Set 1st Entry Info
            pTT2 = (UINT32 *)(TTBL0_PAGE + i*0x400);   //
        }
        else
        {
            pTT1 = (UINT32 *)TTBL1 + VA_1st_Idx + i;
            pTT2 = (UINT32 *)(TTBL1_PAGE + i*0x400);
        }

      *pTT1 = (UINT32)pTT2 | attr_1st;                   // Set 1st Descriptor Info
        CleanNInvalid((UINT32)pTT1);
        for (j=0; j<256; j++)
        {
            pTT1 = (UINT32*)pTT2 + j;                  // Set 2nd Entry Info
            *pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);   // Set 2nd Descriptor Info
            CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
        }
   }
}
UINT32 VA_2_PrintPermission(UINT32 VA) // VA ���ڷ� �޾Ƽ� PA �����ϴ� �Լ�
{
	UINT32 PA = 0;	// Return Value
	UINT32* pTT1;
	UINT32* pTT2;

	UINT32 VA_1st_Idx = 0;
	UINT32 VA_2nd_Idx = 0;

	VA_1st_Idx = (VA & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (VA & L2_INDEX_MASK) >> L2_SHIFT;

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2));
	}

	pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
	pTT2 = (UINT32*)(((UINT32)pTT2) | (VA_2nd_Idx<<2));
	PA = (*pTT2);
	return PA;
}

