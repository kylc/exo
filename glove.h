#ifndef GLOVE_H
#define GLOVE_H

#include <stdint.h>

struct glove_state_t {
	uint8_t raw_input;
};

void glove_init();
void glove_update(struct glove_state_t *state);

#endif
