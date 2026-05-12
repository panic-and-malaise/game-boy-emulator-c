#include "gameboy.h"
#include "bus.h"
#include "cartridge.h"
#include "mmu.h"
#include "ppu.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int power_up_sequence(gameboy *);

int gameboy_init(gameboy *gb) {
	if (!gb) return 1;

	gb->bus = malloc(sizeof(bus));

	gb->cpu = malloc(sizeof(cpu_sm83));
	cpu_sm83_init(gb->cpu, gb->bus);

	gb->mmu = malloc(sizeof(mmu));
	mmu_init(gb->mmu, gb->bus);

	gb->ppu = malloc(sizeof(ppu));
	ppu_init(gb->ppu, gb->bus);

	bus_init(gb->bus, gb->cpu, gb->mmu, gb->ppu);

	gb->cart = malloc(sizeof(cartridge));

	power_up_sequence(gb);

	return 0;
}

int gameboy_free(gameboy *gb) {
	if (!gb) return 1;

	free(gb->bus);
	free(gb->cpu);

	mmu_free(gb->mmu);
	free(gb->mmu);

	free(gb->ppu);

	cartridge_unload(gb->cart);
	free(gb->cart);

	return 0;
}

int power_up_sequence(gameboy *gb) {
	if (!gb) return 1;

	FILE *f = fopen("roms/dmg.bin", "rb"); // hard-coded, very bad
	if (!f) return 1;

	// 0x0000 -> 0x00FF
	mmu_load_file(gb->mmu, f, 0);

	printf("~~~~~~~~~~~~~~~~ START BOOT ROM ~~~~~~~~~~~~~~~~\n");
	hex_dump(gb->mmu->memory_map, gb->mmu->ram_size);
	printf("\n\n~~~~~~~~~~~~~~~~~ END BOOT ROM ~~~~~~~~~~~~~~~~~\n\n");

	fclose(f);
	return 0;
}
