#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct __attribute__((packed)) cartridge_header {
    uint8_t entry_point[4]; 		 // 0x100–0x103
    uint8_t logo[48];				 // 0x104–0x133
	union {							 // 0x134-0x143
		uint8_t title[16];
		struct {
			uint8_t short_title[11];
			uint8_t manufacturer_code[4];
			uint8_t cgb_flag;
		};
	};
    uint8_t new_licensee_code[2];    // 0x144–0x145
    uint8_t sgb_flag;                // 0x146
    uint8_t cartridge_type;          // 0x147
    uint8_t rom_size;                // 0x148
    uint8_t ram_size;                // 0x149
    uint8_t destination_code;        // 0x14a
    uint8_t old_licensee_code;       // 0x14b
    uint8_t version_number;          // 0x14c
    uint8_t header_checksum;         // 0x14d
    uint8_t global_checksum[2];      // 0x14e–0x14f
} cartridge_header;
#pragma pack(pop)

typedef struct cartridge {
	uint8_t *data;
	size_t data_size;
	cartridge_header *header;
} cartridge;

int cartridge_load(cartridge *cart, const char *filename);
int cartridge_unload(cartridge * cart);
int cartridge_print_header(cartridge *cart);

#endif // !CARTRIDGE_H
