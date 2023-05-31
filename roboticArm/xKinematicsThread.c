/*
 * File:   xDirThread.c
 * Author: Erik Sarkinen
 *
 * Created on May 22nd, 2023, 3:13 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"

#define samples 100

volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
volatile extern char rxval[20];  
//volatile extern double boom;
volatile extern unsigned long timerCount;
volatile extern double x;
volatile extern int xDirection;
volatile extern unsigned long timerCount;
volatile extern int timerDone;
//The DMA puts the ADC value in bufferA, a two-dimensional array
volatile extern __eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

void xKinematicsThread( void *pvParameters )
{
    int  i = 0, j = 0;
    unsigned int boom = 0, stick = 0;
    int numDelayLoops = 150;       //Was 1000 500 2000 1000 500 2000
 //   double x = 4;
    int alpha = 0, alphaFB = 0;
    int beta = 44, betaFB = 0;
    int count = 0;
    
    PHASE1 = 62500;     //Stick
    PDC1 = 14000;
    PHASE3 = 62500;     //Boom
    PDC3 = 13000;
    /*
    PHASE3 = 36850;         //Boom
    PDC3 = 2500;       
    PHASE1 = 36850;         //Stick. PHASEx is always 36,850 for a 50Hz pulse
    PDC1 = 3300;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    */
    //PHASE1 = 62500;
    //PDC1 = 14000;
                  
    while(1)
    {
        boom = bufferA[2][0];       //bufferA holds the ADC values put into SRAM with DMA
        stick = bufferA[0][0];
        alphaFB = (int)((605 - boom)/3 + 25);       //Current boom angle from the horizontal
        betaFB = (int)(stick/3.16 - 63);            //Current angle between the boom and the stick

        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '$')      //Stop motors
            {
                break;
            }
            else if(usbRxval[i] == 'l' || usbRxval[i] == 'r')     //'l' for out, 'r' for in
            {
                j = 0;
                alpha = 180*(acos(sqrt((72 - 72*cos(beta*3.1416/180)))/12))/3.1416;
                while((betaFB < beta) && (alpha < alphaFB))     //Move the claw out
                {
                    boom = bufferA[2][j];           //Feedback value from ADC
                    stick = bufferA[0][j];          //Feedback value from ADC
                    j++;
                    if(j >= 8)      //The feedback buffer is 8 unsigned integers
                    {
                        j = 0;
                    }
                    alphaFB = (int)((605 - boom)/3 + 25);       //Converts the feedback value from 0-1024 to degrees
                    betaFB = (int)(stick/3.16 - 63);            //Converts the feedback value from 0-1024 to degrees
                    if(betaFB < beta)       //Only spin the stick motor if the current angle (betaFB) is less than the target (beta)
                    {
                        PDC1--;         //Decrementing PDC1 moves the stick out
                    }
                    if(alpha < alphaFB)
                    {
                        
                        if(count == 1)
                        {
                            count = 0;
                            PDC3--;         //Decrementing PDC2 moves the boom down
                        }
                        count++;
                    }
                    delay(numDelayLoops);
                    if(PDC1 < 6777)     //Was 1658. 6777 for 120 Mhz with 16 prescalar 
                    {
                        PDC1 = 6777;        //We don't let PDC2 get less than 1658
                    }
                    if(PDC3 < 6777)     //6777 for 120 Mhz with 16 prescalar 
                    {
                        PDC3 = 6777;        //We don't let PDC2 get less than 1658
                    }
                }
                j = 0;
                while((beta < betaFB) && (alphaFB < alpha))
                {
                    boom = bufferA[2][j];
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    alphaFB = (int)((605 - boom)/3 + 25);
                    betaFB = (int)(stick/3.16 - 63);
                    if(beta < betaFB)
                    {
                        PDC1++;         //Incrementing PDC1 moves the stick in
                    }
                    
                    if(alphaFB < alpha)
                    {
                        if(count == 1)
                        {
                            count = 0;
                            PDC3++;         //Incrementing PDC2 moves the boom up
                        }
                         count++;
                    }
                    
                    delay(numDelayLoops);
                    if(PDC1 > 15813)     //Was 3870. 6777 for 120 Mhz with 16 prescalar 
                    {
                        PDC1 = 15813;        //We don't let PDC2 get less than 1658
                    }
                    if(PDC3 > 15813)     //6777 for 120 Mhz with 16 prescalar 
                    {
                        PDC3 = 15813;        //We don't let PDC2 get less than 1658
                    }
                }
                if(usbRxval[i] == 'l')
                {
                    beta += 10;
                    if(beta >= 122)
                    {
                        beta = 122;
                    }
                }
                else if(usbRxval[i] == 'r')
                {
                    beta -= 10;
                    if(beta <= 44)
                    {
                        beta = 44;
                    }
                }
            }
        }
    }
}

