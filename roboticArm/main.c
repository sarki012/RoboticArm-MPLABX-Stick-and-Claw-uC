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
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

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
int y = 0, z = 0;
volatile double x = 4.5;
volatile int xDirection = -1;
volatile double stick = 0, tip = 0, claw = 0;
volatile unsigned long timerCount = 0;
volatile int timerDone = 0;
//Raspberry Pi USB to UART1 receive interrupt
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)             
{
    IFS0bits.U1RXIF = 0;        //Clear the interrupt flag
    usbRxval[z] = U1RXREG;         //Add the value in the receive register to the receive array
    z++;
    if(z == 20)
    {  
        z = 0;
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

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void)
{
    if(IFS0bits.T2IF == 1)							// Timer2 causes interrupt.....?
    {
	   IFS0bits.T2IF = 0; 							//Reset Timer1 interrupt flag
       timerCount++;
       if(timerCount == 1000)       //Increment x every second
       {
           timerCount = 0;
           timerDone = 1;
           /*
           if(xDirection == -1)
           {
               return;
           }
           else if(xDirection == 1)
           {
               x += .5;
               if(x >= 10)
               {
                   x = 10;
               }
           }
            else if(xDirection == 0)
           {
               x -= .5;
               if(x <= 4)
               {
                   x = 4;
               }
           }
           return;
            */ 
       }
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
    
    xTaskCreate( stickThread, "Stick", 256, NULL, 1, NULL );    //Thread that controls the stick
	xTaskCreate( tipThread, "Tip", 256, NULL, 1, NULL );      //Thread that controls the tip motion
    xTaskCreate( clawThread, "Claw", 256, NULL, 1, NULL );      //Thread that controls the tip motion
    xTaskCreate( feedbackThread, "Feedback", 256, NULL, 1, NULL );      //Thread that sends the feedback values
    xTaskCreate( stickXDirThread, "Horizontal", 256, NULL, 1, NULL );        //Thread that controls horizontal kinematics
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