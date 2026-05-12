#include "mmu.h"
#include <stdint.h>
#include <stdlib.h>

int mmu_init(mmu *mmu, struct bus *bus) {
	if (!mmu) return 1;

	mmu->memory_map = malloc(0xFFFF);
	mmu->bus = bus;

	return 0;
}

int mmu_free(mmu *mmu) {
	if (!mmu) return 1;

	free(mmu->memory_map);

	return 0;
}

void mmu_write(mmu *mmu, uint16_t addr, uint8_t val) {
	mmu->memory_map[addr] = val;
}

void mmu_write16(mmu *mmu, uint16_t addr, uint16_t val) {
	if (addr == 0xFFFF) return;

	uint8_t low  = val & 0xFF;
	uint8_t high = val >> 8;

	mmu->memory_map[addr]   = low;
	mmu->memory_map[addr+1] = high;
}

uint8_t mmu_read(mmu *mmu, uint16_t addr) {
	return mmu->memory_map[addr];
}

uint16_t mmu_read16(mmu *mmu, uint16_t addr) {
	if (addr == 0xFFFF) return 0x0000;

	uint8_t low  = mmu->memory_map[addr]   & 0xFF;
	uint8_t high = mmu->memory_map[addr+1] >> 8;

	return (low | high);
}

int mmu_load_file(mmu *mmu, FILE *f, uint16_t offset) {
	if (!mmu || !f) return 1;

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	fread(mmu->memory_map + offset, 1, size, f);
	mmu->ram_size = size;

	return 0;
}
