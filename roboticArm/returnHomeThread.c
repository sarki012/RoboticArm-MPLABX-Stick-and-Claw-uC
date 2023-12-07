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

void returnHomeThread( void *pvParameters )
{
    int  i = 0, direction = 0, stick = 0;
    int numDelayLoops = 1000;       //Was 2000
    PHASE3 = 62500;     //Boom
    
    PHASE1 = 62500;     //Stick
    
    PHASE2 = 62500;
    PDC2 = 15000;
    
    
   
    while(1)
    {
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == 'H')
            {
                if(PDC3 <= 10000)
                {
                    while(PDC3 <= 10000)     //PDC3 = boom
                    {
                        PDC3++;         //Incrementing the duty cycle moves the boom up
                        delay(numDelayLoops);
                    }
                }
                else if(PDC3 > 10000)
                {
                    while(PDC3 > 10000)     //PDC3 = boom
                    {
                        PDC3--;        //Decrementing the duty cycle moves the boom down
                        delay(numDelayLoops);
                    }
                }
                if(PDC1 <= 14000)
                {
                    while(PDC1 <= 14000)        //PDC1 = stick
                    {
                        PDC1++;         //Incrementing the duty cycle moves the stick in
                        delay(numDelayLoops);
                    }  
                }
                else if(PDC1 > 14000)
                {
                    while(PDC1 > 14000)        //PDC1 = stick
                    {
                        PDC1--;         //Decrementing the duty cycle moves the stick out
                        delay(numDelayLoops);
                    }  
                }
             
                if(PDC2 <= 15000)
                {
                    while(PDC2 <= 15000)        //PDC1 = stick
                    {
                        PDC2++;         //Incrementing the duty cycle tips the claw up
                        delay(numDelayLoops);
                    }  
                }
                if(PDC2 > 15000)
                {
                    while(PDC2 > 15000)        //PDC1 = stick
                    {
                        PDC2--;        //Decrementing the duty cycle tips the claw down
                        delay(numDelayLoops);
                    }  
                }
                for(int i = 0; i < 65534; i++);     //Sit here for a while
            }
        }
    }
}
