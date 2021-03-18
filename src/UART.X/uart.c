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
//                               RA5 |2      7| RA0 -> TX
//                               RA4 |3      6| RA1 -> RX
//                               RA3 |4      5| RA2 <- Voltage in from the button
//                                   ----------
//**********************************************************************************

#include <xc.h>

#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
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
#include <stdbool.h>
#include <stdint.h>

// Definitions
#define _XTAL_FREQ  1000000 // This is used by the __delay_ms(xx) and __delay_us(xx) functions
uint8_t data = 'ABCD';

void uart_init() {
    SPBRGH = 25 >> 8; // For 9600 Baud and with 11.0592 Mhz Crystal
    SPBRGL = 25 & 0xFF;
    APFCONbits.RXDTSEL = 0; // RA1 as RX Pin
    APFCONbits.TXCKSEL = 0; // RA0 as TX Pin
    TRISAbits.TRISA0 = 0; // RA0 as O/P Pin
    TRISAbits.TRISA1 = 1; // RA0 as I/P Pin
    ANSELA = 0; // Port A all pins are digital I/O Pin
    WPUAbits.WPUA0 = 1; // Enable weak pull up on RA0
    WPUAbits.WPUA1 = 1; // Enable weak pull up on RA1
    TXSTAbits.TX9 = 0; // 8-BIT DATA MODE
    TXSTAbits.TXEN = 1; // ENABLE TRANSMITTER
    TXSTAbits.SYNC = 0; // ENABLE ASYNCHRONOUS MODE
    TXSTAbits.SENDB = 0; // SYNC BREAK TRANSMISSION COMPLETED
    TXSTAbits.BRGH = 1; // SELECT LOW BAUD RATE
    BAUDCONbits.SCKP = 0; // DON'T INVERT POLARITY
    BAUDCONbits.BRG16 = 1; // 8-BIT BAUDRATE GENERATOR
    BAUDCONbits.ABDEN = 0; // AUTO BAUD RATE DETECT DISABLE

    RCSTAbits.RX9 = 0; // ENABLE 8-BIT RECEPTION
    RCSTAbits.CREN = 1; // ENABLE RECEIVER
    RCSTAbits.FERR = 0;
    RCSTAbits.OERR = 0;
    RCSTAbits.SPEN = 1; // ENABLE SERIAL PORT 
}

int main(void) {
    // Set up oscillator control register
    OSCCONbits.SPLLEN = 0; // PLL is disabled
    OSCCONbits.IRCF = 0b1011; // Set OSCCON IRCF bits to select OSC frequency=16Mhz
    OSCCONbits.SCS = 0x02; // Set the SCS bits to select internal oscillator block

    uart_init();

    for (;;) {
        __delay_ms(200);

        TXREG = 'A';
        while (TXSTAbits.TRMT == 0);
    }
    return 0;
}
