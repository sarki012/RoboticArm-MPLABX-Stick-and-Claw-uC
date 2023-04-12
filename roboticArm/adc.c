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

// Define Message Buffer Length for ECAN1/ECAN2
#define  MAX_CHNUM	 			3		// Highest Analog input number in Channel Scan
#define  SAMP_BUFF_SIZE	 		8		// Size of the input buffer per analog input
#define  NUM_CHS2SCAN			3		// Number of channels enabled for channel scan

// Number of locations for ADC buffer = 14 (AN0 to AN13) x 8 = 112 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode
int  BufferA[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((space(dma),aligned(256)));
int  BufferB[MAX_CHNUM][SAMP_BUFF_SIZE] __attribute__((space(dma),aligned(256)));

void ProcessADCSamples(int * AdcBuffer);

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
	AD1CON1bits.FORM   = 2;		// Data Output Format: Fractional
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
}

/*=============================================================================  
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us. 
=============================================================================*/
void initTmr3() 
{
	TMR3 = 0x0000;
	PR3  = 4999;			// Trigger ADC1 every 125usec
	IFS0bits.T3IF = 0;		// Clear Timer 3 interrupt
	IEC0bits.T3IE = 0;		// Disable Timer 3 interrupt
	T3CONbits.TON = 1;		//Start Timer 3
}


// DMA0 configuration
// Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM 
// AMODE: Peripheral Indirect Addressing Mode
// MODE: Continuous, Ping-Pong Mode
// IRQ: ADC Interrupt

void initDma0(void)
{
	DMA0CONbits.AMODE = 2;			// Configure DMA for Peripheral indirect mode
	DMA0CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode
	DMA0PAD=(int)&ADC1BUF0;
	DMA0CNT = (SAMP_BUFF_SIZE*NUM_CHS2SCAN)-1;					
	DMA0REQ = 13;					// Select ADC1 as DMA Request source

	DMA0STAH = 0x0000;
    DMA0STAL = 0x0000;
	DMA0STBH = 0x0000;
    DMA0STBL = 0x0000;
    
    //DMA0STB = __builtin_dmaoffset(BufferB);     //along with a DMA0STBH and DMA0STBL

	IFS0bits.DMA0IF = 0;			//Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1;			//Set the DMA interrupt enable bit

	DMA0CONbits.CHEN=1;				// Enable DMA

}

