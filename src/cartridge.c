#include "cartridge.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cartridge_load(cartridge *cart, const char *filename) {
	if (!cart || !filename) return 1;
	
	FILE *f = fopen(filename, "rb");
	if (!f) return 1;

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	printf("ROM size: %ld (0x%08lx)\n", size, size);
	cart->data_size = size;

	rewind(f);

	cart->data = malloc(size);

	if (!cart->data) goto cleanup;

	printf("Reading data...\n");
	fread(cart->data, 1, size, f);

	printf("Setting header...\n");
	cart->header = (cartridge_header*)(cart->data + 0x100);

	cleanup:

	fclose(f);
	return 0;
}

int cartridge_unload(cartridge * cart) {
	if (!cart) return 1;

	free(cart->data);

	return 0;
}

void dump_rom(cartridge *cart) {
	for (size_t i = 0; i < 0x14F; i++) {
		if (i % 16 == 0) {
			printf("\n");
			printf("%08zx", i);
		}
		if (i % 2 == 0) printf(" ");
		printf("%02x", cart->data[i]);

	}
	printf("\n");

}

int cartridge_print_header(cartridge *cart) {
	if (!cart) return 1;

	// https://gbdev.io/pandocs/The_Cartridge_Header.html#014d--header-checksum
	uint8_t checksum = 0;
	for (uint16_t address = 0x0134; address <= 0x014C; address++) {
		checksum = checksum - cart->data[address] - 1;
	}

	printf("\n~~~~~~~~~~~~~~~~~~~ START CARTRIDGE HEADER ~~~~~~~~~~~~~~~~~~~\n\n");

	printf("Title:\t\t\t%s\n", cart->header->title);
	printf("Manf. code:\t\t\t%s\n", cart->header->manufacturer_code);
	printf("CGB:\t\t\t%s\n", cart->header->cgb_flag ? "true" : "false");
	printf("SGB:\t\t\t%s\n", cart->header->sgb_flag ? "true" : "false");
	printf("New lsc. code:\t\t0x%02x\n", cart->header->new_licensee_code[0]);
	printf("Old lsc. code:\t\t0x%02x\n", cart->header->old_licensee_code);
	printf("Cart type:\t\t%02x\n", cart->header->cartridge_type);
	printf("ROM size:\t\t%02x\n", cart->header->rom_size);
	printf("RAM size:\t\t%02x\n", cart->header->ram_size);
	printf("Destination code:\t%02x\n", cart->header->destination_code);
	printf("ROM version:\t\t%02x\n", cart->header->version_number);
	printf("Header checksum:\t0x%02x : %s\n", cart->header->header_checksum, (checksum == cart->header->header_checksum ? "Valid" : "INVALID"));
	printf("Global checksum:\t0x%02x%02x\n", cart->header->global_checksum[0], cart->header->global_checksum[1]);

	printf("\n~~~~~~~~~~~~~~~~~~~~ END CARTRIDGE HEADER ~~~~~~~~~~~~~~~~~~~~\n");

	return 0;
}
