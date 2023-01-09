/*
 * uart.c
 *
 * UART communication module
 *
 * Created: 05.01.2023 13:33:40
 * Author: Alexander Korostelin https://github.com/flexits
 */ 

#include "uart.h"

//Init UART (change settings if needed - see the datasheet)
#define BAUDRATE 103 //UBRRn=103 for 9600 bits at 8 MHz in async double speed mode
void uart_init(void){
	UBRR0H = (uint8_t)(BAUDRATE>>8);
	UBRR0L = (uint8_t)BAUDRATE;
	UCSR0A = _BV(U2X0);							//async double speed mode
	UCSR0B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0);	//Transmitter and Receiver Enable, RX Complete Interrupt Enable
	UCSR0C = _BV(UCSZ00)|_BV(UCSZ01);			//async, parity disabled, 1 stop bit, 8 data bits
}

//A circular buffer
typedef struct {
	volatile uint8_t buffer[BUFFER_SIZE];	//data container
	volatile uint8_t head;					//index to write at
	volatile uint8_t tail;					//index to read at
} buffer_t;

//Push new value into the buffer (data will be lost and ERR returned if the buffer is full)
//Note: one byte between head and tail always stays unused
static int8_t buffer_push(buffer_t *buff, uint8_t value){
	//get next head index
	uint8_t head_next = buff->head + 1;
	if (head_next == BUFFER_SIZE) head_next = 0;
	//check if full
	if (head_next == buff->tail) return ERR;
	//write value and move index
	buff->buffer[buff->head] = value;
	buff->head = head_next;
	return OK;
}

//Get a value from buffer (returns '\0' if the buffer is empty)
static uint8_t buffer_pop(buffer_t *buff){
	//check if empty
	if (buff->head == buff->tail) return '\0';
	//get value and move index
	uint8_t val = buff->buffer[buff->tail];
	buff->tail += 1;
	if (buff->tail == BUFFER_SIZE) buff->tail = 0;
	return val;
}

//Get number of bytes stored inside the buffer
static uint8_t buffer_data_length(buffer_t *buff){
	if (buff->head == buff->tail) return 0;		//the most probable case goes first
	int8_t length = buff->head - buff->tail;
	if (length > 0) return length;
	return length + BUFFER_SIZE;
}

//reception buffer
static buffer_t rxbuff = {"\0", 0, 0};
//transmission buffer
static buffer_t txbuff = {"\0", 0, 0};

//Read the incoming data (will be dropped if the reception buffer is full)
ISR(USART_RX_vect){
	uint8_t received_data = UDR0;
	buffer_push(&rxbuff, received_data);
}

ISR(USART_UDRE_vect){
	//disable the interrupt if there's no data to send, or write pending data and enable the interrupt
	clr_bit(UCSR0B, UDRIE0);
	if (buffer_data_length(&txbuff) == 0) return;
	UDR0 = buffer_pop(&txbuff);
	set_bit(UCSR0B, UDRIE0);
}

int8_t uart_send_byte(uint8_t value) {
	//push new data into the buffer and invoke transmission
	if (buffer_push(&txbuff, value) == OK){
		set_bit(UCSR0B, UDRIE0);
		return OK;
	}
	return ERR;
}

int8_t uart_send_line(char *buff_p){
	if (!buff_p) return ERR;
	//transmit all chars one by one
	char ch='\0';
	while ((ch = *buff_p++) != '\0')
	{
		uart_send_byte(ch);
	}
	//append with a newline
	uart_send_byte('\r');
	uart_send_byte('\n');
	return OK;
}

uint8_t uart_read_byte(void) {
	return buffer_pop(&rxbuff);
}

uint8_t uart_get_rx_data_length(void) {
	return buffer_data_length(&rxbuff);
}