/*
#define ADC_BUFF_LEN 8 //Length of the DMA Buffer, should be a power of 2

//Create DMA buffer for ADC, the macro is required because the buffer is
//outside of the normal memory space
// Number of locations for ADC buffer = 14 (AN0 to AN13) x 8 = 112 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode
int  bufferA[3][ADC_BUFF_LEN] __attribute__((space(dma),aligned(256)));
int  bufferB[3][ADC_BUFF_LEN] __attribute__((space(dma),aligned(256)));

void ProcessADCSamples(int * AdcBuffer);


//__eds__ unsigned int stickBuffer[ADC_BUFF_LEN] __attribute__((eds,space(dma)));
//__eds__ unsigned int tipBuffer[ADC_BUFF_LEN] __attribute__((eds,space(dma)));
//__eds__ unsigned int clawBuffer[ADC_BUFF_LEN] __attribute__((eds,space(dma)));

 
void dmaInit(void)
{
    ///////////DMA for A/D////////////////////////
    DMA0CONbits.CHEN = 0;       //Channel 0 is disabled
    DMA0CONbits.SIZE = 1;       //Word
    DMA0CONbits.DIR = 0;        //Reads from peripheral address, writes to RAM address
    DMA0CONbits.HALF = 0;        //Initiates interrupt when all of the data have been moved
    DMA0CONbits.NULLW = 0;       //Normal operation
    //DMA0CONbits.AMODE = 0b00;    //Register indirect with post-increment mode
    DMA0CONbits.AMODE = 0b10;    //Peripheral indirect addressing mode
    //DMA0CONbits.MODE = 0b00;     //Continuous, ping-pong modes are disabled    
    DMA0CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode
    DMA0PAD = (volatile unsigned int) &ADC1BUF0; //Point DMA0 to ADC1 buffer
    DMA0CNT = (ADC_BUFF_LEN*3)-1; //Set DMA buffer length (Buffer Length=DMA0CNT+1)
    DMA0REQbits.FORCE = 0;      //Automatic DMA transfer initiation by DMA request
    DMA0REQbits.IRQSEL = 0b00001101;    //Connect DMA0 channel to ACD1
    //Ensure the DMA interrupt is off
    IFS0bits.DMA0IF = 0; //Clear DMA interrupt flag
    IEC0bits.DMA0IE = 0; //Disable DMA interrupt
    //This pair of registers holds the start address of the buffer in the DMA
    //memory space that holds the ACD data. More info on the __builtin_dmaoffset
    //macro can be found on page 307 of the MPLAB XC16 C Compiler User's Guide.
    DMA0STAH = __builtin_dmaoffset(bufferA); //DMA0 start address high word
    DMA0STAL = 0X0000;
    DMA0STBH = __builtin_dmaoffset(bufferB); //DMA0 start address high word
    //DMA0STAL = __builtin_dmaoffset(stickBuffer); //DMA0 start address low byte 
    //DMA0STAL = 0x0000; //DMA0 start address high byte
    DMA0CONbits.CHEN = 1;       //DMA Channel 0 is enabled
}
void adcInit(void)
{
///////////////A/D////////////////////////////////
    TRISAbits.TRISA0 = 1;       //Input
    TRISAbits.TRISA1 = 1;       //Input
    TRISBbits.TRISB0 = 1;       //Input
    
    AD1CON1bits.ADON = 0;   //ADC is disabled before changing registers
    AD1CON1bits.ADSIDL = 0;  //Continue operation in idle mode
    //AD1CON1bits.ADDMABM = 1;     //DMA buffers are written in the order of conversion; the module provides an address to the DMA channel that is the same as the address used for the non-DMA stand-alone buffer
    AD1CON1bits.ADDMABM = 0;        //Scatter/Gather Mode 
    AD1CON1bits.AD12B = 0;   //10 bit
    AD1CON1bits.FORM = 0b10;    //Fractional data output format bits
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC0 = 1;    //Internal counter ends sampling and starts conversion (auto-convert)
    AD1CON1bits.SSRC1 = 1;
    AD1CON1bits.SSRC2 = 1;  //Internal counter ends sampling and starts conversion (auto-convert)
    AD1CON1bits.ASAM = 1;   //Sampling begins immediately after last conversion
    AD1CON1bits.SAMP = 1;   //ADC sample enable bit
    AD1CON1bits.SIMSAM = 1;     //Samples Ch0, CH1, and CH2 simultaneously
    AD1CON2bits.VCFG0 = 0;
    AD1CON2bits.VCFG1 = 0;
    AD1CON2bits.VCFG2 = 0;      //Voltage reference AVDD and AVSS
    AD1CON2bits.CSCNA = 1;      //Scan inputs
    AD1CON2bits.CHPS0 = 1;
    AD1CON2bits.CHPS1 = 1;      //Converts CH0, CH1, CH2, AND CH3
    //AD1CON2bits.BUFM = 1;       // Starts filling first half of buffer on first interrupt and the second half of buffer on next interrupt
    AD1CON2bits.BUFM = 0;       //Always starts filling the buffer from the start address
    AD1CON2bits.SMPI = 0b00000;      //Increments the DMA address after completion of every sample/conversion operation
    AD1CON2bits.ALTS = 0;       //Always uses channel input selects for sample A
    
    //AD1PCFGL = 0x0000;
    AD1CON3bits.ADRC = 1;       //ADC internal RC clock
    
   // AD1CON4bits.ADDMAEN = 0;    //Conversion results stored in ADC1BUF0 through ADC1BUFF registers
     AD1CON4bits.ADDMAEN = 1;       //DMA ADC1BUF0 is the only ADC buffer used
     AD1CON4bits.DMABL = 0b011;     //Allocates 8 words of buffer to each analog input
   // AD1CHS123bits.CH123NB0 = 0;      //Negative input select for sample MUXB bits is VREFL
   // AD1CHS123bits.CH123NB1 = 0;      //Negative input select for sample MUXB bits is VREFL
   // AD1CHS123bits.CH123SB = 0;      //Positive input select for sample MUXB bit is AN0, AN1, and AN2
    AD1CHS123bits.CH123NA0 = 0;     //Negative input select for sample MUXA bits is VREFL
    AD1CHS123bits.CH123NA1 = 0;     //Negative input select for sample MUXA bits is VREFL
    AD1CHS123bits.CH123SA = 0;      //Positive input select for sample MUXA bit is AN0, AN1, and AN2
    
    AD1CSSL = 0b0000000000000111;       //Scan AN0, AN1, and AN2
    AD1CSSHbits.CSS25 = 1;      //Scan AN0 
    
    IEC0bits.AD1IE = 1;         //Interrupt request enabled
    IPC3bits.AD1IP0 = 0;
    IPC3bits.AD1IP1 = 1;
    IPC3bits.AD1IP2 = 1;        //Priority = 6 
    AD1CON1bits.ADON = 1;   //ADC is Operating
    return;
}
*/