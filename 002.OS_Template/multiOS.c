#include "multiOS.h"

#define MAX_APP_NUM (2)
#define NUM_APP0    (0)
#define NUM_APP1    (1)
#define NUM_NONAPP  (0xFF)

#define 	RAM_APP0			0x44100000                      //
#define 	RAM_APP1			(RAM_APP0+SIZE_APP0)            //
#define 	SIZE_STACK0			(1*1024*1024)
#define 	SIZE_STACK1			(1*1024*1024)
#define 	STACK_LIMIT_APP0	(RAM_APP1+SIZE_APP1)
#define 	STACK_LIMIT_APP1	(STACK_LIMIT_APP0+SIZE_STACK0)
#define 	STACK_BASE_APP0		(STACK_LIMIT_APP0+SIZE_STACK0)
#define 	STACK_BASE_APP1		(STACK_LIMIT_APP1+SIZE_STACK1)

#define 	SIZE_APP0			(4*1024*1024)
#define 	SIZE_APP1			(4*1024*1024)

#define 	SECTOR_APP0			100
#define 	SECTOR_APP1			5000

#define SECTOR_SIZE 		512
#define ALIGN_SECTOR(x)	 	((((x+(SECTOR_SIZE-1))&~(SECTOR_SIZE-1))/SECTOR_SIZE))

#define SECTION_SIZE	(0x4000)
#define TTBL0 		(0x44000000)
#define TTBL0_PAGE 	(TTBL0 + SECTION_SIZE)
#define TTBL1 		(0x44008000)
#define TTBL1_PAGE 	(TTBL1 + SECTION_SIZE)
#define TTBL0_CACHE (TTBL0 | (1<<6) | (1<<3) | (0<<1) | (0<<0))
#define TTBL1_CACHE (TTBL1 | (1<<6) | (1<<3) | (0<<1) | (0<<0))

typedef unsigned char UINT8;
typedef unsigned int UINT32;

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

enum mmu
{
    isEmpty = 0,
    VA,
    PA,
    HDD
};

void smallPageLoading(UINT32* VA, UINT32* PA)
{
    memcpy(VA, PA, sizeof(UINT32)*1024);
}

/*
void demandPage(void)
{
    UINT32 i = 0;
    for (i = 0; i < MAX_PAGE_NUM; i++)
    {
        if (pageInfo[i][isEmpty] == CLR)
        {
            pageInfo[i][isEmpty] = SET;
            pageInfo[i][VA] = FaultAddress;
            pageInfo[i][PA] = VA2PA(VA);
            pageInfo[i][HDD] = getHDD(pageInfo[i][PA]);
            smallPageLoading(pageInfo[i][VA], pageInfo[i][PA]);
            // setPageLocate(VA);
            break;
        }
    }
    if (i == MAX_PAGE_NUM)
    {
        setPageNOAccess(VA, PA); // RR으로 VA 임의 지정
        savePage2RAM(VA, PA);
        setPageReLocate(VA, PA);
    }
}
*/


// VA -> PA 매칭하는 함수
void SetTransTable_Page(UINT32 uVaStart, UINT32 uVaEnd, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
	UINT32 i,j;
	UINT32* pTT1;
    UINT32* pTT2;

    UINT32 PA_1st_Idx; // 상위 12비트 PA 시작
    UINT32 VA_1st_Idx; // 상위 12비트 VA 시작
    UINT32 nNumOfSec; // section 개수

	PA_1st_Idx = (uPaStart & ~0xfffff) >> 20;
	VA_1st_Idx = (uVaStart & ~0xfffff) >> 20;

	nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;

	for(i=0; i<=nNumOfSec; i++)
	{
        if (getAppNum() == NUM_APP0)
        {
            pTT1 = (UINT32 *)TTBL0 + ((VA_1st_Idx + i) << 2);	// Set 1st Entry Info
            pTT2 = (UINT32 *)(TTBL0_PAGE + i*0x400);	//
        }
        else // APP 1
        {
            pTT1 = (UINT32 *)TTBL1 + VA_1st_Idx + i;
            pTT2 = (UINT32 *)(TTBL1_PAGE + i*0x400);
        }

		*pTT1 = (UINT32)pTT2 | attr_1st;   	 				// Set 1st Descriptor Info
        CleanNInvalid((UINT32)pTT1);
        for (j=0; j<256; j++)
        {
            pTT1 = (UINT32*)pTT2 + j;						// Set 2nd Entry Info
            *pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);	// Set 2nd Descriptor Info
            CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
        }
	}
}

