//***************FIFO Implementation with critical section****
#define BadFIFOSIZE 32   
#define BadFIFOSUCCESS 1
#define BadFIFOFAIL    0

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
typedef unsigned char BaddataType;
unsigned long static volatile BadSize; /* number of elements in FIFO */
BaddataType static volatile *BadPutPt;    /* Pointer of where to put next */
BaddataType static volatile *BadGetPt;    /* Pointer of where to get next */
BaddataType static BadFifo[BadFIFOSIZE];
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

/*-----------------------BadFifo_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void BadFifo_Init(void){
  BadPutPt=BadGetPt=&BadFifo[0];   // Empty when PutPt=GetPt
  BadSize = 0; 
}


/*-----------------------BadFifo_Put----------------------------
  Enter one character into the fifo
  Inputs: data
  Outputs: true if data is properly saved,
           false if data not saved because it was previously full*/
int BadFifo_Put(BaddataType data){
  if(BadSize==BadFIFOSIZE){
    return BadFIFOFAIL;     // Failed, fifo was previously full
  }
  *(BadPutPt) = data;      // try to Put data into fifo 
  BadPutPt++;              
  if(BadPutPt == &BadFifo[BadFIFOSIZE]){ // need to wrap?
    BadPutPt = &BadFifo[0];
  }

  BadSize++;   // one more element

  return(BadFIFOSUCCESS);
}

/*-----------------------BadFifo_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to return data
  Outputs: true if data is valid, 
           false if fifo was empty at the time of the call*/
int BadFifo_Get(BaddataType *datapt){
	long sr=0;
  if(BadSize == 0){

    return(BadFIFOFAIL);     // Empty if no elements in FIFO 
  }
  *datapt = *(BadGetPt);  // return by reference
  BadGetPt++;             // removes data from fifo
  if(BadGetPt == &BadFifo[BadFIFOSIZE]){ 
    BadGetPt = &BadFifo[0];  // wrap
  }
	sr = StartCritical();                 \
  BadSize--;   // one less element
	EndCritical(sr);
  return(BadFIFOSUCCESS); 
}

//-----------------------BadFifo_Size----------------------------
// Check the status of the FIFO
// Inputs: none
// Outputs: number of elements currently stored
// 0 to FIFOSIZE (0 means empty, FIFOSIZE-1 means full)
unsigned long BadFifo_Size(void){ 
  return BadSize;
}