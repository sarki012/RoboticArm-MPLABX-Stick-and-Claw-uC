/*
 * File:   boomThread.c
 * Author: Erik Sarkinen
 *
 * Created on April 29, 2022, 4:43 PM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

volatile extern char rxval[50];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void clawThread( void *pvParameters )
{
    int  i = 0;
    int boom = 0;
    int m = 0;
    int delayPercentage = 0;
    int numDelayLoops = 0;
    int delayReceived = 0;
    PHASE1 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC1 = 2500;            //Duty cycle register. Starting duty cycle is 2500.
    while(1)
    {
      for(i = 0; i < 45; i++)
        {
            if(rxval[i] == 'd')
            {
                //This means the next character is a +/- followed by three characters
                //which are passed to charToInt which returns an integer value
                //delayPercentage is a number between 0-100 that will speed up or slow down the boom
                delayPercentage = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                numDelayLoops = 1 + delayPercentage*2;        //Number of delay loops passed to delay() function after each motor update
                //We've received stick data and delay data. Break from loop
                delayReceived = 1;
            }
            else if(rxval[i] == 'b')
            {
                //This means the next character is a +/- followed by three characters
                //which are passed to charToInt which returns an integer value
                //boom holds the pixels in the y-direction from the bottom right joystick
                boom = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                if(delayReceived)
                {
                    delayReceived = 0;      //We've received stick data and delay data. Break from loop
                    break;
                }
             }
        }
        //Motor Arithmitic Here
        //PHASE1 and PDC1 are for PWM1L, the boom motor
        //With Max Resolution:
        //PHASEx = 36,850
        //Max Duty Cycle is PDC = 4,054
        //Neutral Duty Cycle is PDC = 3,685;
        //Min Duty Cycle is PDC = 1,474
        if(boom > 200)      //We don't touch the motors if the thumb press is in the circle
        {              
            PDC1--;         //Decrementing the duty cycle moves the stick out
            delay(numDelayLoops);
            if(PDC1 < 1474)
            {
                PDC1 = 1474;        //We don't let PDC1 get less than 1,474
            }
        }
        else if(boom < -200)
        {
            PDC1++;         //Incrementing the duty cycle moves the stick in
            delay(numDelayLoops);
            if(PDC1 > 4054)
            {
                PDC1 = 4054;        //We don't let PDC1 get greater than 4054
            }
        }   
    }
}    