// VA -> PA 매칭하는 함수
// attr_1st : 0 ~ 9번 bit
// attr_2nd : 0 ~ 11번 bit
// uPaStart : 1차 테이블 시작 위치
// uPaStart2 : 2차 테이블 시작 위치 (이건 파라미터 받을 때 계산할거임)
// paging 영역 : 0x44B0000 ~ 0x44B20000 4KB 32개???
/*
#define page_table_size 32 // page table 전체 개수
void SetTransTable_Page_parkdoyun(UINT32 uVaStart, UINT32 uVaEnd, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd, UINT32 uPaStart2)
{
    // 이거 제거임
    // 건들지 마셈 ㅡㅡ
    // 위에 꺼 고치셈
    // OK
	UINT32 i,j;
	UINT32* pTT1; // 1번째 TT Entry 주소 (section)
    UINT32* pTT2; // 2번째 TT Entry 주소 (page)

    UINT32 base_page_tbl = uPaStart & 0xFFFFFC00; // 상위 22비트, page table base address


    UINT32 PA_1st_Idx; // 상위 20비트, PA 시작
    UINT32 VA_1st_Idx; // 상위 12비트, VA 시작
    UINT32 VA_2nd_Idx; // 12 ~ 19 비트
    UINT32 nNumOfSec; // section 개수

	PA_1st_Idx = (uPaStart & 0xFFFFF000) >> 12;
	VA_1st_Idx = (uVaStart & 0xFFF00000) >> 20;
    VA_2nd_Idx = (uVaStart & 0x000FF000) >> 12;

	nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;

    if (getAppNum() == NUM_APP0)
    {
        pTT1 = (UINT32 *)TTBL0 + (VA_1st_Idx << 2);	// Set 1st Entry Info
        //pTT2 = (UINT32 *)(TTBL0_PAGE + i*0x400);	//
    }
    else // APP 1
    {
        pTT1 = (UINT32 *)TTBL1 + (VA_1st_Idx << 2);
        //pTT2 = (UINT32 *)(TTBL1_PAGE + i*0x400);
    }
    pTT2 = (base_page_tbl+(i<<22)) | (VA_2nd_Idx << 2) | 0x01;

    UINT32 page_entry_idx = 0;
	for(i=0; i<=nNumOfSec; i++)
	{
        // 첫번째 T/T Entry 설정
        *pTT1++ = attr_1st | (base_page_tbl+(i<<22));

        // 읽어온 2nd level descriptor
        *pTT2++ =

        //pTT2 = (PA_1st_Idx + (nNumOfSec - i)) << 20 | (uVaStart & 0x00000FFF);

        // page entry는 0x44B00000 부터 0x44B20000까지 32개가 관리된다
        // 그러면 page entry의 N번째에 뭐가 들어갈지 다 저장해야 함???!?!?!?


        page_entry_idx = (page_entry_idx + 1) % 32; // page entry의 몇번째에 들어갈지, RR

		*pTT1 = (UINT32)pTT2 | attr_1st;   	 				// Set 1st Descriptor Info
        CleanNInvalid((UINT32)pTT1);
        for (j=0; j<256; j++)
        {
            pTT1 = (UINT32*)pTT2 + j;						// Set 2nd Entry Info
            *pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);	// Set 2nd Descriptor Info
            CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
        }
	}
}
*/

#define PAGE_TABLE_SIZE 4096
#define SECTION_SIZE 0x100000
#define PAGE_SIZE 4096
#define NUM_L2_ENTRIES 256


// 페이지 테이블을 초기화하는 함수
void init_l2_page_table(UINT32* l2_page_table) {
    for (int i = 0; i < NUM_L2_ENTRIES; i++) {
        l2_page_table[i] = 0;
    }
}

// 가상 주소를 실제 주소로 매핑하는 2차 테이블 설정 함수
void set_l2_page_table_entry(UINT32* l2_page_table, UINT32 virtual_address, UINT32 physical_address, UINT32 attributes) {
    // 2차 페이지 테이블 엔트리 인덱스 계산
    UINT32 index = (virtual_address & (SECTION_SIZE - 1)) / PAGE_SIZE;
    // 페이지 테이블 엔트리 설정
    l2_page_table[index] = (physical_address & ~(PAGE_SIZE - 1)) | (attributes & 0xFFF);
}

typedef struct {
    UINT32* l1_page_table;
    UINT32* l2_page_tables;
} T_page_table;



#define TTBR0 0xFFFFC000  // TTBR0의 가상 주소
#define L1_INDEX_MASK (0xFFF00000)
#define L2_INDEX_MASK (0x000FF000)
#define PAGE_OFFSET_MASK 	(0x00000FFF)
#define L1_SHIFT (20)
#define L2_SHIFT (12)

