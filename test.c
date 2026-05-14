#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cartridge.h"
#include "gameboy.h"
#include "cpu_sm83.h"
#include "mmu.h"

void cpu_tests(gameboy *gb);

int main(int argc, char *argv[]) {
	gameboy gb;
	gameboy_init(&gb);


	// Load after boot ROM
	char *rom_name = "roms/tetris.gb"; // again, hard-coded = bad

	if (argc > 1)
		rom_name = argv[1];

	printf("\n");

	gameboy_load_rom(&gb, rom_name);
	gameboy_power_up_sequence(&gb);

	// Absolutely insane and stupid, needs to be fixed
	// memcpy(gb.mmu->memory_map, gb.cart->data, gb.cart->data_size > 0x7FFF ? 0x7FFF : gb.cart->data_size); // (Also has no MBC support, just gives up instead)

	// cpu_tests(&gb);
	cpu_sm83_debug_loop(gb.cpu);

	cartridge_print_header(gb.cart);

	gameboy_free(&gb);

	return EXIT_SUCCESS;
}

void cpu_tests(gameboy *gb) {
	cpu_sm83 *cpu = gb->cpu;
	mmu *mmu 	  = gb->mmu;
	ppu *ppu 	  = gb->ppu;

	mmu_write16(mmu, 0x0000, 0xEEFA);

	uint16_t both = mmu_read16(mmu, 0x0000);
	uint8_t low = cpu_sm83_fetch8(cpu);
	uint8_t high = cpu_sm83_fetch8(cpu);

	(*cpu->A) = 15;
	printf("FULL\t\tA\t\tF\n%x\t\t%b\t\t%b\n", cpu->AF.full, *cpu->A, *cpu->F);
	cpu_sm83_set_flag(cpu, 0, 0, 0, 1);
	(*cpu->A) = 0xFF;
	printf("FULL\t\tA\t\tF\n%x\t\t%b\t\t%b\n", cpu->AF.full, *cpu->A, *cpu->F);
	cpu_sm83_step(cpu);
	printf("FULL\t\tA\t\tF\n%x\t\t%b\t\t%b\n", cpu->AF.full, *cpu->A, *cpu->F);

	printf("%x\t\t%x\t\t%x\n", both, low, high);

}
