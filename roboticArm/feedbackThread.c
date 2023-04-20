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

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
//volatile extern unsigned int BufferA[MAX_CHNUM][SAMP_BUFF_SIZE];
volatile extern __eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));
//volatile extern __eds__ unsigned int bufferB[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

void feedbackThread( void *pvParameters )
{
    int i = 0;
    while(1)
    {
        sendChar('s');
        intToChar(bufferA[0][i]);
        sendChar('t');
        intToChar(bufferA[1][i]);
        sendChar('c');
        intToChar(bufferA[2][i]);
        i+=2;       //Increment by 2 because data is in words
        if(i > 8)
        {
            i = 0;
        }
        
    }
}
