/*
 * File:   send_char.c
 * Author: eriks
 *
 * Created on Jan 27, 2021, 9:14 AM
 */
#include <xc.h>
#include <p33ep512mc502.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "main.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sendCharUart1(char letter)
{
    U1TXREG = letter;
    while(!U1STAbits.TRMT);
    return;
}
