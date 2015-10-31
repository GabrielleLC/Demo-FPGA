#include "xparameters.h"
#include "xgpio.h"
#include "xutil.h"
#include "vga_ip.h"
#include "xps2.h"
#include <xtmrctr.h>
#include <xintc_l.h>
#include "lab_gobang_AI_2.h"
#include "VGADraw.h"
#include "AI.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#define Human    1
#define Computer 0
#define Nothing  2

typedef struct node
{
    int x;
    int y;
}POINT;

int X,Y;
int N=15; //棋盘大小

int huo4[3];      //活四数
int huo3[3];      //活三数

volatile int Total =0 ;//已下棋子总数
int pointValue[15][15];    //每点评分
int pointValue2[15][15];   //博弈树每点评分

int who=Nothing;
volatile int fall_flag;//人是否落子标志
void everyScore(int who);
//====================================================

int board_state[15][15];
POINT board_record[225];//双方落子记录
int r_times=0;
int sumall(int data[N][N])  ;
int sumline(int line[],int length,int who);
int p_Score(int num,int numof0, int bp[],int who,int zhong0) ;
int jinshou() ;
POINT current_pos,previous_pos;
POINT best(int who) ;
POINT bestboyi(int who);

//======================================================
int huo4[3]={0,0,0};      //活四数
int prehuo4[3]={0,0,0};    //之前的活四数
int huo3[3]={0,0,0};      //活三数
int prehuo3[3]={0,0,0};     //之前的活三数
int changlian=0;           //如果人长连禁手，则置为1
int first;            //谁先走
int min;


//====================================================



int board_state[15][15];
POINT board_record[225];

int turn;
int level=1;
int status=PVP;
int step_flag;
int win_flag;
int BackTimes=0;
int maxMoveX, maxMoveY;
volatile int time0=0;
volatile int count=0;
volatile u32 ssBuf;
static const u32 rgfsNumMap[10] = {0x000000C0, 0x000000F9, 0x000000A4,
								0x000000B0, 0x00000099, 0x00000092,
								0x00000082, 0x000000F8, 0x00000080,
								0x00000090};

//====================================================

int CheckWin(int x_pos, int y_pos, int turn)
{
	int i,j,count;
	//水平
	count=1;
	for (i=x_pos+1, j=y_pos; i<15; i++) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	for (i=x_pos-1, j=y_pos; i>=0; i--) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
    }
	if (count==5)
		return 1;

	//垂直
	count=1;
	for (i=x_pos, j=y_pos+1; j<15; j++) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	for (i=x_pos, j=y_pos-1; j>=0; j--) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	if (count==5)
		return 1;

	//左上至右下
	count=1;
	for (i=x_pos+1,j=y_pos+1; i<15 && j<15; i++,j++) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	for (i=x_pos-1,j=y_pos-1; i>=0 && j>=0; i--,j--) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	if (count==5)
		return 1;

	//左下至右上
	count=1;
    for (i=x_pos+1,j=y_pos-1; i<15 && j>=0; i++,j--) {
	 	if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	for (i=x_pos-1,j=y_pos+1; i>=0 && j<15; i--,j++) {
		if (board_state[i][j]==turn)
			count++;
		else
			break;
	}
	if (count==5)
		return 1;

	//no win
	return 0;
}

