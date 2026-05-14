#include "bus.h"
#include "mmu.h"
#include <stdio.h>

int bus_init(bus *bus, struct cpu_sm83 *cpu, struct mmu *mmu, struct ppu *ppu) {
	if (!cpu || !mmu | !ppu) return 1;

	bus->cpu = cpu;
	bus->mmu = mmu;
	bus->ppu = ppu;

	return 0;
}

uint8_t bus_read(bus *bus, uint16_t addr) {
	if (!bus->boot_rom_enabled && addr < 0x100) addr += 0x100;

	return mmu_read(bus->mmu, addr);
}

uint16_t bus_read16(bus *bus, uint16_t addr) {
	return mmu_read16(bus->mmu, addr);
}

void bus_write(bus *bus, uint16_t addr, uint8_t val) {
	if (addr == 0xFF50 && val) {
		bus->boot_rom_enabled = false;
		printf("DISABLE BOOT ROM!!!!!\n\n\n");
	}

	mmu_write(bus->mmu, addr, val);
}
void bus_write16(bus *bus, uint16_t addr, uint16_t val) {
	mmu_write16(bus->mmu, addr, val);
}
