/* 
* uart.cpp
*
* Created: 06.07.2017 14:01:59
* Author: gert
*/


#include "../includes/Uart.h"


/**
 * \brief 
 *  write single u char when UARTx is ready to use
 * \param c
 *  char or uint8_t (ASCII) to send
 * \param linefeed
 *  if true ... send CR and LF
 *  default value = false
 * \return void
 */
void Uart::write(unsigned char c, bool linefeed /*= false*/) {
	switch(module) {
		case uartModule::uart0:
			while(!(UCSR0A & (1 << UDRE0))) {	}
			UDR0 = c;
			break;
		#if UARTs_MAX > 0
		case uartModule::uart1:
			while(!(UCSR1A & (1 << UDRE1))) {	}
			UDR1 = c;
			break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
			while(!(UCSR2A & (1 << UDRE2))) {	}
			UDR2 = c;
			break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
			while(!(UCSR2A & (1 << UDRE2))) {	}
			UDR2 = c;
			break;
		#endif
	}	
	if (linefeed) {
		LfCr();
	}
}

/**
 * \brief 
 *  send string as char array (call every single char from array
 *  char array must be '\0' terminated
 * \param s
 *  char array '\0' terminated
 * \param linefeed
 *  if true .. send CR and LF
 *  default value = false
 * \return void
 */
void Uart::write(char *s, bool linefeed /*= false*/) {
	while (*s) {
		write((unsigned char)*s, false);
		s++;
	}
	if (linefeed) LfCr();
}


/**
 * \brief 
 *  convert int16_t to char array and call write for char arrays, incl. algebraic sign
 * \param i
 *  integer to send 
 * \param linefeed
 *  is handed over to write for char arrays; 
 *  default value = false
 * \return void
 */
void Uart::write(int16_t i, bool linefeed /*= false*/) {
	char s[7];
	itoa(i, s, 10);
	write(s, linefeed);
}

/**
 * \brief 
 *  convert uint16_t to char array and call write for char arrays
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void Uart::write(uint16_t i, bool linefeed /*= false*/) {
	char s[6];
	itoa(i, s, 10);
	write(s, linefeed);	
}

/**
 * \brief 
 *  convert uint32_t to char array and call write for char arrays
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void Uart::write(uint32_t i, bool linefeed /*= false*/) {
	char s[8];
	itoa(i, s, 10);
	write(s, linefeed);
}

/**
 * \brief 
 *  convert int32_t to char array and call write for char arrays incl algebraic sign
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void Uart::write(int32_t i, bool linefeed /*= false*/) {
	char s[9];
	write(itoa(i, s, 10), linefeed);	
}


/**
 * \brief 
 *  convert float to char array and call write for char arrays incl. algebraic sign
 * \param f
 *  float value to send
 * \param precision
 *  number of decimal places to enclose in array -> and send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void Uart::write(float f, uint8_t precision, bool linefeed /*= false*/) {
	char s[10];
	write(dtostrf(f, 7, precision, s), linefeed);
}


/**
 * \brief 
 *  convert double to char array and call write for char arrays incl. algebraic sign
 * \param d
 *  float value to send
 * \param precision
 *  number of decimal places to enclose in array -> and send
 *  default value is 2 digits
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void Uart::write(double d, uint8_t precision /*= 2*/, bool linefeed /*= false*/) {
	char s[12];
	write(dtostrf(d, 9, precision, s), linefeed);
}

/**
 * \brief 
 *  default constructor 
 *  inits the UART Interface -> Baud, 8 Databits, 1 stop, no parity
 * 
 * \return 
 */
Uart::Uart(uartSettings settings) {
	module = settings.module;
	enRx = settings.enRx;
	enTx = settings.enTx;
	uint16_t UBRR_VAL;
	if (settings.doubleSpeed) {
		UBRR_VAL = (uint16_t)((F_CPU/((uint32_t)settings.baud*800UL))-1); // 103 for Baudrate 19200 und f_cpu = 16MHz
	} else {
		UBRR_VAL = (uint16_t)((F_CPU/((uint32_t)settings.baud*1600UL))-1); // 51 for Baudrate 19200 und f_cpu = 16MHz
	}
	switch(module) {
		case uartModule::uart0:
			UBRR0H = (uint8_t)(UBRR_VAL >> 8);
			UBRR0L = (uint8_t)UBRR_VAL;
			if (settings.doubleSpeed) UCSR0A |= (1 << U2X0);
			UCSR0C |= (((uint8_t)settings.databits & 3) << 1) | ((uint8_t)settings.stopbits << 3) | ((uint8_t)settings.parity << 4);
			UCSR0B |= (((uint8_t)settings.databits & 4) << 2);
		break;
	#if UARTs_MAX > 0
		case uartModule::uart1:
			UBRR1H = (uint8_t)(UBRR_VAL >> 8);
			UBRR1L = (uint8_t)UBRR_VAL;
			if (settings.doubleSpeed) UCSR1A |= (1 << U2X1);
			UCSR1C |= (((uint8_t)settings.databits & 3) << 1) | ((uint8_t)settings.stopbits << 3) | ((uint8_t)settings.parity << 4);
			UCSR1B |= (((uint8_t)settings.databits & 4) << 2);
		break;
	#endif
	#if UARTs_MAX > 1
		case uartModule::uart2:
			UBRR2H = (uint8_t)(UBRR_VAL >> 8);
			UBRR2L = (uint8_t)UBRR_VAL;
			if (settings.doubleSpeed) UCSR2A |= (1 << U2X2);
			UCSR1C |= (((uint8_t)settings.databits & 3) << 1) | ((uint8_t)settings.stopbits << 3) | ((uint8_t)settings.parity << 4);
			UCSR1B |= (((uint8_t)settings.databits & 4) << 2);
		break;
	#endif
	#if UARTs_MAX > 2
		case uartModule::uart3:
			UBRR3H = (uint8_t)(UBRR_VAL >> 8);
			UBRR3L = (uint8_t)UBRR_VAL;
			if (settings.doubleSpeed) UCSR3A |= (1 << U2X3);
			UCSR3C |= (((uint8_t)settings.databits & 3) << 1) | ((uint8_t)settings.stopbits << 3) | ((uint8_t)settings.parity << 4);
			UCSR3B |= (((uint8_t)settings.databits & 4) << 2);
		break;
	#endif
	}
} //uart

