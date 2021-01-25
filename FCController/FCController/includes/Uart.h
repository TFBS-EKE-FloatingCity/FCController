/* 
* uart.h
* Simple UART class for uart on atmega32 and atmega2560
* Overloaded methods write for different datatypes
* no string needed, but every char array must be '\0' terminated
* to ensure correct char array length
* reading can be done by reading only one char or a char array handed over as char* and size
*
* uart must be enabled and can be disabled to start/stop sensing uart port and bind/unbind the rx and tx pins 
*
* with every write method a linefeed can be carried out with the second parameter (boolean), default is no linefeed
* Linefeed and ClearScreen can be executed with extra methods and these are VT100 and above compatible
*
* Created: 08.11.2018
* Version: 1.5
* Updated: 22.03.2020
* Author : gert birkner
*
* use this switch for the uart modules
	switch(module) {
		case uartModule::uart0:
		break;
		#if UARTs_MAX > 0
		case uartModule::uart1:
		break;
		#endif
		#if UARTs_MAX > 1
		case uartModule::uart2:
		break;
		#endif
		#if UARTs_MAX > 2
		case uartModule::uart3:
		break;
		#endif
	}

*/


#ifndef __UART_H__
#define __UART_H__

#include <stdlib.h>
#include "../general.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#  define UARTs_MAX		0
#elif (defined __AVR_ATmega2560__)
#  define UARTs_MAX		3
#endif

#define UART_LF	'\n'
#define UART_CR '\r'

/*
*\brief usable Baudrates
* They depend on the F_CPU of the controller
* don't use high Baudrates when F_CPU is lower
* F_CPU should be about 16 times higher as Baudrate
*/
enum class UartBaudRates : uint16_t {
	br4800 = 48,
	br9600 = 96,
	br14400 = 144,
	br19200 = 192,
	br28800 = 288,
	br38400 = 384,
	br57600 = 576,
	br115200 = 1152,
	br230400 = 2304,
	br460800 = 4608,
	br500000 = 5000,
	br1000000 = 10000
};

/*
*\brief set UART Databits
* AVR UART can have 5 to 9 bits per transaction
* The standard is 8 databits
*/
enum class UartDataBits {
	db5 = 0,
	db6 = 1,
	db7 = 2,
	db8 = 3,
	db9 = 7
};

/*
*\brief set UART Parity
* there are 3 paritybit options
* standard is none
*/
enum class UartParity {
	none = 0,
	even = 2,
	odd = 3
};

/*
*\brief set UART Stopbits
* amount of stopbits to send 
*/
enum class UartStopBit {
	sb1 = 0,
	sb2 = 1
};

/*
*\brief set AVR UART module
* dependent of the controller you can choose the UART module available
* only ATmega2560 and ATmega328 are recognized
*/
enum class uartModule {
	uart0 = 0,
	#if UARTs_MAX > 0
	uart1,
	#endif
	#if UARTs_MAX > 1
	uart2,
	#endif
	#if UARTs_MAX > 2
	uart3
	#endif
};

/*
*\brief
* Uart settings and default values
*/
struct uartSettings {
	uartModule module = uartModule::uart0;
	UartBaudRates baud = UartBaudRates::br9600;
	UartDataBits databits = UartDataBits::db8;
	UartParity parity = UartParity::none;
	UartStopBit stopbits = UartStopBit::sb1;
	bool doubleSpeed = false;
	bool enRx = true;
	bool enTx = true;
};



/*
*\brief
* UART standard class to write char, char*, integers, float and double
* and to read char and char*
*/
class Uart {

//variables
public:
	
private:
	uartModule module = uartModule::uart0;
	bool enRx = true;
	bool enTx = true;

//functions
public:
	Uart(uartSettings settings);
	~Uart();

	void enable();
	void disable();
	void write(unsigned char c, bool linefeed = false);
	void write(char *s, bool linefeed = false);
	void write(uint16_t i, bool linefeed = false);
	void write(int16_t i, bool linefeed = false);
	void write(uint32_t i, bool linefeed = false);
	void write(int32_t i, bool linefeed = false);
	void write(float f, uint8_t precision = 2, bool linefeed = false);
	void write(double d, uint8_t precision = 2, bool linefeed = false);
	void cls();
	void LfCr();
	uint8_t read(unsigned char *x, uint8_t size);
	unsigned char read();


private:
	Uart( const Uart &c );
	Uart& operator=( const Uart &c );

};   //Uart


#endif //__UART_H__
