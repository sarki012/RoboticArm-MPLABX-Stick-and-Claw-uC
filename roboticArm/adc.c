/*
 * File:   adc.c
 * Author: eriks
 *
 * Created on April 11th, 2023, 3:54 PM
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

//Create DMA buffer for ADC, the macro is required because the buffer is
//outside of the normal memory space
__eds__ unsigned int bufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((eds,aligned(128)));

/*=============================================================================
ADC Initialisation for Channel Scan 
=============================================================================*/
void initAdc1(void)
{
    ///////////////A/D////////////////////////////////
    TRISAbits.TRISA0 = 1;       //Input
    TRISAbits.TRISA1 = 1;       //Input
    TRISBbits.TRISB0 = 1;       //Input
    
    AD1CON1bits.ADON = 0;   //ADC is disabled before changing registers
    AD1CON1bits.ADSIDL = 0;  //Continue operation in idle mode
    AD1CON1bits.FORM   = 0;		// Data Output Format: Unsigned Integer
	//AD1CON1bits.FORM   = 2;		// Data Output Format: Fractional
    AD1CON1bits.SSRCG = 0;
	AD1CON1bits.SSRC   = 2;		// Sample Clock Source: GP Timer starts conversion
	AD1CON1bits.ASAM   = 1;		// ADC Sample Control: Sampling begins immediately after conversion
	AD1CON1bits.AD12B  = 0;		// 10-bit ADC operation
	

	AD1CON2bits.CSCNA = 1;		// Scan Input Selections for CH0+ during Sample A bit
	AD1CON2bits.CHPS  = 0;		// Converts CH0
    AD1CON2bits.VCFG = 7;       //Voltage reference AVDD and AVSS
    AD1CON2bits.BUFM = 0;       //Always starts filling the buffer from the start address
    AD1CON2bits.ALTS = 0;       //Always uses channel input selects for sample A
	AD1CON3bits.ADRC = 0;		// ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS = 63;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
								// ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us	

	AD1CON1bits.ADDMABM = 0; 	// DMA buffers are built in scatter/gather mode
	AD1CON4bits.DMABL   = 3;	// Each buffer contains 8 words
    AD1CON4bits.ADDMAEN = 1;       //DMA ADC1BUF0 is the only ADC buffer used
	AD1CON2bits.SMPI    = (NUM_CHS2SCAN-1);	// 3 ADC Channel is scanned
	//AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
	AD1CSSH = 0x0000;			
	AD1CSSLbits.CSS0 = 1;			// Enable AN0 for channel scan
	AD1CSSLbits.CSS1 = 1;			// Enable AN1 for channel scan
	AD1CSSLbits.CSS2 = 1;		// Enable AN2 for channel scan
	
	IFS0bits.AD1IF   = 0;		// Clear the A/D interrupt flag bit
	IEC0bits.AD1IE   = 0;		// Do Not Enable A/D interrupt 
	AD1CON1bits.ADON = 1;		// Turn on the A/D converter
    return;
}

/*=============================================================================  
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us. 
=============================================================================*/
void initTmr3(void) 
{
	TMR3 = 0x0000;
	PR3  = 4999;			// Trigger ADC1 every 125usec
	IFS0bits.T3IF = 0;		// Clear Timer 3 interrupt
	IEC0bits.T3IE = 0;		// Disable Timer 3 interrupt
	T3CONbits.TON = 1;		//Start Timer 3
    return;
}


// DMA0 configuration
// Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM 
// AMODE: Peripheral Indirect Addressing Mode
// MODE: Continuous, Ping-Pong Mode
// IRQ: ADC Interrupt

void initDma0(void)
{
	DMA0CONbits.AMODE = 2;			// Configure DMA for Peripheral indirect mode
	//DMA0CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode
    DMA0CONbits.MODE  = 0;			// Configure DMA for Continuous mode, Ping-Pong mode disabled
 //   DMA0CONbits.DIR = 0;        // Reads from peripheral address, writes to RAM address
	DMA0PAD=(int)&ADC1BUF0;
	DMA0CNT = (SAMP_BUFF_SIZE*NUM_CHS2SCAN)-1;					
	DMA0REQ = 13;					// Select ADC1 as DMA Request source

    DMA0STAH = 0x0000;
    DMA0STAL = __builtin_dmaoffset(bufferA);		

	IFS0bits.DMA0IF = 0;			//Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 0;			//Disable DMA interrupt

	DMA0CONbits.CHEN=1;				// Enable DMA
    return;
}
/*=============================================================================
_DMA0Interrupt(): ISR name is chosen from the device linker script.
=============================================================================*/


void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{	
	IFS0bits.DMA0IF = 0;		// Clear the DMA0 Interrupt Flag
}
