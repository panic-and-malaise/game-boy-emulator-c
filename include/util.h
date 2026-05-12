#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void hex_dump(uint8_t *data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (i % 16 == 0) {
			printf("\n");
			printf("%08zx", i);
		}
		if (i % 2 == 0) printf(" ");
		printf("%02x", data[i]);
	}
}

#endif // !UTIL_H
