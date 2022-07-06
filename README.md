<style>
    td {
        border: 2px solid;
        text-align: center;
    }
    th {
        text-align: center;
    }
</style>

# Activate/Disable PUMP/Generator PWM
because the Mosfets available at the moment are not industrial-Mosfets and they can't handle the fast frequency of PWM on the Pumps/Generators, a bit was implemented in code to determine if the code runs with PWM or without PWM.
when running without PWM the city will not be as stable as running with PWM.
## board.h
```
// PWM Activation Bit
#define PUMP_PWM_ACTIVE 0
```
## main.cpp
```
if(PUMP_PWM_ACTIVE)	//PWM Mode
	{
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM12) |( 1 << WGM13);			// FastPWM mode 14
		TCCR1A |= (1 << COM1A1);						// set OC1A (PB5) as timer1 PWM output (generator)
		TCCR1A |= (1 << COM1B1);						// set OC1B (PB6) as timer 1 PWM output (pump)
		ICR1 = PUMP_ICR -1;								// F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR
		OCR1A = 0;										// DutyCyle = 0%
		OCR1B = 0;										// DutyCycle = 0%
	}
```

# Measuring Arduino / ATmega 2560
## Pinout
<table>
    <thead>
        <tr>
        <th>PIN</th>
        <th>Port</th>
        <th>Function</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td colspan="3">SPI</td>
        </tr>
        <tr>
            <td>IDE 50</td>
            <td>PB3</td>
            <td>MISO</td>
        </tr>
        <tr>
            <td>IDE 51</td>
            <td>PB2</td>
            <td>MOSI</td>
        </tr>
        <tr>
            <td>IDE 52</td>
            <td>PB1</td>
            <td>CLK</td>
        </tr>
        <tr>
            <td>IDE 53</td>
            <td>PB0</td>
            <td>SS</td>
        </tr>
        <tr>
            <td colspan="3">Ultra Sonic</td>
        </tr>
        <tr>
            <td>IDE 14</td>
            <td>PJ1</td>
            <td>Outer US Trigger</td>
        </tr>
        <tr>
            <td>IDE 15</td>
            <td>PJ0</td>
            <td>Inner US Trigger</td>
        </tr>
        <tr>
            <td>IDE 20</td>
            <td>PD1</td>
            <td>Outer US Echo</td>
        </tr>
        <tr>
            <td>IDE 21</td>
            <td>PD0</td>
            <td>Inner US Echo</td>
        </tr>
        <tr>
            <td colspan="3">Pump LED's</td>
        </tr>
        <tr>
            <td>Analog 0</td>
            <td>PF0</td>
            <td>P1(LED board)</td>
        </tr>
        <tr>
            <td>Analog 1</td>
            <td>PF1</td>
            <td>P2(LED board)</td>
        </tr>
        <tr>
            <td>Analog 2</td>
            <td>PF2</td>
            <td>P3(LED board)</td>
        </tr>
        <tr>
            <td>Analog 3</td>
            <td>PF3</td>
            <td>P4(LED board)</td>
        </tr>
        <tr>
            <td>Analog 4</td>
            <td>PF4</td>
            <td>P5(LED board)</td>
        </tr>
        <tr>
            <td>Analog 5</td>
            <td>PF5</td>
            <td>P6(LED board)</td>
        </tr>
        <tr>
            <td>Analog 6</td>
            <td>PF6</td>
            <td>P7(LED board)</td>
        </tr>
        <tr>
            <td>Analog 7</td>
            <td>PF7</td>
            <td>P8(LED board)</td>
        </tr>
        <tr>
            <td>Analog 8</td>
            <td>PK0</td>
            <td>P9(LED board)</td>
        </tr>
        <tr>
            <td>Analog 9</td>
            <td>PK1</td>
            <td>P10(LED board)</td>
        </tr>
        <tr>
            <td>Analog 10</td>
            <td>PK2</td>
            <td>P11(LED board)</td>
        </tr>
        <tr>
            <td>Analog 11</td>
            <td>PK3</td>
            <td>P12(LED board)</td>
        </tr>
        <tr>
            <td>Analog 12</td>
            <td>PK4</td>
            <td>P13(LED board)</td>
        </tr>
        <tr>
            <td>Analog 13</td>
            <td>PK5</td>
            <td>P14(LED board)</td>
        </tr>
        <tr>
            <td>Analog 14</td>
            <td>PK6</td>
            <td>P15(LED board)</td>
        </tr>
        <tr>
            <td>Analog 15</td>
            <td>PK7</td>
            <td>P16(LED board)</td>
        </tr>
        <tr>
            <td>IDE 16</td>
            <td>PH1</td>
            <td>P17(LED board) LED_Pump</td>
        </tr>
        <tr>
            <td>IDE 17</td>
            <td>PH0</td>
            <td>P17(LED board) LED_Generator</td>
        </tr>
        <tr>
            <td>GND</td>
            <td>GND</td>
            <td>P18(LED board)</td>
        </tr>
        <tr>
            <td colspan="3">PWM</td>
        </tr>
        <tr>
            <td>IDE 5</td>
            <td>PE3</td>
            <td>Wind Mill PWM</td>
        </tr>
        <tr>
            <td>IDE 10</td>
            <td>PB4</td>
            <td>Wind Mill Led</td>
        </tr>
        <tr>
            <td>IDE 11</td>
            <td>PB5</td>
            <td>Generator PWM</td>
        </tr>
        <tr>
            <td>IDE 12</td>
            <td>PB6</td>
            <td>PUMP PWM</td>
        </tr>
    </tbody>
