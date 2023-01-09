/*
 * File:   main.c
 * 
 * Interrupt-driven UART communication example for PIC12F1840
 * (for another uC's configure options in uart.c accordingly to the datasheet)
 * 
 * Note: mid-range PICs have only one vector location which is linked to all 
 * interrupt sources, so we have to use a single interrupt function to process 
 * all interrupt sources. This function is implemented inside uart.c - 
 * consider refactoring to ensure compatibility with existing projects!
 * 
 * Author: Alexander Korostelin https://github.com/flexits
 *
 * Created on 06.01.2023, 20:54
 */

// PIC12F1840 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1 (07E4)
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = ON    // Clock Out Enable (CLKOUT function is enabled on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2 (3EFF)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.

#include <xc.h>
#include "uart.h"

void main(void) {
    
    ADCON0 = 0x00;              // ADC off
    DACCON0 = 0x00;             // DAC off
    FVRCON = 0x00;              // FVR off
    CM1CON0 = 0x00;             // Comparator off
    MDCON = 0x00;               // DSM off
    ANSELA = 0x00;              // All ports are digital I/O
    TRISA = 0b00001010;         // All are outputs except RA1 & RA3
    PORTA = 0x00;               // Write 0 to all ports
    OSCTUNE = 0x00;
    OSCCON = 0x00;
    OSCCONbits.IRCF = 0b1110;   // 8 MHz clock
    
    uart_init();                //init interface
    
    ei();                       //enable global interrupts
    
    uart_send_line("I'm alive!"); //test string send
    
    while(1)
    {
        while (uart_get_rx_data_length())	//terminal echo: while reception buffer isn't empty, 
		{									//read bytes and send them back 
			uart_send_byte(uart_read_byte());
		}
    }
}
