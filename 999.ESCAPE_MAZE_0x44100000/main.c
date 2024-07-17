#include "Device_Driver.h"

#include ".\images\image0.h"
#include ".\images\img101.h"
#include ".\images\nct127_walk.h"
#include ".\images\img01.h"
#include ".\images\how_to_play.h"
#include ".\images\map1.h"
#include ".\images\map2.h"
#include ".\images\ghost_red.h"
#include ".\images\ghost_green.h"
#include ".\images\block_black.h"
#include ".\images\block_black2.h"
#include ".\images\block_blue.h"
#include ".\images\block_blue2.h"
#include ".\images\success.h"
#include ".\images\Logo.h"

#include ".\map_array\map1_arr.h"

extern WIN_INFO_ST ArrWinInfo[5];

#define BLACK	0x0000
#define WHITE	0xffff
#define BLUE	0x001f
#define GREEN	0x07e0
#define RED		0xf800
#define YELLOW	0xffe0
#define VIOLET	0xf81f

#define DELAY	8000

#define startX 0
#define LCD_width 1000
#define LCD_height 600

const unsigned short * img[]={Logo, nct127_walk};
const unsigned short * map[] = {map1, map2};
const unsigned short * user[] = {ghost_red, ghost_green};
const unsigned short * block_blk[] = {block_black, block_black2};
const unsigned short * block_blu[] = {block_blue, block_blue2};
const unsigned short * popup[] = {success, how_to_play};
int px, py, xtmp, ytmp, step_size;
int startY, goalX, goalY;
unsigned char dir;

void var_init(int s_size)
{
	px = 0;
	py = s_size;
	startY = s_size;
	goalY = 600 - s_size;
	goalX = 1000 - s_size * 2;
	step_size = s_size;
}

