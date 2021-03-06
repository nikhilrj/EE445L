#include "globals.h"
#include "RIT128x96x4.h"
#include "OLEDdraw.h"
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value


float coslut[60] = {
	0.000, 0.105, 0.208, 0.309, 0.407, 0.500, 0.588, 0.669, 0.743, 0.809, 0.866, 0.914, 0.951, 0.978, 0.995, 1.000, 0.995, 0.978, 0.951, 0.914, 0.866, 0.809, 0.743, 0.669, 0.588, 0.500, 0.407, 0.309, 0.208, 0.105, 0.000, -0.105, -0.208, -0.309, -0.407, -0.500, -0.588, -0.669, -0.743, -0.809, -0.866, -0.914, -0.951, -0.978, -0.995, -1.000, -0.995, -0.978, -0.951, -0.914, -0.866, -0.809, -0.743, -0.669, -0.588, -0.500, -0.407, -0.309, -0.208, -0.105
};

float sinlut[60] = {
	-1.000, -0.995, -0.978, -0.951, -0.914, -0.866, -0.809, -0.743, -0.669, -0.588, -0.500, -0.407, -0.309, -0.208, -0.105, 0.000, 0.105, 0.208, 0.309, 0.407, 0.500, 0.588, 0.669, 0.743, 0.809, 0.866, 0.914, 0.951, 0.978, 0.995, 1.000, 0.995, 0.978, 0.951, 0.914, 0.866, 0.809, 0.743, 0.669, 0.588, 0.500, 0.407, 0.309, 0.208, 0.105, 0.000, -0.105, -0.208, -0.309, -0.407, -0.500, -0.588, -0.669, -0.743, -0.809, -0.866, -0.914, -0.951, -0.978, -0.995
};

void drawCircle(int x, int y, int radius);

int miniteRadius = 35;
int secondRadius = 40;
int hourRadius = 25;
unsigned char color  = 15;

void analogClockDraw(){
	volatile unsigned long delay;
  volatile unsigned long elapsed = 0;
	int x_s, y_s, x_m, y_m, x_h, y_h;

	DisableInterrupts();

	// disable SysTick during setup
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
	delay = NVIC_ST_CURRENT_R ;
	
	 x_s = (int)(coslut[seconds]*secondRadius + 64);
	 y_s = (int)(sinlut[seconds]*secondRadius + 48);
	 x_m = (int)(coslut[minutes]*miniteRadius + 64);
	 y_m = (int)(sinlut[minutes]*miniteRadius + 48);
	 x_h = (int)(coslut[hours*5 + minutes/12]*hourRadius + 64);
	 y_h = (int)(sinlut[hours*5 + minutes/12]*hourRadius + 48);
		
	RIT128x96x4_ClearImage();
	drawCircle(64, 48, 45);
	

	//draws clock hands
	RIT128x96x4_Line(64, 48, x_s, y_s, color);
	RIT128x96x4_Line(64, 48, x_m, y_m, color);
	RIT128x96x4_Line(64, 48, x_h, y_h, color);
	RIT128x96x4_ShowImage();
	RIT128x96x4StringDraw((hours24>=12)?"PM":"AM", 110, 80, color);
	RIT128x96x4StringDraw("Alarm", 0, 76, color);
	RIT128x96x4StringDraw((alarmActive)?"On":"Off", 0, 85, color);

	RIT128x96x4StringDraw("12", 58, 5, color);
	RIT128x96x4StringDraw("3", 100, 48, color);
	RIT128x96x4StringDraw("9", 23, 48, color);
	RIT128x96x4StringDraw("6", 62, 80, color);
	elapsed = 1;
	elapsed = NVIC_ST_CURRENT_R - delay;
	delay = 0;
	EnableInterrupts();
}

//hh:mm:ss
void digitalClockDraw(){
		drawDigitalValue(hours, minutes, seconds);
}

void drawDigitalValue(unsigned int h, unsigned int m, unsigned int s){
	char time[20];	
	sprintf(time, "%02d:%02d:%02d %s\n", (h%12==0)?12:h%12, m, s, (h>=12)?"PM":"AM");
 	RIT128x96x4StringDraw(time, 30, 44, color);
}

void drawInactiveTimer(){
	char time[20];	
	sprintf(time, "Returning in...%d  ", 10-inacTimer);
 	RIT128x96x4StringDraw(time,  10, 80, color);
}


void timerDraw(){
	char time[20];	
	sprintf(time, "   %02d:%02d   ", timerMin, timerSec);
 	RIT128x96x4StringDraw(time, 30, 44, color);
}

void countdownDraw(){
	char time[20];	
	sprintf(time, "   %02d:%02d   ", countMin, countSec);
 	RIT128x96x4StringDraw(time, 30, 44, color);
}

void metronomeDraw(){
	char time[20];
	sprintf(time, "   %d   ", bpm);
	RIT128x96x4StringDraw(time, 30, 44, color);
}


//draws circle with radius r centered at (x,y)
void drawCircle(int x, int y, int r) {
	int i;
	int x_1 = r*coslut[0]+x;
	int y_1 = r*sinlut[0]+y;

	for(i = 1; i < 61; i++){
		int x_2 = r*coslut[i%60]+x;
		int y_2 = r*sinlut[i%60]+y;
		RIT128x96x4_Line(x_1, y_1, x_2, y_2, color);
		x_1 = x_2;
		y_1 = y_2;
	}
}