</table>

# Data Range
## Pump & Generator

from 1 to 100 => Pump active / Generator inactive

from 101 to 200 => Pump inactive / Generator active

0 => Pump inactive / Generator inactive

# Watchdog
To ensure the "while" loop runs smoothly, the watchdog is initialized to [8] seconds, if the code in the "while" loop takes longer than [8] seconds to execute, the Arduino will automatically reset.

the watchdog is reset at each complete cycle.
```
//Init watchdog with 8 seconds
wdt_reset();
wdt_enable(WDTO_8S);
//....code...//
while(1)
{
    wdt_reset(); // Reset watchdog
    //....code...//
}
```

# Timers
## PWM Timers
![PWMSelectMode.png](/.attachments/PWMSelectMode.png)
#### Clock Select table
![ClockSelectTable.png](/.attachments/ClockSelectTable.png)

### Pump/Generator Timer
#### Registers
<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TCCR1A Timer/Counter 1 Control Register A</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>COM1A1</td>
            <td>COM1A0</td>
            <td>COM1B1</td>
            <td>COM1B0</td>
            <td>COM1C1</td>
            <td>COM1C0</td>
            <td>WGM11</td>
            <td>WGM10</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>1</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="2">Compare Output Mode for Channel A</td>
            <td colspan="2">Compare Output Mode for Channel B</td>
            <td colspan="2">Compare Output Mode for Channel C</td>
            <td colspan="2">Waveform Generation Mode</td>
        </tr>
    </tbody>
</table>

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TCCR1B Timer/Counter 1 Control Register B</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>ICNC1</td>
            <td>ICES1</td>
            <td>–</td>
            <td>WGM13</td>
            <td>WGM12</td>
            <td>CS12</td>
            <td>CS11</td>
            <td>CS10</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>0</td>
            <td>-</td>
            <td>1</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td>Input Capture Noise Canceler</td>
            <td>Input Capture Edge Select</td>
            <td>Reserved Bit</td>
            <td colspan="2">Waveform Generation Mode</td>
            <td colspan="3">Clock Select</td>
        </tr>
    </tbody>
</table>

#### Calculation
To Calculate Fast PWM Top (ICR1) with a desired frequency of 2KHz

