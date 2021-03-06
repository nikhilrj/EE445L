// ADCTestmain.c
// Runs on LM3S811
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.
// Daniel Valvano
// June 30, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011

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

// Thumbwheel potentiometer with scaling resistor connected to ADC0

#include "ADCT0ATrigger.h"
#include <stdio.h>
#include "Output.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode



//debug code
//
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
#define GPIO_PORTC_DATA_R       (*((volatile unsigned long *)0x400063FC))
#define GPIO_PORTC_DIR_R        (*((volatile unsigned long *)0x40006400))
#define GPIO_PORTC_DEN_R        (*((volatile unsigned long *)0x4000651C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOC      0x00000004  // port C Clock Gating Control


//the ADC can be triggered three ways
//1) a timer can be set to automatically start the process then generate an interrupt when done
//2) use sampling events to trigger samples (can use other things than timer)
//3) manually start a sample by setting proper register values

#include "temperature.h"
#include "ADCT0ATrigger.h"
#include "fixed.h"
#include "rit128x96x4.h"
//#include "hw_types.h"
//#include "sysctl.h"
#include "lm3s1968.h"
#include "pll.h"

unsigned long temperature; //100 * temperature, in C
unsigned long offset = 0;


char string [10];
char string2[10];
int main(void){
	unsigned long calibratedValue = 0;
	PLL_Init();
//	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | 
//	SYSCTL_XTAL_8MHZ); // 50 MHz 
	DisableInterrupts();
	Output_Init(); 
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;    // activate port C
	
  ADC_InitTimer0ATriggerSeq3(0, 9, 29999); // ADC channel 0, 20 Hz sampling
  GPIO_PORTC_DIR_R |= 0x20;                // make PC5 out (PC5 built-in LED)
  GPIO_PORTC_DEN_R |= 0x20;                // enable digital I/O on PC5 (default setting)
		EnableInterrupts();

	
  while(1){
    GPIO_PORTC_DATA_R ^= 0x20;           // toggle LED
		temperature = convertToTemperature(ADCvalue);
		calibratedValue = ADCvalue + offset;
		Fixed_uDecOut2s(temperature, string);
		RIT128x96x4StringDraw(string, 0,
                      0, 15);
		Fixed_uDecOut2s(ADCvalue, string2);

  }
}
