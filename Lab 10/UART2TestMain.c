// UART2TestMain.c
// Runs on LM3S1968
// Tests the UART1 to implement bidirectional data transfer to and from a
// computer running HyperTerminal.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// October 9, 2011

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

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#ifdef __TI_COMPILER_VERSION__
	//Code Composer Studio Code
	void Delay(unsigned long ulCount){
	__asm (	"    subs    r0, #1\n"
			"    bne     Delay\n"
			"    bx      lr\n");
}
#endif

#include "UART2.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "pll.h"
#include "XBeeOut.h"
#include "SysTick.h"
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
//debug code
int main(void){
  unsigned char i;
  char string[20];  // global to assist in debugging
  unsigned long n;

  //
  // Set the clocking to run at 50MHz from the PLL.
  //
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	PLL_Init();
//	SysTick_Init();
//  UART_Init();              // initialize UART
//  OutCRLF();
	XBeeInit();
	
	while(1){
		
	XBee_sendDataFrame("1234");
		Delay(5500000);
	}
	XBee_sendDataFrame("2");
	
	while(1);
//  for(i='A'; i<='Z'; i=i+1){// print the uppercase alphabet
//    UART_OutChar(i);
//  }
//  OutCRLF();
//  UART_OutChar(' ');
//  for(i='a'; i<='z'; i=i+1){// print the lowercase alphabet
//    UART_OutChar(i);
//  }
//  OutCRLF();
//  UART_OutChar('-');
//  UART_OutChar('-');
//  UART_OutChar('>');
//  while(1){
//    UART_OutString("InString: ");
//    UART_InString(string,19);
//    UART_OutString(" OutString="); UART_OutString(string); OutCRLF();

//    UART_OutString("InUDec: ");  n=UART_InUDec();
//    UART_OutString(" OutUDec="); UART_OutUDec(n); OutCRLF();

//    UART_OutString("InUHex: ");  n=UART_InUHex();
//    UART_OutString(" OutUHex="); UART_OutUHex(n); OutCRLF();

//  }
}