$ f_{DESIRED} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * (1 + \text{TOP})} 
\\~\\
\text{TOP} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * f_{DESIRED}} - 1 \\~\\ 
\text{TOP} = \dfrac{16000000}{1 * 2000} -1 = 7999 $

#### Result
1. PWM Mode: FastPWM mode (14 in PWM Select Mode Table Above)
2. PreScaler: 1 
3. OC1A (PB5) as timer1 pwm output (Generator)
4. OC1B (PB6) as timer 1 pwm output (pump)
5. F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR => ICR1 = 7999


### Windmill Timer
#### Registers
<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TCCR3A Timer/Counter 3 Control Register A</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>COM3A1</td>
            <td>COM3A0</td>
            <td>COM3B1</td>
            <td>COM3B0</td>
            <td>COM3C1</td>
            <td>COM3C0</td>
            <td>WGM31</td>
            <td>WGM30</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="2">Compare Output Mode for Channel A</td>
            <td colspan="2">Compare Output Mode for Channel B</td>
            <td colspan="2">Compare Output Mode for Channel C</td>
            <td colspan="2">Waveform Generation Mode</td>
        </tr>
    </tbody>
</table>

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TCCR3B Timer/Counter 3 Control Register B</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>ICNC3</td>
            <td>ICES3</td>
            <td>–</td>
            <td>WGM33</td>
            <td>WGM32</td>
            <td>CS32</td>
            <td>CS31</td>
            <td>CS30</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>0</td>
            <td>-</td>
            <td>1</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td>Input Capture Noise Canceler</td>
            <td>Input Capture Edge Select</td>
            <td>Reserved Bit</td>
            <td colspan="2">Waveform Generation Mode</td>
            <td colspan="3">Clock Select</td>
        </tr>
    </tbody>
</table>

#### Calculation
To Calculate Fast PWM Top (ICR3) with a desired frequency of 2KHz

$f_{DESIRED} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * (1 + \text{TOP})} 
\\~\\
\text{TOP} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * f_{DESIRED}} - 1 \\~\\ 
\text{TOP} = \dfrac{16000000}{1 * 2000} -1 = 7999$

#### Result
1. PWM Mode: FastPWM mode (14 in PWM Select Mode Table Above)
2. PreScaler: 1
3. OC3A (PE3) as timer3 pwm output (windmill)
4. F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR => ICR3 = 7999

## Interrupt Timers
### Usonic Sensor Timer
#### Registers

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;"> TIMSK4 Timer/Counter 4 Interrupt Mask Register</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>-</td>
            <td>-</td>
            <td>ICIE4</td>
            <td>-</td>
            <td>OCIE4C</td>
            <td>OCIE4B</td>
            <td>OCIE4A</td>
            <td>TOIE4</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>-</td>
            <td>-</td>
            <td>0</td>
            <td>-</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="2">Reserved Bit</td>
            <td>Input Capture Interrupt Enable</td>
            <td>Reserved Bit</td>
            <td>Output Compare C Match Interrupt Enable</td>
            <td>Output Compare B Match Interrupt Enable</td>
            <td>Output Compare A Match Interrupt Enable</td>
            <td>Overflow Interrupt Enable</td>
        </tr>
    </tbody>
</table>

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">EICRA External Interrupt Control Register A</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>ISC31</td>
            <td>ISC30</td>
            <td>ISC21</td>
            <td>ISC20</td>
            <td>ISC11</td>
            <td>ISC10</td>
            <td>ISC01</td>
            <td>ISC00</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="8">External Interrupt 3 - 0 Sense Control Bits</td>
        </tr>
    </tbody>
</table>

#### Result
1. INT0 on PD0 and INT1 on PD1 at any edge for Sensor 1 and 2
2. 30cm max depth of water => 60cm way of sound => 2ms duration
3. 16Bit timer with prescaler 1 (16MHz clock) => after 4ms Overflow => cancel meassurement and write 0 to array
4. no Timer output required, only set prescaler to start timer and configure Overflow Interrupt to end measurement

