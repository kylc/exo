#include "glove.h"

#include <avr/io.h>

void glove_init() {
	DDRF |= _BV(1); // potentiometer input

	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // set prescalar values

	ADMUX |= _BV(REFS0);
	ADMUX |= _BV(ADLAR);

	ADCSRA |= _BV(ADEN);
	ADCSRA |= _BV(ADSC);
}

void glove_update(struct glove_state_t *state) {
	if(!(ADCSRA & _BV(ADSC))) {
		state->raw_input = ADCH;
		ADCSRA |= _BV(ADSC);
	}
}

enum direction glove_direction(struct glove_state_t *state) {
  if(state->raw_input < ADC_UP) {
    return UP;
  } else if(state->raw_input > ADC_DOWN) {
    return DOWN;
  }

  return STOP;
}
