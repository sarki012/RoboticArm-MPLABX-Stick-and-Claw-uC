/*
 * File:   main.c
 * Author: Erik Sarkinen
 *
 * Created on May 5, 2022, 1:46 PM
 */
// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PWMLOCK = OFF            // PWM Lock Enable bit (PWM registers may be written without key sequence)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

#include <xc.h>
#include <p33ep512mc502.h>
/* Standard includes. */
#include <stdio.h>
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "FreeRTOSConfig.h"
#include "main.h"


volatile char usbRxval[20];     //The UART receive array which holds the data sent 
                                //via USB from the Raspberry Pi
volatile char rxval[20];    
int x = 0, y = 0;
volatile double stickAvg = 0, tipAvg = 0, clawAvg = 0;

//Raspberry Pi USB to UART1 receive interrupt
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)             
{
    IFS0bits.U1RXIF = 0;        //Clear the interrupt flag
    usbRxval[x] = U1RXREG;         //Add the value in the receive register to the receive array
    x++;
    if(x == 20)
    {  
        x = 0;
    }
    return;
 }

//uC1 to uC2 UART2 receive interrupt
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)             
{
    IFS1bits.U2RXIF = 0;        //Clear the interrupt flag
    rxval[y] = U2RXREG;         //Add the value in the receive register to the receive array
    y++;
    if(y == 20)
    {  
        y = 0;
    }
    return;
 }

void __attribute__((__interrupt__, auto_psv)) _DefaultInterrupt(void)
{
    if(IFS0bits.AD1IF)
    {
       return;
    }
    else
         return;
    return;
}

void main(void) {
    int i = 0;
    for(i = 0; i < 20; i++)
    {
        usbRxval[i] = 0;
        rxval[i] = 0;       //Initialize the receive array to all 0's
    }
    init();     //Setup clock, UART, and PWMs
    initAdc1();
    initTmr3();
    initDma0();
    //DMA0STAL = __builtin_dmaoffset(stickBuffer); //DMA0 start address low byte 
    xTaskCreate( stickThread, "Stick", 512, NULL, 1, NULL );    //Thread that controls the stick
	xTaskCreate( tipThread, "Tip", 512, NULL, 1, NULL );      //Thread that controls the tip motion
    xTaskCreate( clawThread, "Claw", 512, NULL, 1, NULL );      //Thread that controls the tip motion
    xTaskCreate( feedbackThread, "Feedback", 512, NULL, 1, NULL );      //Thread that sends the feedback values
    
	//Start the scheduler
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
    return;
}
void vApplicationIdleHook( void )
{
	/* Schedule the co-routines from within the idle task hook. */
	vCoRoutineSchedule();
}