// 가상 주소를 실제 주소로 변환하는 함수
UINT32 virtual_to_physical(T_page_table* page_table, UINT32 virtual_address) {
    // 1차 페이지 테이블 엔트리 인덱스 계산
    UINT32 l1_index = (virtual_address & L1_INDEX_MASK) >> L1_SHIFT;
    UINT32 l1_entry = page_table->l1_page_table[l1_index];

    // 2차 페이지 테이블 엔트리 인덱스 계산
    UINT32 l2_index = (virtual_address & L2_INDEX_MASK) >> L2_SHIFT;
    UINT32* l2_page_table = (UINT32*)(l1_entry & ~0xFFF);
    UINT32 l2_entry = l2_page_table[l2_index];

    // 실제 주소 계산
    UINT32 physical_address = (l2_entry & ~0xFFF) | (virtual_address & 0xFFF);

    return physical_address;
}


void SetTransTable_SinlgePage(UINT32 uVaStart, UINT32 uPaStart, UINT32 attr_1st, UINT32 attr_2nd)
{
	UINT32 i,j;
	UINT32* pTT1;
    UINT32* pTT2;

    UINT32 PA_1st_Idx = 0;
    UINT32 VA_1st_Idx = 0;
	UINT32 PA_2nd_Idx = 0;
    UINT32 VA_2nd_Idx = 0;

	UINT32 nNumOfSec = 0;

	VA_1st_Idx = (uVaStart & L1_INDEX_MASK) >> L1_SHIFT;
	PA_1st_Idx = (uPaStart & L1_INDEX_MASK) >> L1_SHIFT;
	VA_2nd_Idx = (uVaStart & L2_INDEX_MASK) >> L2_SHIFT;
	PA_2nd_Idx = (uPaStart & L2_INDEX_MASK) >> L2_SHIFT;


	debugPrintNum(nNumOfSec);

	if (getAppNum() == NUM_APP0)
	{
		pTT1 = (UINT32 *)(TTBL0 + VA_1st_Idx*sizeof(int)) ;			// Set 1st Entry Info
		//pTT2 = (UINT32 *)TTBL0_PAGE;
		*pTT1 = TTBL0_PAGE + VA_1st_Idx*PAGE_SIZE | attr_1st;								// Set 1st Descriptor Info
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + VA_1st_Idx*sizeof(int)) ;			// Set 1st Entry Info
		//pTT2 = (UINT32 *)TTBL1_PAGE;
		*pTT1 = TTBL1_PAGE | attr_1st;								// Set 1st Descriptor Info
	}
	CleanNInvalid((UINT32)pTT1);

	for (j=0; j<256; j++)
	{
		pTT1 = (UINT32*)pTT2 + (j<<2);						// Set 2nd Entry Info
		*pTT1 = (PA_1st_Idx<<20 | j<<12 | attr_2nd);	// Set 2nd Descriptor Info
		CoInvalidateITlbVA(PA_1st_Idx<<20 | j<<12);
	}

}


// VA 0x84100000 ~ 0x84900000 -1 => PA 0x44100000 ~ 0x44900000 -1
UINT8 virtual_to_physical(UINT32 virtual_address)
{
	UINT8 appNum = getAppNum();// current app number
	UINT8 save_addr = virtual_address - 0x40000000 - (appNum*SIZE_APP0); // VA APP0(0x441) APP1(0x445)
	UINT32 l1_descriptor;

	//UINT32 sizeOfPAGE = 1 << 12; // 4KB == 2^12
	UINT32 va19_12;

    // 1 Level Descriptor
	if(appNum == NUM_APP0)
	{
		l1_descriptor = (unsigned int)(TTBL0_PAGE + ((save_addr>>20)<<2));
	}
	if(appNum == NUM_APP1)
	{
		l1_descriptor = (unsigned int)(TTBL1_PAGE + ((save_addr>>20)<<2));
	}

	va19_12 = (save_addr&0xff000)>>12;

}

// page 적재하는 함수 작성
// 박도윤
// page 적재하는 코드
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


// 아 나가셈 ㅡㅡ
// 자리 있음요 ↓

// 가세요라
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
		pTT1 = (UINT32 *)(TTBL0 + (VA_1st_Idx<<2)) ;			// Set 1st Entry Info
		*pTT1 = (TTBL0_PAGE + VA_1st_Idx*PAGE_SIZE) | attr_1st;		// Set 1st Descriptor Info
	}
	else
	{
		pTT1 = (UINT32 *)(TTBL1 + (VA_1st_Idx<<2)) ;			// Set 1st Entry Info
		*pTT1 = (TTBL1_PAGE + VA_1st_Idx*PAGE_SIZE) | attr_1st;		// Set 1st Descriptor Info
	}

	pTT2 = (UINT32*)((*pTT1) & 0xFFFFC00);
	pTT2 = (UINT32*)(pTT2 + (VA_2nd_Idx<<2));								// Set 2nd Entry Info
	*pTT2 = (PA_1st_Idx<<L1_SHIFT | PA_2nd_Idx<<L2_SHIFT | attr_2nd);		// Set 2nd Descriptor Info

	CoInvalidateITlbVA(PA_1st_Idx<<L1_SHIFT | PA_2nd_Idx<<L2_SHIFT);
}
