#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "bus.h"
#include "cartridge.h"
#include "cpu_sm83.h"
#include "mmu.h"
#include "ppu.h"

typedef struct {
	bus *bus;
	cpu_sm83 *cpu;
	mmu *mmu;
	ppu *ppu;
	cartridge *cart;
} gameboy;

int gameboy_init(gameboy *gb);
int gameboy_free(gameboy *gb);

#endif // !GAMEBOY_H