### Interrupts
#### Overflow: ISR(TIMER4_OVF_vect)
when an overflow interrupt is triggered, it means that the USonic measurement took too long and should be stopped/restarted
```
ISR(TIMER4_OVF_vect) {
	TCCR4B &= ~(1 << CS40);		// stop timer
	TCNT4 = 0;					// reset timer value
	waiting = 0;
}
```

#### INT0: ISR(INT0_vect)

INT0 interrupt is triggered when the level of water/distance on the inner USonic measurement changes.

Calculation Distance:

$
\text{Speed of Sound} = 343 m/s
\\~\\
\text{High level Time} = \text{steps count} * \text{step duration}
\\~\\
\text{High level Time} = \text{Timer Value} * 62.5ns = \text{Timer Value} * 0.0625ms
\\~\\
\text{Distance} = \dfrac{\text{High level Time} * \text{Speed of Sound}}{2}
\\~\\
\text{Distance} = \dfrac{\text{Timer Value} * 0.0625ms * 343}{2000} + 0.5
$

Note: the propose of the 0.5 is to round the result number.

```
ISR(INT0_vect) {
	if(PIND & (1 << PD0)) {		// rising edge
		TCCR4B |= (1 << CS40);	// start timer
	} else {					// falling edge
		TCCR4B &= ~(1 << CS40);	// stop timer
		uint16_t dist = (uint16_t)((((double)((uint32_t)TCNT4 * 343UL) * 0.0625)/2000.0) + 0.5);
		tData[0] = (dist >> 8);
		tData[1] = dist;
		TCNT4 = 0;
		waiting = 0;
	}
}
```

### LED Timer
#### Registers

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TCCR5B Timer/Counter 5 Control Register B</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>ICNC5</td>
            <td>ICES5</td>
            <td>-</td>
            <td>WGM53</td>
            <td>WGM52</td>
            <td>CS52</td>
            <td>CS51</td>
            <td>CS50</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>0</td>
            <td>-</td>
            <td>1</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td>Input Capture Noise Canceler</td>
            <td>Input Capture Edge Select</td>
            <td>Reserved Bit</td>
            <td colspan="2">Waveform Generation Mode</td>
            <td colspan="3">Clock Select</td>
        </tr>
    </tbody>
</table>

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">TIMSK5 Timer/Counter 5 Interrupt Mask Register</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>-</td>
            <td>-</td>
            <td>ICIE5</td>
            <td>-</td>
            <td>OCIE5C</td>
            <td>OCIE5B</td>
            <td>OCIE5A</td>
            <td>TOIE5</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>-</td>
            <td>-</td>
            <td>1</td>
            <td>-</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="2">Reserved Bit</td>
            <td>Input Capture Interrupt Enable</td>
            <td>Reserved Bit</td>
            <td>Output Compare C Match Interrupt Enable</td>
            <td>Output Compare B Match Interrupt Enable</td>
            <td>Output Compare A Match Interrupt Enable</td>
            <td>Overflow Interrupt Enable</td>
        </tr>
    </tbody>
</table>

<table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">EICRA External Interrupt Control Register A</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>ISC31</td>
            <td>ISC30</td>
            <td>ISC21</td>
            <td>ISC20</td>
            <td>ISC11</td>
            <td>ISC10</td>
            <td>ISC01</td>
            <td>ISC00</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td colspan="8">External Interrupt 3 - 0 Sense Control Bits</td>
        </tr>
    </tbody>
</table>

#### Calculation
To Calculate Fast PWM Top (ICR5) with a desired frequency of 320Hz

$f_{DESIRED} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * (1 + \text{TOP})} 
\\~\\
\text{TOP} = \dfrac{\text{SYSTEM-CLOCK}}{\text{PRESCALER} * f_{DESIRED}} - 1 \\~\\ 
\text{TOP} = \dfrac{16000000}{1 * 320} -1 = 4999$

