/*
 * File:   send.c
 * Author: eriks
 *
 * Created on Jan 26, 2021, 12:08 AM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"
#include <xc.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void send(char *string)
{
    while(*string)
    {
        int j = 0;
        U1TXREG = *string;
        while(!U1STAbits.TRMT);
        string++;
        for(j = 0; j < 100; j++);
    }
    return;
}
/*
send(int num)
{
    int j = 0, i = 0;
    for(i = 0; i < 4; i++)
    {
        U1TXREG = (unsigned char)(num%10);
        while(!U1STAbits.TRMT);
        num = num >> 4;
        for(j = 0; j < 100; j++);
    }
    return;
}
 * */