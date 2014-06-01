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

#include "glove.h"
#include "icu.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define LED_R_PIN 1
#define LED_G_PIN 2
#define LED_B_PIN 3

//ISR(PCINT0_vect) {
//	print("Changed\n");
//}

volatile uint16_t rpm = 0;
volatile int8_t dir = 1;   // Should be -1 or 1

ISR(TIMER1_CAPT_vect)
{
	PORTB ^= _BV(3);

	//if (TCCR3B & (1<<ICES3)) {
	//		TCCR3B &= ~(1<<ICES3);
	//	tStart = t;
	//} else {
	//	TCCR3B |= (1<<ICES3);
	//	tPulse = t - tStart;
	//}

	rpm = (F_CPU/256) * 60 / ICR1;
	TCNT1 = 0;   // Reset timer
}

// On overflow, reset rpm.
ISR(TIMER1_OVF_vect)
{
	rpm = 0;
}

int main(void)
{
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT4);
	sei();

	// initialize the USB, but don't want for the host to
	// configure.  The first several messages sent will be
	// lost because the PC hasn't configured the USB yet,
	// but we care more about blinking than debug messages!
	usb_init();

	// LEDs
	DDRB |= _BV(1) | _BV(2) | _BV(3);
	PORTB &= ~(_BV(1) | _BV(2) | _BV(3));   // Start LEDs off

	// ICU on Timer1, pin D4
	PORTD |= _BV(4);   // Enable pullup
	DDRD &= ~_BV(4);   // Set as input
	DDRD |= _BV(0);   // Set test output pin
	setup_icu();

	// PWM out on Timer3, pin C6
	OCR3AL = 0x00;   //Load Pulse width
	OCR3AH = 0;
	DDRC |= _BV(6);   //Port C6 as o/p
	TCCR3A = 0x81;   //8-bit, Non-Inverted PWM
	TCCR3B = 1;   //Start PWM

	// PINF1 acc z

	glove_init();

	struct glove_state_t glove_state;

	int state = 0;
	while(1) {
		//switch(state) {
		//	case 0:
		//		PORTB = _BV(LED_R_PIN);
		//		state = 1;
		//		break;
		//	case 1:
		//		PORTB = _BV(LED_G_PIN);
		//		state = 2;
		//		break;
		//	case 2:
		//		state = 0;
		//		PORTB = _BV(LED_B_PIN);
		//		break;
		//}

		glove_update(&glove_state);

		//print("PWM: ");
		//phex(glove_state.raw_input);
		//print("\n");

		print("RPM: ");
		phex16(rpm);
		print("\n");

		PORTD |= _BV(0);
		PORTD &= ~_BV(0);
		OCR3AL = 0x81;
		_delay_ms(100);
	}

	return 0;
}