#### Result
1. CTC mode 12 (see table above)
2. minValue for 1% (e.g.0.05s per LED)
3. maxValue for 100% (e.g. 0.5s per LED)
4. Both directions with outputs PF0 .. PF7 and PK0 .. PK7, plugged vice versa for other direction
5. two pins for MOSFETs to select the LED stripe

### Interrupts
#### CAPT: ISR(TIMER5_CAPT_vect)
To shift the LED's.
```
ISR(TIMER5_CAPT_vect) {
	leds <<= 1;
	if(leds < 3) { 
		leds = 3;
	} else if (leds == 0x8000) {
		leds = 0x8001;
	} 
	PORTF = leds;
	PORTK = (leds >> 8);		
}
```
#### INT2: ISR(INT2_vect)
INT2 interrupt is triggered when a transmission starts.
when a transmission starts, it is important to stop the LED's timer so it doesn't interfere with the transmission.
```
ISR(INT2_vect) {
	TCCR5B &= ~((1 << CS50) | (1 << CS51));		// stop LED Timer
}
```

# SPI
The communication between the Raspberry and the rest of the Arduino's is achieved using SPI.
To activate SPI:
 <table>
    <thead>
        <tr>
            <th colspan=8 style="text-align: center;">SPCR – SPI Control Register</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Pins:</td>
            <td>SPIE</td>
            <td>SPE</td>
            <td>DORD</td>
            <td>MSTR</td>
            <td>CPOL</td>
            <td>CPHA</td>
            <td>SPR1</td>
            <td>SPR0</td>
        </tr>
        <tr>
            <td>Data</td>
            <td>0</td>
            <td>1</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
            <td>1</td>
        </tr>
        <tr>
            <td>Description:</td>
            <td>SPI Interrupt Enable</td>
            <td>SPI Enable</td>
            <td>Data Order</td>
            <td>Master/Slave Select</td>
            <td>Clock Polarity</td>
            <td>Clock Phase</td>
            <td colspan="2">SPI Clock Rate Select</td>
        </tr>
    </tbody>
</table>

#### Result
1. SPI Enabled
2. Oscillator Frequency: 16

# Pump/Generator Duty Cycle
Like mentioned above the data received from the Raspberry has the following range:

from 1 to 100 => Pump active / Generator inactive

from 101 to 200 => Pump inactive / Generator active

so to round the number in case that the pump is active => 100 - Received data

Example:

$\text{Pump Power \%} = 100 - \text{Received data}
\\~\\
\text{Pump Power \%} = 100 - 30 = 70 \%$

and the opposite is applied to generator => Received data - 100

Example:

$\text{Generator Power \%} = \text{Received data} - 100
\\~\\
\text{Generator Power \%} = 170 - 100 = 70 \%$

the duty cycle of the pump or generator should always be between the value of ICR and the value of FASTEST_OCR so#

Pump:

$\text{Duty Cycle} = \dfrac{(100 - \text{Received data}) * (ICR - FASTEST\_OCR)}{100} + FASTEST\_OCR$

Example:

$\text{Duty Cycle} = \dfrac{(100 - 30) * (8000 - 4000)}{100} + 4000
\\~\\
\text{Duty Cycle} = \dfrac{70 * 4000}{100} + 4000 = 6800$

Generator:

$\text{Duty Cycle} = \dfrac{(\text{Received data} - 100) * (ICR - FASTEST\_OCR)}{100} + FASTEST\_OCR$

Example:

$\text{Duty Cycle} = \dfrac{(170 - 100) * (8000 - 4000)}{100} + 4000
\\~\\
\text{Duty Cycle} = \dfrac{70 * 4000}{100} + 4000 = 6800$
# Program Diagram
![ProgramDiagram.png](/.attachments/ProgramDiagram.png)
