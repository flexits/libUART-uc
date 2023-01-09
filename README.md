# libUART-uc
UART peripheral library for different microcontrollers. Uses interrupt-driven approach without wait cycles and delay macroses. Utilizes two circular buffers to store data being sent and received. The source code is extensively commented to help beginners.  
Each version has a demo implementation that sends a welcome string and then just echoes back all characters being sent. A compiled binary and a minimal Proteus 8 project to test it are included.  
Default configuration is 9600 baud 8N1N, it may be adjusted inside `uart_init(void)` (refer to the uC datasheet). The internal uC oscillator is used for clock by default. Of course it may be also adjusted but please be aware that this will require corresponding UART configuration changes (yes again, see the datasheet).

## What it is capable of
- initialize UART hardware module
- send a single byte via UART
- send a null-terminated string and a newline after it
- get a number of bytes containing in the reception buffer
- pop a byte from the reception buffer

## Compatible hardware
Directory | Tested with  | IDE
--- | --- | ---
AVR_ATmega | ATmega 88PA/168PA | Microchip Studio 7
PIC12 | PIC12F1840 | MPLAB X IDE 6  

*To be continued when I'll have time and hardware access.*  

## Contribution
Feel free to modify the implementation for any other uC and contribute it!
