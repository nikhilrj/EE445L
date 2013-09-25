// PointerTrafficLight.c
// Runs on LM3S1968
// Use a pointer implementation of a Moore finite state machine to operate
// a traffic light.
// Daniel Valvano
// June 15, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011
   Program 3.1, Example 3.1

 Copyright 2011 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// north facing car detector connected to PE1 (1=car present)
// east facing car detector connected to PE0 (1=car present)
// east facing red light connected to PF5
// east facing yellow light connected to PF4
// east facing green light connected to PF3
// north facing red light connected to PF2
// north facing yellow light connected to PF1
// north facing green light connected to PF0

#include "PLL.h"
#include "SysTick.h"

#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002400C)) // bits 1-0
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTF_OUT          (*((volatile unsigned long *)0x400250FC)) // bits 5-0
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control

struct State {
  unsigned long Out;            // 6-bit output
  unsigned long Time;           // 10 ms
  const struct State *Next[5];
	};// depends on 2-bit input

	typedef const struct State STyp;
//#define goN   &FSM[0]
//#define waitN &FSM[1]
//#define goE   &FSM[2]
//#define waitE &FSM[3]

#define slowDelay 100
#define fastDelay 100

#define held1 &MainFSM[0]
#define held2 &MainFSM[1]
#define pressed3 &MainFSM[2]

#define startOscillate &MainFSM[3]
#define oscillate2 &MainFSM[4]
#define oscillate3 &MainFSM[5]
#define oscillate4 &MainFSM[6]
#define oscillate5 &MainFSM[7]
#define oscillate6 &MainFSM[8]
#define oscillate7 &MainFSM[9]
#define oscillate8 &MainFSM[10]
#define oscillate9 &MainFSM[11]
#define oscillate10 &MainFSM[12]
#define oscillate11 &MainFSM[13]
#define oscillate12 &MainFSM[14]
#define oscillate13 &MainFSM[15]
#define oscillate14 &MainFSM[16]
#define oscillate15 &MainFSM[17]
#define oscillate16 &MainFSM[18]
#define oscillate17 &MainFSM[19]
#define init &MainFSM[20]

#define CW 1
#define CCW 2
#define STILL 0

// inputs: 1, 2, 1&2, 3

STyp MainFSM[20]={
	 {CW, slowDelay, { held1, held2, startOscillate, pressed3, init}}, //held1
	 {CCW, slowDelay, { held1, held2, startOscillate, pressed3, init}},//held2
	 {CW, slowDelay, { held1, held2, startOscillate, pressed3, init}}, //pressed 3
		{CW, fastDelay, { held1, held2, oscillate2, pressed3, init}}, // oscillation stage 1
		{CW, fastDelay, { held1, held2, oscillate3, pressed3, init}}, // oscillation stage 2
		{CW, fastDelay, { held1, held2, oscillate4, pressed3, init}}, // oscillation stage 3
		{CW, fastDelay, { held1, held2, oscillate5, pressed3, init}}, // oscillation stage 4
		{CW, fastDelay, { held1, held2, oscillate6, pressed3, init}}, // oscillation stage 5
		{CW, fastDelay, { held1, held2, oscillate7, pressed3, init}}, // oscillation stage 6
		{CW, fastDelay, { held1, held2, oscillate8, pressed3, init}}, // oscillation stage 7
		{CW, fastDelay, { held1, held2, oscillate9, pressed3, init}}, // oscillation stage 8
		{CW, fastDelay, { held1, held2, oscillate10, pressed3, init}}, // oscillation stage 9
		{CW, fastDelay, { held1, held2, oscillate11, pressed3, init}}, // oscillation stage 10
		{CW, fastDelay, { held1, held2, oscillate12, pressed3, init}}, // oscillation stage 11
		{CW, fastDelay, { held1, held2, oscillate13, pressed3, init}}, // oscillation stage 12
		{CW, fastDelay, { held1, held2, oscillate14, pressed3, init}}, // oscillation stage 13
		{CW, fastDelay, { held1, held2, oscillate15, pressed3, init}}, // oscillation stage 14
		{CW, fastDelay, { held1, held2, oscillate16, pressed3, init}}, // oscillation stage 15
		{CW, fastDelay, { held1, held2, startOscillate, pressed3, init}}, // oscillation stage 16
		{STILL, fastDelay, { held1, held2, startOscillate, pressed3, init}}, // Init stage

};

#define motorDelay 100
#define motor5 &MotorFSM[0]
#define motor6 &MotorFSM[1]
#define motor10 &MotorFSM[2]
#define motor9 &MotorFSM[3]

STyp MotorFSM[4]={
	 {0x05, motorDelay, {motor5, motor6, motor9}},
	 {0x06, motorDelay, {motor6, motor10, motor5}},
	 {0x0A, motorDelay, {motor10, motor9, motor6}},
	 {0x09, motorDelay, {motor9, motor5, motor10}}
};

STyp *MotorPt;

void FSMMotor(unsigned long);

int main(void){
  STyp *Pt;  // state pointer
  unsigned long Input;
  PLL_Init();                  // configure for 50 MHz clock
  // activate port F and port E
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF+SYSCTL_RCGC2_GPIOE;
  SysTick_Init();              // initialize SysTick timer
	
	//TODO activate buttons, stepper motor pins, etc.
	
  GPIO_PORTE_DIR_R &= ~0x03;   // make PE1-0 in
  GPIO_PORTE_AFSEL_R &= ~0x03; // disable alt func on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;    // enable digital I/O on PE1-0
  GPIO_PORTF_DIR_R |= 0x3F;    // make PF5-0 out
  GPIO_PORTF_AFSEL_R &= ~0x3F; // disable alt func on PF5-0
  GPIO_PORTF_DEN_R |= 0x3F;    // enable digital I/O on PF5-0
	
	Pt = held1;
	MotorPt = init;
	
  while(1){
		FSMMotor(Pt-> Out); //call FSM motor for output
    SysTick_Wait10ms(Pt->Time);// wait 10 ms * current state's Time value
		//TODO Get input from buttons (0 = 1 pressed, 1 = 2 pressed, 2 = 1,2 pressed, 3 = 3 has been updown, 4 = none pressed)
    Pt = Pt->Next[Input];      // transition to next state
  }
}


//Sub-FSM for stepper motor
//takes in dir (CW, CCW, or STILL) and outputs correct value to stepper
void FSMMotor(unsigned long dir){

	//TODO Output to motor depending on current state
  //No delay, because master FSM takes care of that
	MotorPt = MotorPt->Next[dir]; //depending on dir, transition to next state	
}
