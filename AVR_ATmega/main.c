/*
 * Interrupt-driven UART communication example for ATmega
 * (tested on ATmega 88PA/168PA, 
 * for another uC's configure registers in uart.c accordingly to the datasheet)
 *
 * Created: 06.01.2023 12:04:29
 * Author: Alexander Korostelin https://github.com/flexits
 */ 

#include "uart.h"
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

int main(void)
{
	SMCR = _BV(SE);					//enable Idle mode
	
	uart_init();					//init interface
	sei();							//enable interrupts
	uart_send_line("I'm alive!");	//test string send

    while(1)
	{
		while (uart_get_rx_data_length())	//terminal echo: while reception buffer isn't empty, 
		{									//read bytes and send them back 
			uart_send_byte(uart_read_byte());
		}
	    asm("sleep");				//go to idle mode until reception
	}
}

