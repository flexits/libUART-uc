/*
 * uart.h
 *
 * UART communication module (interrupt-driven).
 *
 * There are two circular buffers, for the data being sent and received correspondingly. 
 * If they become full (data is produced faster that consumed), each new byte will be 
 * silently discarded. Increase buffers sizes if you encounter a problem with that.
 *
 * It's required to complete the initialization by calling uart_init() beforehands!
 *
 * Each function returns OK/ERR codes by default.
 *
 * Created: 05.01.2023 13:26:36
 * Author: Alexander Korostelin https://github.com/flexits
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

//return codes
#define ERR	(-1)
#define OK	0

//operate the specified bit of the specified register (byte)
#define set_bit(reg, bit) ((reg) |= _BV(bit))		//set the bit to 1
#define clr_bit(reg, bit) ((reg) &= ~_BV(bit))		//set the bit to 0

//buffer length
#define BUFFER_SIZE 24

/*
 * Obligatory initialization procedure.
 * Doesn't enable interrupts though - don't forget to do it manually!
 * For uC's other than ATmega 88PA/168PA registers' names might change, see datasheet!
 */
void uart_init(void);

/*
 * Send a single byte via UART.
 * If the transmission buffer is full, the data will be lost and ERR will be returned.
 */
int8_t uart_send_byte(uint8_t value);

/*
 * Send a string with a new line after it; returns ERR if null pointer passed, otherwise OK.
 * The source string must be null-terminated!
 */
int8_t uart_send_line(char *buff_p);

/*
 * Get number of unread data bytes in the RX buffer
 */
uint8_t uart_get_rx_data_length(void);

/*
 * Read a single byte from the reception buffer.
 * Returns '\0' if the buffer is empty.
 */
uint8_t uart_read_byte(void);

#endif /* UART_H_ */