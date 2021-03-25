//**********************************************************************************
// Example program showing how to handle analog read on a PIC12F1822
//
// Device: PIC12F1822
// Demo Board: PICkit 4
// Compiler: Microchip XC8 v2.32
// IDE: MPLAB X v5.45
// Created: 09 March 2021
//
// This program shows how to read analog input and send data to UART protocol.
// The main idea of UART is to send data via serial connection.
// I use a cheap chinese soil moisture sensor YL-69 without the digital circuit that came with the sensor.
// For this example I use PICKIT 4 programmer and CP2102 connector.
// The sensor works like this: if the soil is dry it outputs 5V, if it is moist it outputs less.
// After I obtain the data from the sensor I send it to my PC using UART protocol.
// In order to receive the connection I use PuTTY.
//**********************************************************************************
//                                   PIC12F1822 Pinout for this example
//                                   ----------
//            5V Power source -> Vdd |1      8| GND
//                               RA5 |2      7| RA0 -> TX
//                               RA4 |3      6| RA1 -> RX
//                               RA3 |4      5| RA2 <- Voltage in from analog sensor
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
#include <stdlib.h>
#include <stdio.h>

// Definitions
#define _XTAL_FREQ  1000000 // This is used by the __delay_ms(xx) and __delay_us(xx) functions

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

void uart_send(char* message) {
    for (size_t i = 0; i < sizeof message / sizeof message[0]; i++) {
        TXREG = message[i];

        // The heart of the EUSART is the serial
        // Transmit Shift Register (TSR). The TSR obtains its data from
        // the transmit buffer, which is the TXREG register
        while (TXSTAbits.TRMT == 0); // Wait the register to clear out
    }
}

unsigned int Read_ADC_Value(void) {
    unsigned int ADCValue;

    ADCON0bits.GO = 1; // start conversion
    while (ADCON0bits.GO); // wait for conversion to finish
    ADCValue = ADRESH << 8; // get the 2 msbs of the result and rotate 8 bits to the left
    ADCValue = ADCValue + ADRESL; // now add the low 8 bits of the resut into our return variable
    return (ADCValue); // return the 10bit result in a single variable
}

int main(void) {
    unsigned int AnalogValue; // used to store ADC result after capture

    uart_init();

    // Set up oscillator control register
    OSCCONbits.SPLLEN = 0; // PLL is disabled
    OSCCONbits.IRCF = 0b1011; // Set OSCCON IRCF bits to select OSC frequency=1Mhz
    OSCCONbits.SCS = 0x02; // Set the SCS bits to select internal oscillator block

    TRISAbits.TRISA0 = 0; // RA0 = DAC voltage output
    TRISAbits.TRISA1 = 0; // RA1 = nc
    TRISAbits.TRISA2 = 1; // RA2 = Analog voltage in
    TRISAbits.TRISA3 = 0; // RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0; // RA4 = nc
    TRISAbits.TRISA5 = 0; // RA5 = nc

    // Set up ADC
    ANSELAbits.ANSA2 = 1; // Select A2 as analog input pin for analog sensor
    // You will need to set the ANSA bits for each pin you want
    // to use as analog inputs

    ADCON0bits.CHS = 0b00010; // This selects which analog input to use for the ADC conversion
    // for this example we are using A2 as our input

    ADCON1bits.ADCS = 0x01; // select ADC conversion clock select as Fosc/8
    ADCON1bits.ADFM = 0x01; // results are right justified

    ADCON0bits.ADON = 1; // ADC is on

    int DAC_Value; // this could be used to set the DAC output register
    for (;;) {
        AnalogValue = Read_ADC_Value(); // Read the analog voltage on pin RA1
        DAC_Value = (AnalogValue >> 5) & 0x1F; // divide ADC value by 32 and mask off lower 5 bits

        // Since this chip is too little to use some of the fancy functions
        // like sprintf or itoa and ftoa I had to improvise
        if (AnalogValue > 10 && AnalogValue <= 20) {
            uart_send("A ");
        } else if (AnalogValue > 20 && AnalogValue <= 30) {
            uart_send("B ");
        } else if (AnalogValue > 30 && AnalogValue <= 40) {
            uart_send("C ");
        } else if (AnalogValue > 40 && AnalogValue <= 50) {
            uart_send("D ");
        } else if (AnalogValue > 50 && AnalogValue <= 60) {
            uart_send("E ");
        } else if (AnalogValue > 60 && AnalogValue <= 70) {
            uart_send("F ");
        } else if (AnalogValue > 70 && AnalogValue <= 80) {
            uart_send("G ");
        } else if (AnalogValue > 80 && AnalogValue <= 90) {
            uart_send("H ");
        } else if (AnalogValue > 90 && AnalogValue <= 100) {
            uart_send("G ");
        } else {
            uart_send("Z ");
        }

        // Same idea as above
        if (DAC_Value > 10 && DAC_Value <= 20) {
            uart_send("A1 ");
        } else if (DAC_Value > 20 && DAC_Value <= 30) {
            uart_send("B1 ");
        } else if (DAC_Value > 30 && DAC_Value <= 40) {
            uart_send("C1 ");
        } else if (DAC_Value > 40 && DAC_Value <= 50) {
            uart_send("D1 ");
        } else if (DAC_Value > 50 && DAC_Value <= 60) {
            uart_send("E1 ");
        } else if (DAC_Value > 60 && DAC_Value <= 70) {
            uart_send("F1 ");
        } else if (DAC_Value > 70 && DAC_Value <= 80) {
            uart_send("G1 ");
        } else if (DAC_Value > 80 && DAC_Value <= 90) {
            uart_send("H1 ");
        } else if (DAC_Value > 90 && DAC_Value <= 100) {
            uart_send("G1 ");
        } else {
            uart_send("Z1 ");
        }

        // Uncomment if you want to output digital signal based on analog input.
        // DACCON1bits.DACR = DAC_Value;
        __delay_ms(100); // wait a little bit
    }

    return 0;
}