void game(int idx)
{
	Lcd_Clr_Screen();
	Lcd_Draw_BMP(0,0,map[idx]); // img 출력

	// 그 위에 작은 사진 올릴 수 있는지?
	Lcd_Draw_BMP(startX,startY,user[idx]);
	int map_arr[25][41];

	int i, j;
	if(idx == 0)
	{
		for(i = 0; i < 13; i++)
		{
			for(j = 0; j < 21; j++)
			{
				map_arr[i][j] = map1_arr[i][j];
			}
		}
	}
	else if(idx == 1)
	{
		for(i = 0; i < 25; i++)
		{
			for(j = 0; j < 41; j++) map_arr[i][j] = map2_arr[i][j];
		}
	}
	else
	{
		//Uart_Printf("\nInvalid index.\n");
		return;
	}

		// stage 1
		for(;;)
		{
			if(py == goalY && px == goalX)
			{
				Lcd_Draw_BMP(100,100,popup[0]);
				break;
			}

			//Uart_Printf("\nEnter direction key.\n");
			dir = Uart1_Get_Char(); // 방향키는 특수 키라 두번 입력받아야 함 -> 27 91로 똑같다, WASD space로 동작
			//dir2 = Uart1_Get_Char();


			if(dir == 119)
			{
				//Uart_Printf("UP\n");
				//Lcd_Clr_Screen();
				if(((py - step_size) < 0) || (map_arr[(py - step_size) / step_size][px / step_size] == 0)) continue;
				//Lcd_Draw_BMP(0,0,map[0]);
				py -= step_size;
				if(py < startY) py = startY;
				else Lcd_Draw_BMP(px, py + step_size, block_blk[idx]);
				Lcd_Draw_BMP(px,py,user[idx]);
			}
			else if(dir == 97)
			{
				//Uart_Printf("LEFT\n");
				//Lcd_Clr_Screen();
				//Lcd_Draw_BMP(0,0,map[0]);
				if(((px - step_size) < 0) || (map_arr[(py / step_size)][(px - step_size) / step_size] == 0)) continue;
				px -= step_size;
				if(px < startX) px = startX;
				else Lcd_Draw_BMP(px + step_size, py, block_blk[idx]);
				Lcd_Draw_BMP(px,py,user[idx]);
			}
			else if(dir == 115)
			{
				//Uart_Printf("DOWN\n");
				//Lcd_Clr_Screen();
				//Lcd_Draw_BMP(0,0,map[0]);
				if(((py + step_size) > LCD_height) || (map_arr[(py + step_size) / step_size][px / step_size] == 0)) continue;
				py += step_size;
				if(py > LCD_height) py = LCD_height;
				else if ((py - step_size) == startY && px == startX) Lcd_Draw_BMP(px, py - step_size, block_blu[idx]);
				else Lcd_Draw_BMP(px, py - step_size, block_blk[idx]);
				Lcd_Draw_BMP(px,py,user[idx]);
			}
			else if(dir == 100)
			{
				//Uart_Printf("RIGHT\n");
				//Lcd_Clr_Screen();
				//Lcd_Draw_BMP(0,0,map[0]);
				if(((px + step_size) > LCD_width) || (map_arr[(py / step_size)][(px + step_size) / step_size] == 0)) continue;
				px += step_size;
				if(px > LCD_width) px = LCD_width;
				else if ((px - step_size) == startX && py == startY) Lcd_Draw_BMP(px - step_size, py, block_blu[idx]);
				else Lcd_Draw_BMP(px - step_size, py, block_blk[idx]);
				Lcd_Draw_BMP(px,py,user[idx]);
			}
			else if(dir == 32)
			{
				//Uart_Printf("SPACE\n");
				//Lcd_Clr_Screen();
				//Lcd_Draw_BMP(0,0,map[0]);
				Lcd_Draw_BMP(px, py, block_blk[idx]);
				py = startY;
				px = startX;
				Lcd_Draw_BMP(px,py,user[idx]);
			}
			else if(dir == 104)
			{
				//Uart_Printf("HOWTOPLAY\n");
				//Lcd_Clr_Screen();
				Lcd_Draw_BMP(100,100,popup[1]);
				for(;;)
				{
					// X 키로 나가기
					//Uart_Printf("\nEnter direction key.(EXIT)\n");
					dir = Uart1_Get_Char(); // 방향키는 특수 키라 두번 입력받아야 함 -> 27 91로 똑같다, WASD space로 동작
					if(dir == 120)
					{
						Lcd_Draw_BMP(0,0,map[idx]);
						Lcd_Draw_BMP(px,py,user[idx]);
						break;
					}
					//else Uart_Printf("Input is invalid.[%d]\n", dir);
				}
			}
			//else Uart_Printf("Input is invalid.[%d]\n", dir);
			//Uart_Printf("py[%d], px[%d], y[%d], x[%d] %d\n", py, px, py / step_size, px / step_size, map_arr[py/step_size][px/step_size]);
			//Delay(10);

		}
}

void Main(void)
{
	//Uart_Printf(">>APP0 => LCD Display\n\n");

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
	Lcd_Brightness_Control(8);
	Key_Poll_Init();

	Lcd_Select_Display_Frame_Buffer(0, 0);
	Lcd_Select_Draw_Frame_Buffer(0, 0);
	Lcd_Clr_Screen();
	Lcd_Draw_BMP(0,0,img[0]); // img 출력

	Key_Wait_Key_Released();
	Key_Wait_Key_Pressed();

	// Stage 1
	var_init(50);
	game(0);
	/*
	Lcd_Get_Info_BMP(&xtmp, &ytmp, user[0]);

	//Set_Frame_Buffer1_Cache(0);

	memcpy((void *)LCD_FB41_START_ADDR, user[0], xtmp * ytmp * 2 + 8);
	//Set_Frame_Buffer1_Cache(mode);

	//Timer1_Stopwatch_Run();
	//if(mode == 4) Partial_Invalidate_Cache(LCD_FB41_START_ADDR, xtmp * ytmp * 2 + 8);
	Lcd_Draw_BMP(px,py,user[0]);
	*/
	Key_Wait_Key_Released();
	Key_Wait_Key_Pressed();

	// stage 2
	var_init(25);
	game(1);

	for(;;){}

}
