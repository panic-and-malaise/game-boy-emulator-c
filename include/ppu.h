#ifndef PPU_H
#define PPU_H

struct bus;

typedef struct ppu {
	struct bus *bus;
} ppu;

int ppu_init(ppu* ppu, struct bus *bus);

#endif // !PPU_H
