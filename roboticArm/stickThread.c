/*
/*
 * File:   stickThread.c
 * Author: Erik Sarkinen
 *
 * Created on February 21st, 2023, 2:18 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20];  
volatile extern double stickAvg;

void stickThread( void *pvParameters )
{
    int  i = 0, boom = 0;
    int numDelayLoops = 200;
 
    PHASE1 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC1 = 3200;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    while(1)
    {
        sendCharUart2('a');
        intToCharUart2((int)stickAvg);
        for(i = 0; i < 15; i++)
        {
            if(rxval[i] == 'z')
            {
                boom = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                break;
            }
        }
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '&')
            {
                break;
            }
            else if(usbRxval[i] == 'O')
            {
                PDC1--;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC1 < 1700)
                {
                    PDC1 = 1700;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'I')
            {
                PDC1++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC1 > 3500)
                {
                    PDC1 = 3500;        //We don't let PDC2 get greater than 3870
                } 
            }        
        }
    }
}