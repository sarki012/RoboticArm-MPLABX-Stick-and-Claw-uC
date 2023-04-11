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
volatile extern unsigned int stickADC;
volatile extern unsigned int tipADC;
volatile extern unsigned int clawADC;

void feedbackThread( void *pvParameters )
{
    while(1)
    {
        sendChar('s');
        sendChar((stickADC >> 8) & 0x00FF);
        sendChar(stickADC & 0x00FF);
        sendChar('t');
        sendChar((tipADC >> 8) & 0x00FF);
        sendChar(tipADC & 0x00FF);
        sendChar('c');
        sendChar((clawADC >> 8) & 0x00FF);
        sendChar(clawADC & 0x00FF);
    }
}
