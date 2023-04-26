/*
 * File:   .c
 * Author: Erik Sarkinen
 *
 * Created on March 29th, 2023 2:22 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

#define bufferSize 50
volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
//volatile extern unsigned int BufferA[MAX_CHNUM][SAMP_BUFF_SIZE];
volatile extern __eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));
//volatile extern __eds__ unsigned int bufferB[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

void feedbackThread( void *pvParameters )
{
    int i = 0, count = 0;
    unsigned long int stickAvgBuffer = 0, tipAvgBuffer = 0, clawAvgBuffer = 0;
    double stickAvg = 0, tipAvg = 0, clawAvg = 0;
    while(1)
    {
        //sendChar('s');
        //intToChar(bufferA[0][i]);
        stickAvgBuffer += bufferA[0][0];
        tipAvgBuffer += bufferA[1][0];
        clawAvgBuffer += bufferA[2][0];
        
        if(count == bufferSize)
        {
            stickAvg = stickAvgBuffer/bufferSize;
            sendChar('s');
            intToChar((int)stickAvg);
            stickAvgBuffer = 0;
            delay(100);
            tipAvg = tipAvgBuffer/bufferSize;
            sendChar('t');
            intToChar((int)tipAvg);
            tipAvgBuffer = 0;
            delay(100);
            clawAvg = clawAvgBuffer/bufferSize;
            sendChar('c');
            intToChar((int)clawAvg);
            clawAvgBuffer = 0;
            delay(100);
            count = 0;
        }
        count++;
       // sendChar('c');
        //intToChar(bufferA[2][i]);
        i+=2;       //Increment by 2 because data is in words
        if(i > 8)
        {
            i = 0;
        }
        
    }
}
