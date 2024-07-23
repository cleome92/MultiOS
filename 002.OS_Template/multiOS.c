#include "multiOS.h"
#include "device_driver.h"

#define RW_WBWA_PAGE2_ACCESS	(1<<11 | 0x5<<6 | 0x3<< 4 | 1<<3 | 1<<1)
#define RW_WBWA_PAGE2_NOACCESS	(1<<11 | 0x5<<6 | 0x3<< 4 | 1<<3 | 1<<1)
/*
1. APP Switiching 죽이고 한 앱만 계속 실행시키면서 Page 테스트
2. VA to PA 함수 작성 필요
3. printf로 중간중간 setTansTable 내부 for loop 찍어보면서 디버깅 해보기
    setPageAccess(VA, PA)
    setPageNOAccess(VA, PA)
    해당 함수를 통해 Access 권한 부여
    setPageReLocate(VA, PA)
    getPAadress(VA)
*/
#define MAX_PAGE_NUM    (32)
#define SET             (1)
#define CLR             (0)

UINT32 pageInfo[MAX_PAGE_NUM][3];
Page_Info page_entry_info[32]; // page에 관한 정보 담은 배열 ex) page_entry_info[0] : 0x44B00000 ~ 0x44B01000 관련 정보

enum mmu
{
    isEmpty = 0,
    VA,
    PA,
    HDD
};


// VA -> PA 매칭하는 함수
// attr_1st : 0 ~ 9번 bit
// attr_2nd : 0 ~ 11번 bit
// uPaStart : 1차 테이블 시작 위치
// uPaStart2 : 2차 테이블 시작 위치 (이건 파라미터 받을 때 계산할거임)
// paging 영역 : 0x44B0000 ~ 0x44B20000 4KB 32개???


#define PAGE_TABLE_SIZE 4096
//#define SECTION_SIZE 0x100000
#define PAGE_SIZE 4096
#define NUM_L2_ENTRIES 256


#define TTBR0 0xFFFFC000  // TTBR0의 가상 주소
#define L1_INDEX_MASK (0xFFF00000)
#define L2_INDEX_MASK (0x000FF000)
#define PAGE_OFFSET_MASK 	(0x00000FFF)
#define L1_SHIFT (20)
#define L2_SHIFT (12)


// page 적재하는 함수
    /*
    1. VA -> PA 변환 (함수 사용)
    2. 만약 00이라면 -> page entry에 적재해야 함 (memcpy 이용)
        1) page는 RR로 적재
        2) src의 주소 기억하는 공간 따로 만들어야 함 -> 32개니까 32개 PA 배열 만들자 (UINT32)
        3) 만약 다 찼다면 -> 어떤 거를 빼고 다시 넣을 건지? (이건 내가 하는 거 X)
        (+) 대체하고 빠진 page의 속성 다시 00으로 바꿔야 함
    3. 01이면 page entry에서 찾아서 사용
        1) 32개니까 그냥 전체 다 찾기 (loop)
    */
// src 주소 기억 -> OS에 전역변수로 선언해서 저장하기 src 주소 (PA, NA), 몇번 app인지

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
	Uart_Printf("[VA_2_PA] VA[%0.8X], PA[%0.8X]\n", VA, PA);
	return PA;
}


// option을 set/clr로 전달하여 해당 VA를 접근을 부여하거나 해제한다.
void set_VA_Access(UINT32 VA, UINT32 option) // option이 1이면 10, 0이면 00으로
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
    Uart_Printf("[set_Page_Info] PA[%0.8X] VA[%0.8X] App_num[%d]\n", page_entry_info[idx].PA, page_entry_info[idx].VA, page_entry_info[idx].App_num);
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
	Uart_Printf("[load_page]\n");
    UINT32 i;
    UINT32 full_idx = 0; // 만약 page entry 다 찼다면, 0번부터 순서대로 replacement 수행
    UINT32 page_entry_idx;
    UINT32 *dest_addr, *src_addr;

    // 일단 PA에서 4, 5bit(AP[1:0]) 00인지 11인지 확인하기
    UINT32 tmp_PA = VA_2_PA(VA);

    if(!get_VA_Access(VA)) // 2nd descriptor 00 -> page fault
    {
    	Uart_Printf("[load_page] page fault!!\n");
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
        	Uart_Printf("[load_page] page entry is full..idx[%d]\n", full_idx);
            page_entry_idx = full_idx;
            //set_VA_Access(page_entry_info[full_idx].VA, 0);
            // memory로 백업해야 하는 페이지(replace)의 mmu 2차 테이블 값 원래대로 복원, replace되는 페이지 접근권한 0으로 수정
            SetTransTable_SinlgePage(page_entry_info[full_idx].VA, page_entry_info[full_idx].PA, 0x01, 0x00);

            // page entry에 있던 page(replace되는 페이지)를 memory로 다시 백업
            dest_addr = (UINT32 *)(page_entry_info[full_idx].PA);
            src_addr = (UINT32 *)(page_entry_base + full_idx * PAGE_SIZE);
            Uart_Printf("[load_page] backup memcpy [%d] -> [%d]\n", &dest_addr, &src_addr);
            memcpy(dest_addr, src_addr, PAGE_SIZE); // page -> memory

            full_idx = (full_idx + 1) % 32;
        }

        dest_addr = (UINT32 *)(page_entry_base + page_entry_idx * PAGE_SIZE);
        src_addr = (UINT32 *)tmp_PA;

