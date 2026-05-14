#ifndef BUS_H
#define BUS_H

#include <stdbool.h>
#include <stdint.h>

struct cpu_sm83;
struct mmu;
struct ppu;

typedef struct bus {
	struct cpu_sm83 *cpu;
	struct mmu *mmu;
	struct ppu *ppu;

	bool boot_rom_enabled;
} bus;

int bus_init(bus *bus, struct cpu_sm83 *cpu, struct mmu *mmu, struct ppu *ppu);

uint8_t bus_read(bus *bus, uint16_t addr);
uint16_t bus_read16(bus *bus, uint16_t addr);

void bus_write(bus *bus, uint16_t addr, uint8_t val);
void bus_write16(bus *bus, uint16_t addr, uint16_t val);

#endif // !BUS_H