// default destructor
Uart::~Uart() {
} //~uart



/**
 * \brief 
 *  reads chars from UART0 into an array until \n or \r is hit or size -1 is reached
 *  and sets an final \0
 * \param x
 *  char array
 * \param size
 *  max. number of chars
 *
 * \return number of read chars without \0
 */
uint8_t Uart::read(unsigned char *x, uint8_t size) {
	uint8_t i = 0;
	if (size == 0) return 0;				// return 0 if no space
	unsigned char c = 0;
	while (i < (size - 1)) {				// check space is available (including additional null char at end)
		c = read();		
		if (c == '\n' || c == '\r') break;  // break on NULL OR ENTER character
		x[i] = c;							// write into the supplied buffer
		i++;
	}
	if (i >= size) i = size - 1;// ensure the last character in the array is '\0'
	x[i] = '\0';	
	return i;				// return number of characters written (incl. '\0')
}


/**
 * \brief 
 *  reads a single char from UART
 * 
 * \return read unsigned char
 */
unsigned char Uart::read() {
	uint8_t r = 0;
	switch(module) {
		case uartModule::uart0:
			while( !(UCSR0A & (1<<RXC0)));
			r = UDR0;
			break;
		#if UARTs_MAX > 0
		case uartModule::uart1:
			while( !(UCSR1A & (1<<RXC1)));
			r = UDR1;
			break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
			while( !(UCSR2A & (1<<RXC2)));
			r = UDR2;
			break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
			while( !(UCSR3A & (1<<RXC3)));
			r = UDR3;
			break;
		#endif
	}
	return r;
}


/**
 * \brief 
 *  enables UARTx receive and transmit modes
 * 
 * \return void
 */
void Uart::enable() {

	switch(module) {
		case uartModule::uart0:
			if(enRx) UCSR0B |= (1<<RXEN0);
			if(enTx) UCSR0B |= (1<<TXEN0);
			break;
		#if UARTs_MAX > 0 
		case uartModule::uart1:
			if(enRx) UCSR1B |= (1<<RXEN1);
			if(enTx) UCSR1B |= (1<<TXEN1);
			break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
			if(enRx) UCSR2B |= (1<<RXEN2);
			if(enTx) UCSR2B |= (1<<TXEN2);
			break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
			if(enRx) UCSR3B |= (1<<RXEN3);
			if(enTx) UCSR3B |= (1<<TXEN3);
			break;
		#endif
	}
}


/**
 * \brief 
 *  disables recieve and transmitt for UARTx
 * 
 * \return void
 */
void Uart::disable() {

	switch(module) {
		case uartModule::uart0:
			UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0)); 
			break;
		#if UARTs_MAX > 0
		case uartModule::uart1:
			UCSR1B &= ~((1<<RXEN1) | (1<<TXEN1)); 
			break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
			UCSR2B &= ~((1<<RXEN2) | (1<<TXEN2)); 
			break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
			UCSR3B &= ~((1<<RXEN3) | (1<<TXEN3)); 
			break;
		#endif
	}


	UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0));  // UART RX, TX 
}


/**
 * \brief 
 *  send clear screen command for VT100 Terminal
 *  with Escape sequences
 * 
 * \return void
 */
void Uart::cls() {
	write((unsigned char)(27));
	write((unsigned char)('['));
	write((unsigned char)('2'));
	write((unsigned char)('J'));
	write((unsigned char)(27));
	write((unsigned char)('['));
	write((unsigned char)('H'));
}



/**
 * \brief 
 *  send CR and LF sequence for VT100 Terminal
 *  to display proper Linefeed
 * 
 * \return void
 */
void Uart::LfCr() {
	write((unsigned char)UART_LF);
	write((unsigned char)UART_CR);
}
