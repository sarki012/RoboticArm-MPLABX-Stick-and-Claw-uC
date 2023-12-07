/*
 * File:   boomThread.c
 * Author: Erik Sarkinen
 *
 * Created on September 10th, 2022, 1:20 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20];  
//volatile extern double boom;
//The DMA puts the ADC value in bufferA
volatile extern __eds__ unsigned int bufferA[SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));


void boomThread( void *pvParameters )
{
    int  i = 0, direction = 0, stick = 0;
    int numDelayLoops = 500;       //Was 2000
    PHASE3 = 62500;
    PDC3 = 10000;
    for(int i = 0; i < 10; i++)
    {
        PDC3--;
        delay(100);
    }
    //PHASE3 = 36850;         //Fosc = 120 MHz, Prescaler = 8, PHASE2 = 50,000
    //PDC3 = 2500;           //Min PDC2 = 16,636, Max PDC2 = 31,818
   //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '@')
            {
                break;
            }
            else if(usbRxval[i] == 'd')
            {
                PDC3--;         //Decrementing the duty cycle moves the boom down
                delay(numDelayLoops);
                if(PDC3 < 6777)     //Was 1658
                {
                    PDC3 = 6777;        //We don't let PDC2 get less than 1658
                }
            }
            else if(usbRxval[i] == 'u')
            {
                PDC3++;         //Incrementing the duty cycle moves the boom up
                delay(numDelayLoops);
                if(PDC3 > 15813)
                {
                    PDC3 = 15813;
                }
            }
        }
    }
}
