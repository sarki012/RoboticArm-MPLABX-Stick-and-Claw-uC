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

volatile extern char rxval[20];     //The UART receive array which holds the data sent 
                                    //via Bluetooth from the tablet
void tipThread( void *pvParameters )
{
    int  i = 0;
    int curl = 0;
    int delayPercentage = 0;
    int numDelayLoops = 0;
    int delayReceived = 0;
    PHASE3 = 36850;         //PHASEx is always 36,850 for a 50Hz pulse
    PDC3 = 1500;            //Duty cycle register. Starting duty cycle is 1500.
    while(1)
    {
        for(i = 0; i < 45; i++)
        {
            //This means the next character is a +/- followed by three characters
            //which are passed to charToInt which returns an integer value
            //delayPercentage is a number between 0-100 that will speed up or slow down the boom
            if(rxval[i] == 'd')
            {
                delayPercentage = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                numDelayLoops = delayPercentage;        //Number of delay loops passed to delay() function after each motor update
                delayReceived = 1;      //This means next time we get a 'c' we break from the loop
            }
            else if(rxval[i] == 'c')
            {
                //This means the next character is a +/- followed by three characters
                //which are passed to charToInt which returns an integer value
                //curl holds the pixels in the x-direction from the bottom right joystick
                curl = charToInt(rxval[i+1], rxval[i+2], rxval[i+3], rxval[i+4]);
                if(delayReceived)
                {
                     //We've received stick data and delay data. Break from loop
                    delayReceived = 0;
                    break;
                }
             }
        }
        //Motor Arithmitic Here
        //PHASE3 and PDC3 are for the bucket curl motor
        //With Max Resolution:
        //PHASEx = 36,850
        //Max Duty Cycle is PDC = 4,054
        //Neutral Duty Cycle is PDC = 3,685;
        //Min Duty Cycle is PDC = 1,474
        if(curl > 200)      //We don't touch the motors if the thumb press is in the circle
        {
            PDC3++;         //Incrementing the duty cycle rolls the bucket out
            delay(numDelayLoops);
            if(PDC3 > 4054)
            {
                PDC3 = 4054;        //We don't let PDC1 get greater than 4054
            }

        }
        else if(curl < -200)
        {
            PDC3--;         //Decrementing the duty cycle curls the bucket
            delay(numDelayLoops);
            if(PDC3 < 1474)
            {
                PDC3 = 1474;        //We don't let PDC3 get less than 1,474
            }
        }
    } 
}