int CheckBan(int x_pos, int y_pos, int turn)
{
	int i,j,count,lian3,lian4,lian6;

	lian3=0;
	lian4=0;
	lian6=0;

	//水平
	count=1;
	if(board_state[x_pos+1][y_pos]==EMPTY){
		for (i=x_pos+2, j=y_pos; i<15; i++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1, j=y_pos; i>=0; i--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
	    }
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos-1][y_pos]==EMPTY){
		for (i=x_pos+1, j=y_pos; i<15; i++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-2, j=y_pos; i>=0; i--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos-1][y_pos]!=EMPTY&&board_state[x_pos+1][y_pos]!=EMPTY){
		for (i=x_pos+1, j=y_pos; i<15; i++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1, j=y_pos; i>=0; i--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
		if (count>5)
			++lian6;
	}

	//垂直
	count=1;
	if(board_state[x_pos][y_pos+1]==EMPTY){
		for (i=x_pos, j=y_pos+2; j<15; j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos, j=y_pos-1; j>=0; j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
	    }
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos][y_pos-1]==EMPTY){
		for (i=x_pos, j=y_pos+1; j<15; j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos, j=y_pos-2; j>=0; j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos][y_pos+1]!=EMPTY&&board_state[x_pos][y_pos-1]!=EMPTY){
		for (i=x_pos, j=y_pos+1; j<15; j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos, j=y_pos-1; j>=0; j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
		if (count>5)
			++lian6;
	}

	//左上至右下
	count=1;
	if(board_state[x_pos+1][y_pos+1]==EMPTY){
		for (i=x_pos+2,j=y_pos+2; i<15 && j<15; i++,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1,j=y_pos-1; i>=0 && j>=0; i--,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
	    }
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos-1][y_pos-1]==EMPTY){
		for (i=x_pos+1,j=y_pos+1; i<15 && j<15; i++,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-2,j=y_pos-2; i>=0 && j>=0; i--,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos+1][y_pos+1]!=EMPTY&&board_state[x_pos-1][y_pos-1]!=EMPTY){
		for (i=x_pos+1,j=y_pos+1; i<15 && j<15; i++,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1,j=y_pos-1; i>=0 && j>=0; i--,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
		if (count>5)
			++lian6;
	}

	//左下至右上
	count=1;
	if(board_state[x_pos+1][y_pos-1]==EMPTY){
		for (i=x_pos+2,j=y_pos-2; i<15 && j>=0; i++,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1,j=y_pos+1; i>=0 && j<15; i--,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
	    }
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos-1][y_pos+1]==EMPTY){
		for (i=x_pos+1,j=y_pos-1; i<15 && j>=0; i++,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-2,j=y_pos+2; i>=0 && j<15; i--,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
	}

	count=1;
	if(board_state[x_pos+1][y_pos-1]!=EMPTY&&board_state[x_pos-1][y_pos+1]!=EMPTY){
		for (i=x_pos+1,j=y_pos-1; i<15 && j>=0; i++,j--) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
		}
		for (i=x_pos-1,j=y_pos+1; i>=0 && j<15; i--,j++) {
			if (board_state[i][j]==turn)
				count++;
			else
				break;
    	}
		if (count==3)
			++lian3;
		if (count==4)
			++lian4;
		if (count>5)
			++lian6;
	}

	if (lian3>1) return 1;
	if (lian4>1) return 1;
	if (lian6>0) return 1;

	return 0;
}

void timer_int_handler(void * baseaddr_p) {
	unsigned int csr;
	int num;
    unsigned int MskAnodes;

	csr = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0);
    if (csr & XTC_CSR_INT_OCCURED_MASK && win_flag==0) {
	    count++;
	    if (count%2==0) {
	    	MskAnodes=0x00000D00;
	    	num=(TIME_LIMIT-time0)/10;
	    }
	    else {
	    	MskAnodes=0x00000E00;
	    	num=(TIME_LIMIT-time0)%10;
	    }
    	ssBuf = MskAnodes | rgfsNumMap[num];
        Xil_Out32(XPAR_SEVSEG_DISP_12BITS_BASEADDR, ssBuf);
    }
    if (count==200) {
    	count=0;
    	time0++;
    	DrawBack(3,1119,EMPTY);
    	DrawTime(88, EMPTY);
        if (time0>20)
    	    DrawTime(TIME_LIMIT-time0, CURSOR);
    	else
    		DrawTime(TIME_LIMIT-time0, turn);
    }
    if (time0>TIME_LIMIT) {
    	time0=0;
    	if (turn==HUMAN_PLAYER) {
    		turn=COMPUTER_PLAYER;
    		xil_printf("\r\nComputer Player's turn!\r\n");
    	}
    	else {
    	    turn=HUMAN_PLAYER;
    	    xil_printf("\r\nHuman Player's turn!\r\n");
    	}
    }
    XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0, csr);
}

