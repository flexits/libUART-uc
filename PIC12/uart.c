/* 
 * File:   uart.c
 * 
 * Author: Alexander Korostelin https://github.com/flexits
 *
 * Created on 06.01.2023, 20:54
 */

#include "uart.h"

//Init UART (change settings if needed - see the datasheet)
void uart_init(void){
    TXREG = 0x00;
    BAUDCON = 0x00;
    BAUDCONbits.BRG16 = 1;      //SYNC = 0, BRGH = 0, BRG16 = 1; SPBRG = 51 for 9600 baud at FOSC = 8 MHz
    SPBRG = 51;
    TXSTA = 0x00;
    RCSTA = 0x00;
    TXSTAbits.TXEN = 1;         //Transmit enabled
    RCSTAbits.CREN = 1;         //Receiver enabled
    RCSTAbits.SPEN = 1;         //Serial port enabled
    INTCONbits.PEIE = 1;        //Peripheral Interrupt Enable
    PIE1bits.RCIE = 1;          //USART receive interrupt enable
}

//A circular buffer
typedef struct {
	volatile uint8_t buffer[BUFFER_SIZE];	//data container
	volatile uint8_t head;					//index to write at
	volatile uint8_t tail;					//index to read at
} buffer_t;

//Push new value into the buffer (data will be lost and ERR returned if the buffer is full)
//Note: one byte between head and tail always stays unused
int8_t buffer_push(buffer_t *buff, uint8_t value){
	//get next head index
    uint8_t head_next = buff->head + 1;
    if (head_next >= BUFFER_SIZE) head_next = 0;
	//check if full
	if (head_next == buff->tail) return ERR;
	//write value and move index
	buff->buffer[buff->head] = value;
	buff->head = head_next;
	return OK;
}

//Get a value from buffer (returns '\0' if the buffer is empty)
uint8_t buffer_pop(buffer_t *buff){
	//check if empty
	if (buff->head == buff->tail) return '\0';
	//get value and move index
	uint8_t val = buff->buffer[buff->tail];
    buff->tail += 1;
    if (buff->tail >= BUFFER_SIZE) buff->tail = 0;
	return val;
}

//Get number of bytes stored inside the buffer
uint8_t buffer_data_length(buffer_t *buff){
	if (buff->head == buff->tail) return 0;		//the most probable case goes first
	int8_t length = (int8_t)(buff->head - buff->tail);
	if (length > 0) return (uint8_t)length;
	return (uint8_t)(length + BUFFER_SIZE);
}

//reception buffer
static buffer_t rxbuff = {"\0", 0, 0};
//transmission buffer
static buffer_t txbuff = {"\0", 0, 0};

void __interrupt() uartIsr(void){
    if (TXIE && TXIF){
        //write a byte of data and enable the interrupt or
        //disable the interrupt if there's no data to send
        TXREG = buffer_pop(&txbuff);
        if (buffer_data_length(&txbuff)){
            PIE1bits.TXIE = 1;
        } else {
            PIE1bits.TXIE = 0;
        }
    }
    if (RCIE && RCIF){
        //read the incoming data 
        //(will be dropped if the reception buffer is full)
        uint8_t framerr = FERR;
        uint8_t value = RCREG;
        //ignore characters with framing errors
        if (!framerr) buffer_push(&rxbuff, value);
        //reset receiver if overrun occured
        if (OERR){
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
    }
    return;
}

int8_t uart_send_byte(uint8_t value){
    //push new data into the buffer and invoke transmission
	if (buffer_push(&txbuff, value) == OK){
        PIE1bits.TXIE = 1;
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