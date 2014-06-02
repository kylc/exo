/* LED Blink Example with USB Debug Channel for Teensy USB Development Board
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008, 2010 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "print.h"

#include "safety.h"
#include "glove.h"
#include "icu.h"
#include "ESC.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define LED_R_PIN 1
#define LED_G_PIN 2
#define LED_B_PIN 3

//ISR(PCINT0_vect) {
//	print("Changed\n");
//}

volatile uint16_t rpm = 0;
volatile int16_t dir = 1;   // Should be -1 or 1
volatile int16_t dc = 0;

ISR(TIMER3_CAPT_vect)
{
	PORTB ^= _BV(3);

	//if (TCCR3B & (1<<ICES3)) {
	//		TCCR3B &= ~(1<<ICES3);
	//	tStart = t;
	//} else {
	//	TCCR3B |= (1<<ICES3);
	//	tPulse = t - tStart;
	//}

	rpm = (F_CPU/256) * 60 / ICR3;
	TCNT3 = 0;   // Reset timer
}

// On overflow, reset rpm.
ISR(TIMER3_OVF_vect)
{
	rpm = 0;
}

int main(void)
{
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	sei();

	// initialize the USB, but don't want for the host to
	// configure.  The first several messages sent will be
	// lost because the PC hasn't configured the USB yet,
	// but we care more about blinking than debug messages!
	usb_init();

	// LEDs
	DDRB |= _BV(1) | _BV(2) | _BV(3);
	PORTB |= _BV(0);
	PORTB &= ~(_BV(1) | _BV(2) | _BV(3));   // Start LEDs off

	// ICU on Timer3, pin C7
	PORTC |= _BV(7);   // Enable pullup
	DDRC &= ~_BV(7);   // Set as input
	setup_icu();
	initESC();

	// Debug
	DDRD |= _BV(0);

	safety_init();

	struct glove_state_t glove_state;
	glove_init();

	armESC();

	int state = 0;
	while(1) {
		glove_update(&glove_state);

		print("ADC: ");
		phex(glove_state.raw_input);
		print("\n");

		// print("Direction: ");
		// phex(glove_direction(&glove_state));
		// print("\n");

		// print("RPM: ");
		// phex16(dc);
		// print("\n");

		// 0x85 = 1064 us
		// 0xe8 = 1856 us
		PORTD |= _BV(0);
		PORTD &= ~_BV(0);

		if(safety_enabled()) {
		  enum direction dir = glove_direction(&glove_state);
		  if(dir == STOP) {
		    setESC(0);
		    print("Stopped\n");
		    PORTB |= _BV(1);
		    PORTB &= ~(_BV(2) | _BV(3));   // Start LEDs off
		  } else if(dir == UP) {
		    setESC(100);
		    print("Moving up\n");
		    PORTB |= _BV(2);
		    PORTB &= ~(_BV(1) | _BV(3));   // Start LEDs off
		  } else if(dir == DOWN) {
		    setESC(-100);
		    print("Moving down\n");
		    PORTB |= _BV(3);
		    PORTB &= ~(_BV(1) | _BV(2));   // Start LEDs off
		  }
		} else {
		    setESC(0);
		    print("Disarmed\n");
		}

		_delay_ms(5);
	}

	return 0;
}
