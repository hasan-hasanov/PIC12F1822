//**********************************************************************************
// Example program showing how to handle interrupts on a PIC12F1822
//
// Device: PIC12F1822
// Demo Board: PICkit 4
// Compiler: Microchip XC8 v2.32
// IDE: MPLAB X v5.45
// Created: 19 June 2021
//
// This program shows how to generate simple PWM signal.
//**********************************************************************************
//                                   PIC12F1822 Pinout for this example
//                                   ----------
//          3.3V Power source -> Vdd |1      8| GND
//                 PWM output <- RA5 |2      7| RA0
//                               RA4 |3      6| RA1
//                               RA3 |4      5| RA2
//                                   ----------
//**********************************************************************************

#include <xc.h>

#pragma config FOSC = INTOSC  // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF     // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF    // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF    // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF       // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF      // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF    // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF     // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF   // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO   // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON    // Low-Voltage Programming Enable (Low-voltage programming enabled)

#include <xc.h> // Include standard header file

int main()
{
    //
    // 8 MHz Internal Oscillator so that PLL will give 32 MHz
    //
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SPLLEN = 1;

    // With 32 MHz system clock, PWM frame frequency will be 32 MHz/4/256 = 31.25 kHz
    PR2 = 0xFF;

    // PWM mode: P1A, P1B active-high
    CCP1CONbits.CCP1M = 0b1100;

    // Duty Cycle = 0.25
    CCPR1L = 0x40;

    // PWM P1A Output is on RA5 (Pin 2 of the DIP-8)
    TRISAbits.TRISA5 = 0;
    APFCONbits.CCP1SEL = 1;

    PIR1bits.TMR2IF = 0;
    // Leave prescale, postscale = 1:1, turn on the timer
    T2CONbits.TMR2ON = 1;

    // The initialize function did not set the steering bit.
    // This can be set or cleared to enable or disable
    // the PWM P1A output
    PSTR1CONbits.STR1A = 1;

    while (1)
    {
    }
}