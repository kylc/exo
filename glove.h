#ifndef GLOVE_H
#define GLOVE_H

#define ADC_UP 0x9B
#define ADC_DOWN 0x7D

#include <stdint.h>

struct glove_state_t {
	uint8_t raw_input;
};

enum direction { STOP, UP, DOWN };

void glove_init();
void glove_update(struct glove_state_t *state);
enum direction glove_direction(struct glove_state_t *state);

#endif
