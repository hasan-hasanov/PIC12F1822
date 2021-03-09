//**********************************************************************************
// Example program showing how to read a button input on a PIC12F1822
//
// Device: PIC12F1822
// Demo Board: PICkit 2 Low Pin Count Demo Board + AC244043 header board for debugging
// Compiler: Microchip XC8 v2.32
// IDE: MPLAB X v5.45
// Created: 09 March 2021
//
// This program shows mainly how to initialize PIC to capture a button input.
// For this example I use PICKIT 4 programmer.
// The result is when a user press the button an LED blinks.
// Do not forget to connect a Pull Down resistor for the voltage in PIN.
// In our example this is RA4
//**********************************************************************************
//								  	 PIC12F1822 Pinout for this example
//								 	 ----------
//          3.3V Power source -> Vdd |1      8| GND
//							     RA5 |2      7| RA0
// Voltage in from the button -> RA4 |3      6| RA1
//							     RA3 |4      5| RA2 -> voltage out for the LED
//								     ----------
//**********************************************************************************
#pragma config FOSC = ECH       // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include <xc.h> // include standard header file

// Definitions
#define _XTAL_FREQ  16000000        // this is used by the __delay_ms(xx) and __delay_us(xx) functions

void main() {
    // set up oscillator control register
    OSCCONbits.SPLLEN = 0; // PLL is disabled
    OSCCONbits.IRCF = 0x0F; //set OSCCON IRCF bits to select OSC frequency=16Mhz
    OSCCONbits.SCS = 0x02; //set the SCS bits to select internal oscillator block

    ANSELAbits.ANSA0 = 0; // Set to digital
    ANSELAbits.ANSA1 = 0; // Set to digital
    ANSELAbits.ANSA2 = 0; // Set to digital
    ANSELAbits.ANSA4 = 0; // Set to digital

    // Set up I/O pins
    // PORT A Assignments
    TRISAbits.TRISA0 = 0; // RA0 = Digital Voltage out
    TRISAbits.TRISA1 = 0; // RA1 = Digital Voltage out
    TRISAbits.TRISA2 = 0; // RA2 = Digital Voltage out
    TRISAbits.TRISA3 = 0; // RA3 = Digital Voltage out
    TRISAbits.TRISA4 = 1; // RA4 = Digital Voltage in
    TRISAbits.TRISA5 = 0; // RA0 = Digital Voltage out

    PORTAbits.RA0 = 0; // Zero ALL the PORTA pins
    PORTAbits.RA1 = 0; // Zero ALL the PORTA pins
    PORTAbits.RA2 = 0; // Zero ALL the PORTA pins
    PORTAbits.RA3 = 0; // Zero ALL the PORTA pins
    PORTAbits.RA4 = 0; // Zero ALL the PORTA pins
    PORTAbits.RA5 = 0; // Zero ALL the PORTA pins

    // Set up ADC
    ADCON0 = 0; // ADC is off

    for (;;) {
        __delay_ms(100); // wait 100 ms
        if (PORTAbits.RA4 == 0) {
            
            // Set LAT A2 bit to low
            LATAbits.LATA2 = 0;
        } else {
            
            // Set LAT A2 bit to high
            LATAbits.LATA2 = 1;
        }
    }
}