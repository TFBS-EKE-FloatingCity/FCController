/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 

#include <avr/io.h>
#include "general.h"

int8_t rData[2] = {0, 0};
	
	
// Inside sensor (0 - 300)
// Outside sensor (0 - 300)
// Current pump speed (-100 - 100)
// Current windmill speed (0 - 100)
int8_t tData[4] = {0, 0, 0, 0};

int main(void)
{
    /* Replace with your application code */
	// 1 Messrunde
    while (1) 
    {
		// *** TESTING *** //
		tData[0] = 250;
		tData[1] = 175;
		tData[2] = -55;
		tData[3] = 50;
		// *** TESTING *** //		
		
		/************************************************************************/
		/*			   2 Bytes empfangen und in rData schreiben                 */
		/*						4 Bytes aus tData senden                        */
		/************************************************************************/
		// We will send 4 bytes so count up to 3
		for (uint8_t idx = 0; idx < 4; idx++) {
			
			// Write 1st byte into register
			SPDR = tData[idx];
			
			// Wait for transmission
			while(!(SPSR & (1<<SPIF)));
			
			// Because only the first 2 bytes are real data => check if its the 1st or 2nd byte
			if (idx < 2) {
				// Read register
				rData[idx] = SPDR;
			}
		}
		
		//TODO: Pumpe setzen
		//TODO: Windrad setzen
		//TODO: LED Lauflicht sezten
		//TODO: USonic Messung
		//TODO: für Test: die beiden rData Bytes in die ersten beiden tData Bytes schreiben
		
    }
}

