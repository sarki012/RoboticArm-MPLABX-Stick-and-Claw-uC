// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "FreeRTOSConfig.h"

#define SAMPLE_RATE 2      //Was 1000 Was 20
#define LOOPS 2000     //2000 is good. Was 500. Was 2500. Tried 2000. Too fast. Last try: 2500. Ok but still too fast. Last try 3000
#define LOOPS_CURL 200     //Want the curl to be faster

// Define Message Buffer Length for ECAN1/ECAN2
#define  MAX_CHNUM	 			3		// Highest Analog input number in Channel Scan
#define  SAMP_BUFF_SIZE	 		8		// Size of the input buffer per analog input
#define  NUM_CHS2SCAN			3		// Number of channels enabled for channel scan

void init(void);
void stickThread(void *pvParameters);
void tipThread(void *pvParameters);
void clawThread(void *pvParameters);
void feedbackThread(void *pvParameters);
void delay(int numLoops);
void intToCharUart1(int integer);
void intToCharUart2(int integer2);
void sendCharUart1(char);
void sendCharUart2(char);
int charToInt(char digit3, char digit2, char digit1, char digit0);
void initAdc1(void);
void initTmr3(void);
void initDma0(void);

#endif	/* XC_HEADER_TEMPLATE_H */

