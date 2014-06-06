#include "safety.h"

#include <avr/io.h>

void safety_init() {
	// Drive PIND6 low
	DDRD |= _BV(PIND7);
	PORTD &= ~_BV(PIND7);

	DDRB &= ~_BV(PINB7);
	PORTB |= _BV(PINB7); // pull up safety
}

bool safety_enabled() {
	// Check if safety is plugged in
	return !(PINB & _BV(PINB7));
}