void InitializeGame(int x_cur, int y_cur)
{
	int i,j;
	for (i=0; i<15; i++) {
		for (j=0; j<15; j++) {
		    board_state[i][j]=EMPTY;
		}
	}
	DrawNumber(level,3,2,EMPTY);
	DrawWinning(0,1, EMPTY);
	DrawStatus(1, 21, EMPTY,PVP);
	DrawBack(3,1119,EMPTY);
	count=0;
	time0=0;
	turn=HUMAN_PLAYER;
	step_flag=0;
	win_flag=0;

	DrawBoard();
	DrawChess(x_cur, y_cur, CURSOR);
	DrawTime(88, EMPTY);
	DrawTime(30, turn);
	DrawStatus(1, 21, COMPUTER_PLAYER,PVP);
	xil_printf("\r\nGame Start!\r\n");

}

int main (void)
{
   XGpio dip;
   int dip_check;


   static XPs2 Ps2Inst;
   XPs2_Config *ConfigPtr;
   u32 StatusReg;
   u32 BytesReceived;
   u8 RxBuffer;
   int key_count=0;
   int i;

   status=PVP;
   int x_cur=7, y_cur=7, x_pos=0, y_pos=0;

   XGpio_Initialize(&dip, XPAR_DIP_SWITCHES_8BITS_DEVICE_ID);
   XGpio_SetDataDirection(&dip, 1, 0xffffffff);

   ConfigPtr = XPs2_LookupConfig(XPAR_XPS_PS2_0_0_DEVICE_ID);
   XPs2_CfgInitialize(&Ps2Inst, ConfigPtr, ConfigPtr->BaseAddress);

   XIntc_RegisterHandler(XPAR_XPS_INTC_0_BASEADDR,
   		                 XPAR_XPS_INTC_0_XPS_TIMER_0_INTERRUPT_INTR,
                         (XInterruptHandler) timer_int_handler,
                         (void *)XPAR_XPS_TIMER_0_BASEADDR);

   XIntc_MasterEnable(XPAR_XPS_INTC_0_BASEADDR);
   XIntc_EnableIntr(XPAR_XPS_INTC_0_BASEADDR, 0x1);

   XTmrCtr_SetLoadReg(XPAR_XPS_TIMER_0_BASEADDR, 0, 333333);

   XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

   XIntc_EnableIntr(XPAR_XPS_TIMER_0_BASEADDR, XPAR_XPS_TIMER_0_INTERRUPT_MASK);

   XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK |
     						XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

   microblaze_enable_interrupts();

   InitializeGame(x_cur, y_cur);status=PVP;

   xil_printf("-- Game Starts! --\r\n");
   xil_printf("\r\nHuman Player's turn!\r\n");

   int vga_input;
   vga_input=(0<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(1<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(2<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(3<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(4<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(5<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(6<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(7<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(8<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(0<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

   vga_input=(0<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(1<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(2<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(3<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(4<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(5<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(6<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(7<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(8<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

     vga_input=(0<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(1<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(2<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(3<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(4<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(5<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(6<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(7<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(8<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
     vga_input=(9<<24)+(29<<16)+(1<<8)+3;
     VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

   vga_input=(0<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(1<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(2<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(3<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(4<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(5<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(6<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(7<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(8<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

   vga_input=(9<<24)+(1<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(2<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(3<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(4<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(5<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(6<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(7<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(8<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(9<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(10<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

   vga_input=(9<<24)+(11<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(12<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(13<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(14<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(15<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(16<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(17<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(18<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(19<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(20<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);

   vga_input=(9<<24)+(21<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(22<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(23<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(24<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(25<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(26<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(27<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(28<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(29<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);
   vga_input=(9<<24)+(30<<16)+(1<<8)+3;
   VGA_IP_mWriteReg(XPAR_VGA_IP_0_BASEADDR, 0, vga_input);


   while (1)
   {
	  if (turn==HUMAN_PLAYER || (turn==COMPUTER_PLAYER && status==PVP)) {
		  do {
			  if (turn==COMPUTER_PLAYER && status==PVC)
			  	  break;
		      dip_check=XGpio_DiscreteRead(&dip, 1);
	          StatusReg = XPs2_GetStatus(&Ps2Inst);
	      }while((StatusReg & XPS2_STATUS_RX_FULL) == 0);
	      BytesReceived = XPs2_Recv(&Ps2Inst, &RxBuffer, 1);
	      key_count=(key_count+1)%3;
	      if (key_count==0) {
	    	  if (RxBuffer==0x21&& win_flag==0) {
	    		  DrawNumber(level,3,2,EMPTY);

	    		  if(level==1)
	    			  level=2;
	    		  else
	    			  level=1;

	    		  if(status==PVC )
	    		  DrawNumber(level,3,2,0);
	    		  else if(status==CVP)
			      DrawNumber(level,3,2,1);
	    		  else
	    		  DrawNumber(level,3,2,EMPTY);

	    	  }
		      if (RxBuffer==0x1D && win_flag==0) {
			      EraseCursor(x_cur, y_cur);
		          if (y_cur<=0)
		    	      y_cur=14;
		          else
		    	      y_cur--;
		          DrawChess(x_cur, y_cur, CURSOR);
		      }
		      if (RxBuffer==0x1B && win_flag==0) {
		  	      EraseCursor(x_cur, y_cur);
		  	      if (y_cur>=14)
		  	          y_cur=0;
		  	      else
		  	          y_cur++;
		  	      DrawChess(x_cur, y_cur, CURSOR);
		      }
		      if (RxBuffer==0x1C && win_flag==0) {
		  	      EraseCursor(x_cur, y_cur);
		  	      if (x_cur<=0)
		  	          x_cur=14;
		  	      else
		  		      x_cur--;
		  	      DrawChess(x_cur, y_cur, CURSOR);
		      }
		      if (RxBuffer==0x23 && win_flag==0) {
		  	      EraseCursor(x_cur, y_cur);
		  	      if (x_cur>=14)
		  	          x_cur=0;
		  	      else
		  		      x_cur++;
		  	      DrawChess(x_cur, y_cur, CURSOR);
		      }
		      if (RxBuffer==0x5A && win_flag==0) {
		    	  DrawBack(3,1119,EMPTY);
			      if (board_state[x_cur][y_cur]==EMPTY) {

			  	      if(status==CVP)
				      DrawChess(x_cur, y_cur, 1-turn);
			  	      else
			  	      DrawChess(x_cur, y_cur, turn);

				      board_state[x_cur][y_cur]=turn;
				      board_record[BackTimes].x=x_cur;
				      board_record[BackTimes].y=y_cur;
				      BackTimes++;
				      count=0;
				      time0=0;
				      if (turn==COMPUTER_PLAYER)
				    	  step_flag=1;
				      if (CheckWin(x_cur,y_cur,turn)==1) {
					      xil_printf("\r\nHuman Player wins!\r\n");
					      win_flag=1;
					      DrawWinning(0, 1, EMPTY);
					      if(status==CVP)
						      DrawWinning(0, 1, 1-turn);
					      else
					          DrawWinning(0, 1, turn);
				      }
				      if (CheckBan(x_cur,y_cur,turn)==1){
				    	  xil_printf("\r\nComputer Player wins!\r\n");
				    	  win_flag=1;
				    	  DrawWinning(0, 1, EMPTY);
					      if(status==CVP)
						      DrawWinning(0, 1, turn);
					      else
					          DrawWinning(0, 1, 1-turn);
				      }
				      else {
				    	  if (turn==HUMAN_PLAYER)
				    		  turn=COMPUTER_PLAYER;
				    	  else
				    		  turn=HUMAN_PLAYER;
				          xil_printf("\r\nComputer Player's turn!\r\n");
				      }
			      }
			  }
		      if (RxBuffer==0x29 && turn==HUMAN_PLAYER && win_flag==0) {
		    	  count=0;time0=0;

		    	  if (status==PVP) {
			    	  x_cur=7;
			    	  y_cur=7;
			    	  for (i=0; i<256; i++) {
			    	  		board_record[i].x=0;
			    	  	    board_record[i].y=0;
			    	  			  }
				      InitializeGame(x_cur, y_cur);status=PVP;
		    		  DrawStatus(1, 21, EMPTY,status);
		    		  status=PVC;
		    		  DrawNumber(level,3,2,0);
		    		  DrawStatus(1, 21, COMPUTER_PLAYER,status);
		    	  }
		    	  else if(status==PVC) {
			    	  x_cur=7;
			    	  y_cur=7;
			    	  for (i=0; i<256; i++) {
			    	  		board_record[i].x=0;
			    	  	    board_record[i].y=0;
			    	  			  }
				      InitializeGame(x_cur, y_cur);status=PVP;
		    		  DrawStatus(1, 21, EMPTY,status);
		    		  status=CVP;
		    		  DrawNumber(level,3,2,1);
		    		  DrawStatus(1, 21, COMPUTER_PLAYER,status);
		    		  turn=COMPUTER_PLAYER;
		    	  }
		    	  else if(status==CVP) {
			    	  x_cur=7;
			    	  y_cur=7;
			    	  for (i=0; i<256; i++) {
			    	  		board_record[i].x=0;
			    	  	    board_record[i].y=0;
			    	  			  }
				      InitializeGame(x_cur, y_cur);status=PVP;
		    		  DrawStatus(1, 21, EMPTY,status);
		    		  status=PVP;
		    		  DrawStatus(1, 21, COMPUTER_PLAYER,status);
		    	  }
		      }
		      if (RxBuffer==0x76) {
		    	  x_cur=7;
		    	  y_cur=7;
		    	  for (i=0; i<256; i++) {
		    	  		board_record[i].x=0;
		    	  	    board_record[i].y=0;
		    	  			  }
			      InitializeGame(x_cur, y_cur);status=PVP;
		      }
		      if (RxBuffer==0x2D) {
		      	  if(BackTimes>0){
		      		BackTimes--;
		      	  	x_cur=board_record[BackTimes].x;
		      	  	y_cur=board_record[BackTimes].y;
		      	  	board_state[x_cur][y_cur]=EMPTY;
		      	  	DrawChess(x_cur,y_cur,EMPTY);
		      	  	turn=1-turn;
		      	  	if(status==PVC)
		      	  	{
		      	  	BackTimes--;
		      	  	x_cur=board_record[BackTimes].x;
		      	  	y_cur=board_record[BackTimes].y;
		      	  	board_state[x_cur][y_cur]=EMPTY;
		      	  	DrawChess(x_cur,y_cur,EMPTY);
		      	  	turn=HUMAN_PLAYER;
		      	  	}
		      	    DrawBack(3,1119,turn);
		      	  					      }
		      	  				  		  }
	      }
	  }

	  if (turn==COMPUTER_PLAYER && (status==PVC ||status==CVP )&& win_flag==0) {
	      if (step_flag==0) {
	  		  if (x_cur-1<0)
	  			  x_pos=x_cur+1;
	  		  else
	  			  x_pos=x_cur-1;
	  		      y_pos=y_cur;
	  		      step_flag=1;
	      }
	  	  else {
	  		  if(level==2||level==3){
	  		  EvaluateComputerMove(board_state, 0, MIN_INFINITY, MAX_INFINITY, 0, 0);
	  		  x_pos=maxMoveX;
	  		  y_pos=maxMoveY;
	  		  xil_printf("\r\n computer \r\n");}
	  		  else
	  		  {
		  	  everyScore(Computer);
		  	  current_pos=best(Computer);
	  		  x_pos=current_pos.y;
	  		  y_pos=current_pos.x;
	  		  xil_printf("\r\n computer \r\n");
	  		  }

	  	  }
  	      xil_printf("\r\n%x, %x\r\n", x_pos, y_pos);

  	      if(status==CVP)
	      DrawChess(x_pos, y_pos, 1-turn);
  	      else
  	      DrawChess(x_pos, y_pos, turn);


	  	  board_state[x_pos][y_pos]=COMPUTER_PLAYER;
	  	  board_record[BackTimes].x=x_pos;
  		  board_record[BackTimes].y=y_pos;
  		  BackTimes++;

	  	  count=0;
	  	  time0=0;
	      if (CheckWin(x_pos, y_pos, turn)) {
	  	      xil_printf("\r\nComputer Player wins!\r\n");
	  	      win_flag=1;
	  	      DrawWinning(0,1, EMPTY);
	  	      if(status==CVP)
	  	    	  DrawWinning(0,1, 1-turn);
	  	      else
	  	    	  DrawWinning(0,1, turn);
	  	      turn=HUMAN_PLAYER;
	  	  }
	      else {
	    	  turn=HUMAN_PLAYER;
	    	  xil_printf("\r\nHuman Player's turn!\r\n");
	      }
	  }
   }
   return 0;
}

POINT best(int who)                  //找出得分最好的点，对于human是最大的，对于computer找最小
{


    POINT bestPoint;
    int bestValue;
    int i,j;
    POINT bestPoints[226];
    int num=0;                               //最大值的个数

    srand( (unsigned)time( NULL ) );         //初始化随机数种子
    if(who==Human) {
        bestValue=-99999999;

        for(i=0;i<N;i++) {
            for(j=0;j<N;j++){
                if(pointValue[i][j]>bestValue && board_state[i][j]==Nothing){
                    num=0;
                    bestValue=pointValue[i][j];
                    bestPoint.x=j;bestPoint.y=i;         //注意i，j与x,y的对应
                    bestPoints[num++]=bestPoint;
                }
                else if(pointValue[i][j]==bestValue && board_state[i][j]==Nothing){
                    bestPoint.x=j;bestPoint.y=i;
                    bestPoints[num++]=bestPoint;
                }
            }
        }
    }

    if(who==Computer) {

        bestValue=99999999;

        for(i=0;i<N;i++) {
            for(j=0;j<N;j++){
                if(pointValue[i][j]<bestValue && board_state[i][j]==Nothing){
                    num=0;
                    bestValue=pointValue[i][j];
                    bestPoint.x=j;bestPoint.y=i;         //注意i，j与x,y的对应
                    bestPoints[num++]=bestPoint;
                }
                else if(pointValue[i][j]==bestValue && board_state[i][j]==Nothing){
                    bestPoint.x=j;bestPoint.y=i;
                    bestPoints[num++]=bestPoint;
                }
            }
        }
    }

    bestPoint=bestPoints[rand()%num];            //随机选一个




     return bestPoint;
}

void everyScore(int who)         //遍历每个点评分          需知道当前状态，该谁下子
{
    int i,j;
    int vData;
    for(i=0;i<15;i++){
        for(j=0;j<15;j++){
            pointValue[i][j]=0;
        }
    }

    for(i=0;i<15;i++){
        for(j=0;j<15;j++){
            if(board_state[i][j]==Nothing){
            	board_state[i][j]=who;         //如果落子
                pointValue[i][j]=sumall(board_state);           // 算分
                if(jinshou()){
                    if(who==Computer && first==Computer)  pointValue[i][j]=99999999;   //禁手
                    else if(who==Human&& first==Human)  pointValue[i][j]=-99999999;   //禁手
                }
                board_state[i][j]=Nothing;           //恢复
            }
        }
    }

}

int sumall(int data[N][N])                  //对当前状态评分，
{
    int heng,shu,zhengxie,fanxie;          //横，竖，正斜，反斜的总分
    int i,j,k;
    int num=2*N-1-8;     //斜向路数
    int *state[15];

    huo3[Computer]=0;huo3[Human]=0;
    huo4[Computer]=0;huo4[Human]=0;

    for(i=0;i<N;i++){
        state[i]=data[i];
    }
    int line[N];
    heng=0;
    for(i=0;i<N;i++){heng+=(sumline(state[i],N,Human)-sumline(state[i],N,Computer));}

    shu=0;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            line[j]=data[j][i];
        }
        shu+=(sumline(line,N,Human)-sumline(line,N,Computer));
    }


    zhengxie=0;            //     "/"   从n=4 到n=N-5   共num=21路
    for(k=4;k<N;k++){
        j=0;
        for(i=k;i>=0;i--){
            line[j]=data[i][k-i];
            j++;
        }
        zhengxie+=(sumline(line,k+1,Human)-sumline(line,k+1,Computer));
    }
    for(k=N;k<num+4;k++){
        j=0;
        for(i=N-1;i>=0;i--){
            line[j]=data[i][k-i];
            j++;
        }
        zhengxie+=(sumline(line,2*N-1-k,Human)-sumline(line,2*N-1-k,Computer));
    }


    fanxie=0;                 //     "\"   从n=4 到n=N-5   共num=21路
    for(k=4;k<N;k++){
        j=0;
        for(i=N-1-k;i<N;i++){
            line[j]=data[i][j];
            j++;
        }
       fanxie+=(sumline(line,k+1,Human)-sumline(line,k+1,Computer));
    }
    for(k=N;k<num+4;k++){
        j=0;
        for(i=0;i<=2*N-k-2;i++){
            line[j]=data[i][i+k-N+1];
            j++;
        }
        fanxie+=(sumline(line,29-k,Human)-sumline(line,29-k,Computer));
    }



    return heng+shu+zhengxie+fanxie;


}

int sumline(int line[],int length,int who)           //行评分函数   输入 某一行，  该行长度   对哪一方进行评分
{
    int j,k;
    int another;             //另一方
    int side[2];             //左右两边两个是否被堵住，堵住为1，没堵住为0
    int num=0;
    int numof0=0;           //可能链接的最大个数
    int zhong0=0;           //中间空格
    int score=0;               //得分

    if(who==Human) another=Computer;
    else if(who==Computer) another=Human;
    else return -1;


    for(j=0;j<length;j++){
        if(line[j]!=who)continue;
        zhong0=0;
        side[0]=0;side[1]=0;
        num=1;numof0=1;
        if(j==0 || line[j-1]==another)side[0]=1;    //左侧被封堵
        else {
                for(k=j-1;k>=0&&line[k]!=another;k--){
                    numof0++;   //左侧无子
                }
            }

        for(j++;j<length && line[j]==who;j++){
            if(line[j]==who) {num++;numof0++;}
        }
        if(j==length) {side[1]=1;}      //右侧被封堵
        else if(line[j]==Nothing)
        {   side[1]=0;
            for(k=j;k<length && line[k]!=another;k++){
                numof0++;
                if(line[k]==Nothing){
                    if(zhong0==0)zhong0++;
                    else break;
                }
            }
            if(j+1!=length&&line[j+1]==who){num++;j++;
                if(j+1!=length&&line[j+1]==who){num++;j++;
                    if(j+1!=length&&line[j+1]==who){num++;j++;
                    if(j+1!=length&&line[j+1]==who){num++;j++;
                    	if(j+1!=length&&line[j+1]==who){num++;j++;
                    		if(j+1!=length&&line[j+1]==who){num++;j++;
                    	}}
                    }
                }
                }
                }


            else zhong0=0;
        }

        else if(line[j]==another) side[1]=1;      //右侧被封堵

        score+= p_Score(num,numof0,side,who,zhong0);
    }
    return score;
}

int p_Score(int num,int numof0, int bp[],int who,int zhong0)           //算权重分
{
    int max = 0;
    if(numof0<5){return 0;}

    if (num == 5 && zhong0==0)
    {
        return 1000000;   //成5
    }
    else if (num==5 && zhong0!=0){max+=3000;huo4[who]++;}
       else if(num>5 && zhong0==0){

           if(first==who)
               {   changlian=1;
                   return 0;}   //禁手
           else return 1000000;
       }

    else if (num == 4)
    {
        if (bp[1] == 1 && bp[0] == 1) // 两边都被堵
        {
            max += 0;
        }
        else if (bp[1] == 0 && bp[0] == 0) //活四
        {
            max += 3000;
            huo4[who]++;
        }
        else
        {
            max += 900; //冲四
            huo4[who]++;
        }
    }
    else if (num == 3)
    {
        if (bp[0] == 0 && bp[1] == 0)
        {
            max += 460; //活三
            huo3[who]++;
        }
        else if (bp[0] == 1 && bp[1] == 1)
        {
            max += 0; //不成五
        }
        else
        {
            max += 30; //死三

        }
    }
    else if (num == 2)
    {
        if (bp[0] == 0 && bp[1] == 0)
        {
            max += 45;  //活二
        }
        else if (bp[0] == 1 && bp[1] == 1)
        {
            max += 0;
        }
        else             //死二
        {
            max += 5;
        }
    }
    else if (num == 1)
    {
        if (bp[0] == 0 && bp[1] == 0)
        {
            max += 3;
        }
        else if (bp[0] == 1 && bp[1] == 1)
        {
            max += 0;
        }
        else
        {
            max += 1;
        }
    }
    return max;
}
/*
POINT bestboyi(int who)
{
    POINT bestPoint;
    int bestValue;
    int i,j;
    POINT bestPoints[226];
    int num=0;                               //最大值的个数

    srand( (unsigned)time(NULL));         //初始化随机数种子
    if(who==Human) {
        bestValue=-99999999;

        for(i=0;i<N;i++) {
            for(j=0;j<N;j++){
                if(pointValue2[i][j]>bestValue && board_state[i][j]==Nothing){
                    num=0;
                    bestValue=pointValue2[i][j];
                    bestPoint.x=j;bestPoint.y=i;         //注意i，j与x,y的对应
                    bestPoints[num++]=bestPoint;
                }
                else if(pointValue2[i][j]==bestValue && board_state[i][j]==Nothing){
                    bestPoint.x=j;bestPoint.y=i;
                    bestPoints[num++]=bestPoint;
                }
            }
        }
    }

    if(who==Computer) {

        bestValue=99999999;

        for(i=0;i<N;i++) {
            for(j=0;j<N;j++){
                if(pointValue2[i][j]<bestValue && board_state[i][j]==Nothing){
                    num=0;
                    bestValue=pointValue2[i][j];
                    bestPoint.x=j;bestPoint.y=i;         //注意i，j与x,y的对应
                    bestPoints[num++]=bestPoint;
                }
                else if(pointValue2[i][j]==bestValue && board_state[i][j]==Nothing){
                    bestPoint.x=j;bestPoint.y=i;
                    bestPoints[num++]=bestPoint;
                }
            }
        }
    }

    bestPoint=bestPoints[rand()%num];            //随机选一个


     return bestPoint;
}


*/

int jinshou()              //判定禁手          根据first来判定
{
    prehuo3[Computer]=huo3[Computer];prehuo3[Human]=huo3[Human];
    prehuo4[Computer]=huo4[Computer];prehuo4[Human]=huo4[Human];
    sumall(board_state);

    if (huo3[first]-prehuo3[first]>=2)   return 1;
    if (huo4[first]-prehuo4[first]>=2)   return 1;
    if(changlian==1) return 1;
    return 0;
}



