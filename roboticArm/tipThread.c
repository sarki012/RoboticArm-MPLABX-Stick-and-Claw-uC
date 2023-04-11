/*
 * File:   curlThread.c
 * Author: Erik Sarkinen
 *
 * Created on July 30th, 2022, at 11:05 AM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void tipThread( void *pvParameters )
{
    int  i = 0;
    int numDelayLoops = 500;
 
    PHASE2 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC2 = 3000;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    while(1)
    {
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '^')
            {
                break;
            }
            else if(usbRxval[i] == 't')
            {
                PDC2--;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC2 < 1658)
                {
                    PDC2 = 1658;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'p')
            {
                PDC2++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC2 > 3870)
                {
                    PDC2 = 3870;        //We don't let PDC2 get greater than 3870
                } 
            }        
        }
    }
}