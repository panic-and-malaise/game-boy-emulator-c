#include "ppu.h"

int ppu_init(ppu* ppu, struct bus *bus) {
	if (!ppu || !bus) return 1;

	ppu->bus = bus;

	return 0;
}
