#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>

typedef union {
	uint16_t full;
	struct {
		uint8_t low;
		uint8_t high;
	};
} register_;


#endif // !REGISTER_H
