/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 

#include <avr/io.h>
#include "general.h"

uint8_t rData[2] = {0, 0};
uint8_t tData[4] = {0, 0, 0, 0}

int main(void)
{
	uint8_t idx = 0;
    /* Replace with your application code */
	// 1 Messrunde
    while (1) 
    {

		while(!(SPSR & (1<<SPIF)));
		rData[idx] = SPDR;	
		//TODO: 2 Bytes empfangen und in rData schreiben
		//TODO: 4 Bytes aus tData senden
		
		//TODO: Pumpe setzen
		//TODO: Windrad setzen
		//TODO: LED Lauflicht sezten
		//TODO: USonic Messung
		//TODO: für Test: die beiden rData Bytes in die ersten beiden tData Bytes schreiben
		
    }
}

