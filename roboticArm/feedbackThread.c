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

#define bufferSize 8
volatile extern char usbRxval[20];     //The UART receive array which holds the data sent 

//The DMA puts the ADC value in bufferA, a two-dimensional array
volatile extern __eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));   
volatile extern double stick, tip, boom;

void feedbackThread( void *pvParameters )
{
    int i = 0, count = 0;
    unsigned long int boomAvgBuffer = 0, stickAvgBuffer = 0, tipAvgBuffer = 0;
    while(1)
    {  
        stickAvgBuffer += bufferA[0][i];    //Two dimensional array: Row [0] is the stick ADC
        tipAvgBuffer += bufferA[1][i];      //Two dimensional array: Row [1] is the tip ADC
        boomAvgBuffer += bufferA[2][i];     //Two dimensional array: Row [2] is the claw ADC
        delay(1000);
        count++;
        //Only going to send the average every bufferSize loops of the thread
        if(count == bufferSize)
        {
            boom = boomAvgBuffer/bufferSize;
            sendCharUart1('b');                  //'c' for claw
            intToCharUart1((int)boom);
            boomAvgBuffer = 0;
            stick = ((stickAvgBuffer/bufferSize)/3.16) - 63;       //Average
            sendCharUart1('s');      //Send 's' for stick
            intToCharUart1((int)stick);       //intToChar converts the integer to four characters and sends them out on UART1
            stickAvgBuffer = 0;
            tip = tipAvgBuffer/bufferSize;
            sendCharUart1('t');                  //'t' for tip
            intToCharUart1((int)tip);
            tipAvgBuffer = 0;
            count = 0;
        }
        
        i++;       //Increment by 2 because data is in words
        if(i >= 8)
        {
            i = 0;
        }
        /*
        if(count == bufferSize)
        {
            stickAvg = stickAvgBuffer/bufferSize;       //Average
            sendChar('s');      //Send 's' for stick
            intToChar((int)stickAvg);       //intToChar converts the integer to four characters and sends them out on UART1
            stickAvgBuffer = 0;
            //delay(100);
            tipAvg = tipAvgBuffer/bufferSize;
            sendChar('t');                  //'t' for tip
            intToChar((int)tipAvg);
            tipAvgBuffer = 0;
          //  delay(100);
            clawAvg = clawAvgBuffer/bufferSize;
            sendChar('c');                  //'c' for claw
            intToChar((int)clawAvg);
            clawAvgBuffer = 0;
        //    delay(100); 
            count = 0;
         
        }
         */ 
    }
}
