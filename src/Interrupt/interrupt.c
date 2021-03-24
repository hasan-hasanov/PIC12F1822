//**********************************************************************************
// Example program showing how to handle interrupts on a PIC12F1822
//
// Device: PIC12F1822
// Demo Board: PICkit 4
// Compiler: Microchip XC8 v2.32
// IDE: MPLAB X v5.45
// Created: 09 March 2021
//
// This program shows how to handle interrupts.
// The main idea of an interrupt is to break the program flow and execute another code instead.
// For this example I use PICKIT 4 programmer.
// The result is when a user press the button the PIC enters in interrupt state and toggles an LED.
// Do not forget to connect a Pull Down resistor for the voltage in PIN.
// In our example this is RA2 which is also the INT pin.
// From the documentation:
//          The INT pin can be used to generate an asynchronous
//          edge-triggered interrupt. This interrupt is enabled by
//          setting the INTE bit of the INTCON register. The
//          INTEDG bit of the OPTION register determines on which
//          edge the interrupt will occur. When the INTEDG bit is
//          set, the rising edge will cause the interrupt. When the
//          INTEDG bit is clear, the falling edge will cause the
//          interrupt. The INTF bit of the INTCON register will be set
//          when a valid edge appears on the INT pin. If the GIE and
//          INTE bits are also set, the processor will redirect
//          program execution to the interrupt vector.
//**********************************************************************************
//                                   PIC12F1822 Pinout for this example
//                                   ----------
//          3.3V Power source -> Vdd |1      8| GND
//                               RA5 |2      7| RA0
//                               RA4 |3      6| RA1 -> voltage out for the LED
//                               RA3 |4      5| RA2 <- Voltage in from the button
//                                   ----------
//**********************************************************************************

#include <xc.h>

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

#include <xc.h> // Include standard header file

// Definitions
#define _XTAL_FREQ  16000000 // This is used by the __delay_ms(xx) and __delay_us(xx) functions

void __interrupt(high_priority) high_priority_interrupt(void) {
    INTCONbits.GIE = 0; // Disable global interrupts until processing

    if (INTCONbits.INTF) { // Check if the interrupt is triggered on INTE PIN which is RA0
        LATAbits.LATA1 = ~LATAbits.LATA1;
        INTCONbits.INTF = 0; // Set the interrupt to handled so it can process further interrupts
    }

    INTCONbits.GIE = 1; // Enable global interrupts after processing
}

void main() {
    // Set up oscillator control register
    OSCCONbits.SPLLEN = 0; // PLL is disabled
    OSCCONbits.IRCF = 0x0F; // Set OSCCON IRCF bits to select OSC frequency=16Mhz
    OSCCONbits.SCS = 0x02; // Set the SCS bits to select internal oscillator block

    ANSELA = 0x00; // Set all pins to digital

    TRISA = 0x00; // Set all I/O pins as output
    TRISAbits.TRISA2 = 1; // RA2 = Set as voltage in. This is the INT pin.

    PORTA = 0x00; // Zero ALL the PORTA pins
    ADCON0 = 0; // ADC is off

    INTCONbits.GIE = 1; // Enable global interrupts
    INTCONbits.INTE = 1; // Enable external interrupts
    OPTION_REGbits.INTEDG = 1; // Interrupt on rising edge on RA2

    for (;;) {
    }
}