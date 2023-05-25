/*
 * File:   init.c for UC2
 * Author: Erik Sarkinen
 *
 * Created on February 12, 2022, 8:26 AM
 */

#include <xc.h>
#include <stdio.h>
#include <p33ep512mc502.h>
#include <stdlib.h>
#include <string.h>


void init(void)
{ 
    int i = 0;
    OSCCONbits.NOSC0 = 0;       //Fast RC Oscillator
    OSCCONbits.NOSC1 = 0;       //Fast RC Oscillator
    OSCCONbits.NOSC2 = 0;       //Fast RC Oscillator
   // OSCCONbits.OSWEN = 1;       //Requests oscillator switch to selection specified by the NOSC[2:0] bits
    OSCTUN = 0;         //Center frequency 7.37 MHz
    /*
    PLLFBD = 0b0000000000111111;     //PLLDIV, M, PLL Multiplier = 65    
    CLKDIVbits.PLLPRE = 0;      //Divide by 2
    CLKDIVbits.PLLPOST = 0;     // Divide by 2
    CLKDIVbits.FRCDIV0 = 0;     //Divide by 1
    CLKDIVbits.FRCDIV1 = 0;
    CLKDIVbits.FRCDIV2 = 0;
    
    __builtin_write_OSCCONH(1) ; // 1 = FRCPLL clock selection
 //   __builtin_write_OSCCONL(OSCCON | 0x01);
    __builtin_write_OSCCONL(1) ; // Sets OSWEN bit
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b001);
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
   */ 
    RPINR18 = 0b0000000000100110;       //U1RX = RP38
    RPINR19 = 0b0000000000100101;       //U2RX = RP37 
    RPOR3 = 0b0000001100000001;     // U2TX = RP41, U1_TX = RP40

   // Servo Circuit Board Config 
    TRISBbits.TRISB4 = 0;       //EAN
    TRISBbits.TRISB1 = 0;       //P2_4
    TRISBbits.TRISB0 = 0;       //P2_0
    TRISAbits.TRISA4 = 0;       //SW Button
    //Set up Bluetooth  EEPROM programming   Normal Normal_NC2
    LATBbits.LATB4 = 0;     //EAN  0           0        1     
    LATBbits.LATB1 = 1;     //P2_4 1           1        1
    LATBbits.LATB0 = 1;     //P2_0 0           1        1 
    LATAbits.LATA4 = 1;     //SW Button
    
    ///////////////////UART2/////////////////////////////////////////////
    U2MODEbits.UARTEN = 0;      //UART2 is disabled
    TRISBbits.TRISB9 = 1;       //U2 TX
    TRISBbits.TRISB5 = 1;       //U2 RX
    U2MODEbits.USIDL = 0;       //Continue module operation in Idle mode
    U2MODEbits.IREN = 0;        //IrDA encoder and decoder disabled
    U2MODEbits.UEN0 = 0;        //U2TX and RX pins are enabled and used. 
    U2MODEbits.UEN1 = 0;        //CTS and RTS pins are controlled by PORT latches
    U2MODEbits.WAKE = 0;        //UART2 continues to sample the U1RX pin; interrupt is generated on the falling edge
    U2MODEbits.LPBACK = 0;      //Loopback mode is disabled
    U2MODEbits.ABAUD = 0;       //Auto-Baud disabled
    U2MODEbits.URXINV = 0;      //Idle state is '0'
    U2MODEbits.BRGH = 1;        //High Speed
    U2MODEbits.PDSEL1 = 0;      //8-bit data, no parity
    U2MODEbits.URXINV = 0;  // Bit4 IdleState = 1  (for dsPIC)
    U2MODEbits.RTSMD = 0;   // Bit11 Simplex Mode
    U2MODEbits.PDSEL0 = 0;      
    U2MODEbits.STSEL = 0;       //One stop bit 
    INTCON2bits.GIE = 1;        //Global interrupt enable bit (enabled)
    IEC1bits.U2RXIE = 1;        //UART 2 interrupt enabled
    IFS1bits.U2RXIF = 0;       //Reset RX interrupt flag 
    IPC7bits.U2RXIP = 7;        //Set RX interrupt priority to high priority
    U2STAbits.URXISEL1 = 0;     //Interrupt is set when there is 1 char
    U2STAbits.URXISEL0 = 0;     
    U2STAbits.ADDEN = 0;          //Address detect mode disabled
    U2STAbits.PERR = 0;         //Parity Error
    U2STAbits.OERR = 0;           //Clear the Receive //Overflow Flag.
   // U2BRG = 129;                 //BAUD RATE 115,200 Fosc = 120Mhz Mhz, Fp = Fosc/2
    U2BRG = 7;                 //BAUD RATE 115,200 Fosc = 7.37 Mhz, Fp = Fosc/2 5 is close
    IEC1bits.U2TXIE = 0;        //No transmit interrupt (We poll TRMT)
    U2MODEbits.UARTEN = 1;      //UART2 is enabled
    U2STAbits.UTXEN = 1;        //Transmit enabled
   
    ///////////////////UART1/////////////////////////////////////////////
    U1MODEbits.UARTEN = 0;      //UART1 is disabled
    TRISBbits.TRISB6 = 1;       //U1 RX
    TRISBbits.TRISB8 = 1;       //U1 TX
    U1MODEbits.USIDL = 0;       //Continue module operation in Idle mode
    U1MODEbits.IREN = 0;        //IrDA encoder and decoder disabled
    U1MODEbits.UEN0 = 0;        //U1TX and RX pins are enabled and used. 
    U1MODEbits.UEN1 = 0;        //CTS and RTS pins are controlled by PORT latches
    U1MODEbits.WAKE = 0;        //UART1 continues to sample the U1RX pin; interrupt is generated on the falling edge
    U1MODEbits.LPBACK = 0;      //Loopback mode is disabled
    U1MODEbits.ABAUD = 0;       //Auto-Baud disabled
    U1MODEbits.URXINV = 0;      //Idle state is '0'
    U1MODEbits.BRGH = 1;        //High Speed
    U1MODEbits.PDSEL1 = 0;      //8-bit data, no parity
    U1MODEbits.URXINV = 0;      // Bit4 IdleState = 1  (for dsPIC)
    U1MODEbits.RTSMD = 0;       // Bit11 Simplex Mode
    U1MODEbits.PDSEL0 = 0;      
    U1MODEbits.STSEL = 0;       //One stop bit 
    IEC0bits.U1RXIE = 1;        //UART 1 interrupt enabled
    IFS0bits.U1RXIF = 0;        //Reset RX interrupt flag 
    IPC2bits.U1RXIP = 7;        //Set RX interrupt priority to high priority
    U1STAbits.URXISEL1 = 0;     //Interrupt is set when there is 1 char
    U1STAbits.URXISEL0 = 0;     
    U1STAbits.ADDEN = 0;        //Address detect mode disabled
    U1STAbits.PERR = 0;         //Parity Error
    U1STAbits.OERR = 0;         //Clear the Receive //Overflow Flag.
   // U1BRG = 129;                 //BAUD RATE 115,200 Fosc = 120Mhz Mhz, Fp = Fosc/2
    U1BRG = 7;                 //BAUD RATE 115,200 Fosc = 7.37 Mhz, Fp = Fosc/2
    IEC0bits.U1TXIE = 0;        //No transmit interrupt (We poll TRMT)
    U1MODEbits.UARTEN = 1;      //UART1 is enabled
    U1STAbits.UTXEN = 1;        //Transmit enabled
    
    ////////////////PWM Initialization///////////////////////////////
    PTCONbits.PTEN = 0;         //PWM Module is disabled before any code
    
    TRISBbits.TRISB15 = 0;      //PWM1L is an output
    TRISBbits.TRISB14 = 0;      //PWM1H is an output
    TRISBbits.TRISB13 = 0;      //PWM2L is an output
    TRISBbits.TRISB12 = 0;      //PWM2H is an output
    TRISBbits.TRISB11 = 0;      //PWM3L is an output
    TRISBbits.TRISB10 = 0;      //PWM3H is an output different types of outputs?
    PORTBbits.RB15 = 0;         //Clear the output pin
    PORTBbits.RB14 = 0;         //Clear the output pin
    PORTBbits.RB13 = 0;         //Clear the output pin
    PORTBbits.RB12 = 0;         //Clear the output pin
    PORTBbits.RB11 = 0;         //Clear the output pin
    PORTBbits.RB10 = 0;         //Clear the output pin
    
    PTCONbits.SYNCOEN = 0;      //SYNCO1 output is enabled
    PTCONbits.SYNCEN = 0;       //External synchronization of primary time base is disabled
    PTCONbits.SYNCSRC0 = 0;     //Synchronous Source is PTGO17
    PTCONbits.SYNCSRC1 = 1;
    PTCONbits.SYNCSRC2 = 0;
  //  PTCON2bits.PCLKDIV = 4;     //Divide by 16 prescalar
    PTCON2bits.PCLKDIV = 2;     //Divide by 4 prescalar
   // PTCON2bits.PCLKDIV0 = 0;      //010 divide by 4 prescaler *Adjust this depending on resolution
    //PTCON2bits.PCLKDIV1 = 1;
    //PTCON2bits.PCLKDIV2 = 0;
    PTCONbits.PTSIDL = 0;
    PTCONbits.EIPU = 0;
    PTCONbits.SYNCPOL = 0;
    PWMCON1bits.ITB = 1;        //PHASE1 register provides timing for this PWM generator
    PWMCON2bits.ITB = 1;        //PHASE2 register provides timing for this PWM generator
    PWMCON3bits.ITB = 1;        //PHASE3 register provides timing for this PWM generator
    PWMCON1bits.MDCS = 0;       //PDC1 register provide duty cycle information for this PWM generator
    PWMCON2bits.MDCS = 0;       //PDC2 register provide duty cycle information for this PWM generator
    PWMCON3bits.MDCS = 0;       //PDC3 register provide duty cycle information for this PWM generator
    PWMCON1bits.FLTIEN = 0;     //Fault interrupt disabled
    PWMCON2bits.FLTIEN = 0;     //Fault interrupt disabled
    PWMCON1bits.TRGIEN = 0;     //Trigger interrupt disabled
    PWMCON2bits.TRGIEN = 0;     //Trigger interrupt disabled
    PWMCON1bits.CLIEN = 0;      //Current limit interrupt disabled
    PWMCON2bits.CLIEN = 0;      //Current limit interrupt disabled
    PWMCON1bits.MTBS = 0;       //Primary master time base
    PWMCON2bits.MTBS = 0;       //Primary master time base
    PWMCON1bits.CAM = 0;        //Edge aligned
    PWMCON2bits.CAM = 0;        //Edge aligned
    PWMCON3bits.CAM = 0;        //Edge aligned
    PWMCON1bits.FLTSTAT = 0;    //No fault interrupt is pending
    PWMCON2bits.FLTSTAT = 0;    //No fault interrupt is pending
    PWMCON1bits.DTC = 0;
    PWMCON2bits.DTC = 0;
    PWMCON1bits.XPRES = 0;
    PWMCON2bits.XPRES = 0;
    PWMCON1bits.IUE = 0;
    PWMCON2bits.IUE = 0;
    PWMCON1bits.DTC = 2;
    PWMCON2bits.DTC = 2;
    IOCON1bits.PENL = 1;        //PWM module controls PWM1L pin
    IOCON2bits.PENL = 1;        //PWM module controls PWM2L pin
    IOCON1bits.PENH = 1;        //PWM module controls PWM1H pin
    IOCON2bits.PENH = 1;        //PWM module controls PWM2H pin
    IOCON3bits.PENH = 1;        //PWM module controls PWM3H pin
    IOCON3bits.PENL = 1;        //PWM module controls PWM3L pin
    IOCON1bits.PMOD0 = 1;       //Pin pair is in the redundant output mode
    IOCON1bits.PMOD1 = 0;       //Pin pair is in the redundant output mode
    IOCON2bits.PMOD0 = 1;       //Pin pair is in the redundant output mode
    IOCON2bits.PMOD1 = 0;       //Pin pair is in the redundant output mode
    IOCON3bits.PMOD0 = 1;       //Pin pair is in the redundant output mode
    IOCON3bits.PMOD1 = 0;       //Pin pair is in the redundant output mode
    IOCON1bits.OVRENH = 0;      //PWM generator controls pin
    IOCON1bits.OVRENL = 0;      //PWM generator controls pin
    IOCON2bits.OVRENH = 0;      //PWM generator controls pin
    IOCON2bits.OVRENL = 0;      //PWM generator controls pin
    IOCON3bits.OVRENL = 0;      //PWM generator controls pin
    IOCON3bits.OVRENH = 0;      //PWM generator controls pin
    IOCON1bits.POLH = 0;
    IOCON1bits.POLL = 0;
    IOCON2bits.POLH = 0;
    IOCON2bits.POLL = 0;
    IOCON3bits.POLH = 0;
    IOCON3bits.POLL = 0;
    IOCON1bits.SWAP = 0;
    IOCON2bits.SWAP = 0;
    IOCON3bits.SWAP = 0;
    IOCON1bits.OSYNC = 0;
    IOCON2bits.OSYNC = 0;
    IOCON3bits.OSYNC = 0;
    IOCON1bits.OVRDAT = 0;
    IOCON2bits.OVRDAT = 0;
    IOCON3bits.OVRDAT = 0;
    PTCONbits.SEIEN = 0;        //Special event interrupt disabled
    DTR1 = 0;
    DTR2 = 0;
    ALTDTR1 = 0;
    ALTDTR2 = 0;
    FCLCON1 = 0x0000;
    FCLCON2 = 0x0000;
    FCLCON1bits.FLTMOD = 3;
    FCLCON2bits.FLTMOD = 3;
    PTCONbits.PTEN = 1;         //PWM Module is enabled
    
    //Timer 1 500 ms timer for incrementing or decrementing x
    T2CONbits.TON = 0;          //Stops 16-bit timer 2
    T2CONbits.TSIDL = 0;        //Continue operation in idle mode
    T2CONbits.TCKPS = 0;        //1:1 prescalar
    T2CONbits.TCS = 0;          //Internal clock
    T2CONbits.TGATE = 0;        //Disable gated timer mode operation
    T2CONbits.T32 = 0;      //16-bit
    TMR2 = 0x00; 			//Clear contents of the timer register
    PR2 = 3684;	//Load the Period register with 1mSec counter 
	IPC1bits.T2IP = 0x01; 	//Setup Timer2 interrupt for desired priority level
	// (This example assigns level 1 priority)
	IFS0bits.T2IF = 0; 		//Clear the Timer2 interrupt status flag
	IEC0bits.T2IE = 1; 		//Enable Timer2 interrupts
	T2CONbits.TON = 1; 		//Start Timer2 with prescaler settings at 1:1 and
							//clock source set to the internal instruction cycle
    return;
}