//        if(app_num == 0) src_addr = (UINT32 *)(tmp_PA);
//        else if(app_num == 1) src_addr = (tmp_PA - RAM_APP0) + RAM_APP1; // VA 가 아니고 PA 이기 때문에 그냥 tmp_PA를 할당하는 게 맞는지???
//        else return -1;

        // load page
        memcpy(dest_addr, src_addr, PAGE_SIZE);
        Uart_Printf("[load_page] load memcpy [%d] -> [%d]\n", &dest_addr, &src_addr);

        // page entry info update
        page_entry_info[page_entry_idx].PA = tmp_PA;
        page_entry_info[page_entry_idx].VA = VA;
        page_entry_info[page_entry_idx].App_num = app_num;

        // mmu table의 마지막 주소 변경하여 이후 OS가 page entry에 접근할 수 있도록 함, 접근 권한 변경
        SetTransTable_SinlgePage(VA, tmp_PA, 0x01, 0x02);

        //set_VA_Access(VA, 1);

        // TLB 정리
        CoInvalidateMainTlb();
        Uart_Printf("[load_page] invalidate main TLB\n");

        // 꺼내 쓰기 (write back) -> 이건 핸들러 정리되고 OS가 다시 접근할 것임 -> 그 떄 자동으로 접근됨

    }
    else if(get_VA_Access(VA)) // 11 -> 이미 있음, 꺼내 쓰기 (write back 어떻게?) -> 실제로는 이 부분 실행될 수 없음, 예외 처리
    {
    	Uart_Printf("[load_page] page fault X!!\n");
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
        	Uart_Printf("[find_page_entry] [%d]th entry is found!!\n", i);
            return i;
        }
    }
    Uart_Printf("[find_page_entry] entry can't be found.\n");
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
   setAppNum(NUM_APP0);
}
void API_App1_Ready(void)
{
   CoSetASID(2);
   CoSetTTBase(TTBL1_CACHE);
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

void SetTransTable_SinlgePage(UINT32 uVaStart, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
	Uart_Printf("[SetTransTable_SinglePage] VA[%0.8X] -> PA[0.8X], attr_1st[%d], attr_2nd[%d]\n", uVaStart, uPaStart, attr_1st, attr_2nd);
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
	   Uart_Printf("[SetTransTable_SinglePage] App0\n");
      pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2));               // Set 1st Entry Info
      *pTT1 = (TTBL0_PAGE + VA_1st_Idx*PAGE_SIZE) | attr_1st;      // Set 1st Descriptor Info
   }
   else
   {
	   Uart_Printf("[SetTransTable_SinglePage] App1\n");
      pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2)) ;            // Set 1st Entry Info
      *pTT1 = (TTBL1_PAGE + VA_1st_Idx*PAGE_SIZE) | attr_1st;      // Set 1st Descriptor Info
   }
   CoInvalidateITlbVA((UINT32)pTT1);

   pTT2 = (UINT32*)(*pTT1 & ~0x3FF);
   pTT2 = (UINT32*)(pTT2 + (VA_2nd_Idx<<2));                        // Set 2nd Entry Info
   *pTT2 = ((PA_1st_Idx<<L1_SHIFT) | (PA_2nd_Idx<<L2_SHIFT) | attr_2nd);   // Set 2nd Descriptor Info
   CoInvalidateITlbVA(uVaStart);

   if ((uVaStart&0xFFFFF) == 0)
   {
      //Uart_Printf("[SetTransTable_SinglePage] Success, APP : %d  original VA = %0.8X  PA = %0.8X computed PA = %0.8X, Permission PA = %0.8X\n", getAppNum(), uVaStart, uPaStart, VA_2_PA(uVaStart), VA_2_PrintPermission(uVaStart));
   }
}
