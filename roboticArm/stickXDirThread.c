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

void stickXDirThread( void *pvParameters )
{
    int  i = 0, j = 0, boom = 0;
    unsigned int stick = 0;
    int numDelayLoops = 1000;       //Was 500 2000 1000 500 2000
 //   double x = 4;
    int beta = 0, betaFB = 0;
    
    PHASE1 = 36850;
    PDC1 = 3300;
    
    //PHASE1 = 62500;
    //PDC1 = 14000;
    
    //PHASE2 = 6142;
   // PDC2 = 3906;
   // PHASE2 = 24567;         //24,567 for a 300 Hz Pulse with divide by 1 prescaler. PHASEx is always 36,850 for a 50Hz pulse
    //PDC2 = 13000;            //With divide by 1 prescalar min duty cyle = 6,700, max = 15,634
    //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870                   
   
    while(1)
    {
        stick = bufferA[0][0];
        sendCharUart2('a');
        intToCharUart2((int)stick);
        sendCharUart2('x');
        intToCharUart2((int)(x*10));
        for(i = 0; i < 15; i++)
        {
            if(rxval[i] == 'z')
            {
                boom = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
            }
        }
        beta = (int)((acos((72 - x*x)/72))*180/3.1416);
        betaFB = (int)(stick/3.16 - 63);
      
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '$')      //Stop motors
            {
                xDirection = -1;
                break;
            }
            else if(usbRxval[i] == 'l')     //'l' for out
            {
            //   timerCount = 0;
              //  xDirection = 1;
                sendCharUart2('x');      //Send 'x' for x-direction
                intToCharUart2((int)(x*10));       //intToChar converts the integer to four characters and sends them out on UART1
                j = 0;
                beta = (int)((acos((72 - x*x)/72))*180/3.1416);
                while(betaFB < beta)
                {
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    betaFB = (int)(stick/3.16 - 63);
                    PDC1--;         //Decrementing PDC1 moves the stick out
                    delay(numDelayLoops);
                    if(PDC1 < 1658)     //6777 for 120 Mhz with 16 prescalar 
                    {
                        PDC1 = 1658;        //We don't let PDC2 get less than 1658
                    }
                }
                j = 0;
                while(betaFB < beta)
                {
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    betaFB = (int)(stick/3.16 - 63);
                    PDC1++;         //Incrementing the duty cycle moves the stick in
                    delay(numDelayLoops);
                    if(PDC1 > 3870)        //15813 for 120 Mhz with 16 prescalar
                    {
                        PDC1 = 3870;        //We don't let PDC2 get greater than 3870
                    } 
                }
                x += .5;
                if(x >= 10 )
                {
                    x = 10;
                }
                timerCount = 0;
                while(!timerDone);
                timerDone = 0;
                timerCount = 0;
                break;
            }
            else if(usbRxval[i] == 'r')         //I for In
            {
                timerCount = 0;
                xDirection = 0;
                sendCharUart2('x');      //Send 'x' for x-direction
                intToCharUart2((int)(x*10));       //intToChar converts the integer to four characters and sends them out on UART1
                j = 0;
                beta = (int)((acos((72 - x*x)/72))*180/3.1416);
                while(betaFB < beta)        //Make a window +/- 5, not =. 
                {
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    betaFB = (int)(stick/3.16 - 63);
                    PDC1--;         //Decrementing PDC1 moves the stick out
                    delay(numDelayLoops);
                    if(PDC1 < 1658)
                    {
                        PDC1 = 1658;        //We don't let PDC2 get less than 1658
                    }
                }
                j = 0;
                while(betaFB > beta)
                {
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    betaFB = (int)(stick/3 - 63);
                    PDC1++;         //Incrementing the duty cycle moves the stick in
                    delay(numDelayLoops);
                    if(PDC1 > 3870)
                    {
                        PDC1 = 3870;        //We don't let PDC2 get greater than 3870
                    }             
                }
                x -= .5;    //decrement by .1 every 10 ms
                if(x < 4)
                {
                    x = 4;
                }
                timerCount = 0;
                while(!timerDone);        //1000 ms
                timerDone = 0;
                timerCount = 0;
                break;
            }
        }
    }
}

