/*
 * File:   stickThread.c
 * Author: Erik Sarkinen
 *
 * Created on February 21st, 2023, 2:18 PM
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
//volatile extern double stick;
volatile extern unsigned long timerCount;
//The DMA puts the ADC value in bufferA, a two-dimensional array
volatile extern __eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

void stickThread( void *pvParameters )
{
    int  i = 0, j = 0, boom = 0, count = 0;
    unsigned int stick = 0;
    int numDelayLoops = 1000;        //was 750 200
    double x = 6;
    int beta = 0, betaFB = 0;
 
    PHASE1 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC1 = 3200;            //Duty cycle register. Starting duty cycle is x. Max + PDCx = 1658, max - PDCx = 3870
    
    while(1)
    {
        stick = bufferA[0][0];
        sendCharUart2('a');
        intToCharUart2((int)stick);
        sendCharUart2('x');
        intToCharUart2((int)(x*100));
        for(i = 0; i < 15; i++)
        {
            
            if(rxval[i] == 'z')
            {
                boom = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                break;
            }
              
        }
        beta = (int)((acos((72 - x*x)/72))*180/3.1416);
     //   betaFB = ((stick/3)*3.1416)/180 - 1.1;
        betaFB = (int)(stick/3 - 63);
        for(i = 0; i < 20; i++)
        {
            if(usbRxval[i] == '&')
            {
                break;
            }
            else if(usbRxval[i] == 'O')
            {
                j = 0;
                beta = (int)((acos((72 - x*x)/72))*180/3.1416);
                //beta = acos((72 - x*x)/72);
                while(betaFB < beta)
                {
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                   // betaFB = ((stick/3)*3.1416)/180 - 1.1;
                    betaFB = (int)(stick/3 - 63);
                    PDC1--;     //Decrementing PDC1 moves the stick out
                    delay(numDelayLoops);
                    if(PDC1 < 1700)
                    {
                        PDC1 = 1700;        //We don't let PDC2 get less than 1658
                        break;
                    }
                    
                }
                j = 0;
                while(betaFB > beta)
                {
                   // betaFB = ((stick/3)*3.1416)/180 - 1.1;
                    stick = bufferA[0][j];
                    j++;
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    betaFB = (int)(stick/3 - 63);
                    PDC1++;         //Incrementing the duty cycle moves the stick in
                    delay(numDelayLoops);
                    if(PDC1 > 3500)
                    {
                        PDC1 = 3500;        //We don't let PDC2 get greater than 3870
                        break;
                    }            
                }
                x += .5;
                if(x > 12)
                {
                    x = 12;
                }
                timerCount = 0;
                while(timerCount < 500);
                timerCount = 0;
                /*
                if(count == 100)
                {
                    x += .1;
                    if(x > 12)
                    {
                        x = 12;
                    }
                    count = 0;
                }
                 */ 
               // count++;
              //  if()
                /*
                PDC1--;         //Decrementing the duty cycle moves the stick out
                delay(numDelayLoops);
                if(PDC1 < 1700)
                {
                    PDC1 = 1700;        //We don't let PDC2 get less than 1658
                }
                 */ 
            }
            else if(usbRxval[i] == 'I')
            {
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
                   // betaFB = ((stick/3)*3.1416)/180 - 1.1;
                    betaFB = (int)(stick/3 - 63);
                    PDC1--;     //Decrementing PDC1 moves the stick out
                    delay(numDelayLoops);
                    if(PDC1 < 1700)
                    {
                        PDC1 = 1700;        //We don't let PDC2 get less than 1658
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
                  //  betaFB = ((stick/3)*3.1416)/180 - 1.1;      //Convert to degrees so while loop compares integers
                    betaFB = (int)(stick/3 - 63);
                    PDC1++;         //Incrementing the duty cycle moves the stick in
                    delay(numDelayLoops);
                    if(PDC1 > 3500)
                    {
                        PDC1 = 3500;        //We don't let PDC2 get greater than 3870
                        break;
                    } 
                }
                x -= .5;    //decrement by .1 every 10 ms
                if(x < 6)
                {
                    x = 6;
                }
                timerCount = 0;
                while(timerCount < 500);        //100 ms
                timerCount = 0;
                /*
                if(count == 100)
                {
                    x -= .1;
                    if(x < 6)
                    {
                        x = 6;
                    }
                    count = 0;
                }
                count++;
                 * */
                /*
                if(betaFB == beta)
                {
                    break;
                }
                 * */
                /*
                PDC1++;         //Incrementing the duty cycle moves the stick in
                delay(numDelayLoops);
                if(PDC1 > 3500)
                {
                    PDC1 = 3500;        //We don't let PDC2 get greater than 3870
                } 
                 */ 
            }        
        }
    }
}