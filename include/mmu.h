#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdio.h>

struct bus;

typedef struct mmu {
	struct bus* bus;
	uint8_t *memory_map;
	size_t ram_size;
} mmu;

int mmu_init(mmu *mmu, struct bus *bus);
int mmu_free(mmu *mmu);

void mmu_write(mmu *mmu, uint16_t addr, uint8_t val);
void mmu_write16(mmu *mmu, uint16_t addr, uint16_t val);

uint8_t mmu_read(mmu *mmu, uint16_t addr);
uint16_t mmu_read16(mmu *mmu, uint16_t addr);

int mmu_load_file(mmu *mmu, FILE *f, uint16_t offset);

#endif // !MMU_H
