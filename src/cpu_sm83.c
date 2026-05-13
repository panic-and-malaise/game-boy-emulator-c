#include "cpu_sm83.h"
#include "bus.h"
#include "register.h"

#include <stdint.h>
#include <stdio.h>

int cpu_sm83_init(cpu_sm83 *cpu, struct bus* bus) {
	if (!cpu) return 1;

	cpu->bus = bus;

	cpu->AF = (register_){ .full = 0 };
	cpu->BC = (register_){ .full = 0 };
	cpu->DE = (register_){ .full = 0 };
	cpu->HL = (register_){ .full = 0 };

	cpu->A = &cpu->AF.high;
	cpu->F = &cpu->AF.low;

	cpu->B = &cpu->BC.high;
	cpu->C = &cpu->BC.low;

	cpu->D = &cpu->DE.high;
	cpu->E = &cpu->DE.low;

	cpu->H = &cpu->HL.high;
	cpu->L = &cpu->HL.low;

	return 0;
}

uint8_t cpu_sm83_fetch8(cpu_sm83 *cpu) {
	return bus_read(cpu->bus, cpu->PC++);
}

uint8_t fetch8(cpu_sm83 *cpu) {
	return bus_read(cpu->bus, cpu->PC++);
}

uint16_t fetch16(cpu_sm83 *cpu) {
	uint16_t value = bus_read16(cpu->bus, cpu->PC);
	cpu->PC += 2;
	return value;
}

void cpu_sm83_set_flag(cpu_sm83 *cpu, bool z, bool n, bool h, bool c) {
	// A	Y	result
	// 0	0	0
	// 0	1	1
	// 1	0	0
	// 1	1	1

	*cpu->F ^= (z << 3);
	*cpu->F ^= (n << 2);
	*cpu->F ^= (h << 1);
	*cpu->F ^= (c << 0);
}

bool cpu_sm83_get_flag_z(cpu_sm83 *cpu) {
	return (*cpu->F & 0x08);
}

bool cpu_sm83_get_flag_n(cpu_sm83 *cpu) {
	return (*cpu->F & 0x04);
}

bool cpu_sm83_get_flag_h(cpu_sm83 *cpu) {
	return (*cpu->F & 0x02);
}

bool cpu_sm83_get_flag_c(cpu_sm83 *cpu) {
	return (*cpu->F & 0x01);
}

uint8_t NONE() {
	return 0;
}

uint8_t NOP() {
	return 4;
}

uint8_t LD_r16_n16(cpu_sm83 *cpu, register_ *r16, uint16_t n16) {
	r16->full = n16;
	return 12;
}

uint8_t LD_r16_r8(cpu_sm83 *cpu, register_ *r16, uint8_t *r8) {
	r16->full = *r8;
	return 8;
}

uint8_t LD_r8_r8(cpu_sm83 *cpu, uint8_t *r8, uint8_t *load) {
	*r8 = *load;
	return 4;
}

uint8_t LD_r8_addr_HL(cpu_sm83 *cpu, uint8_t *r8) {
	*r8 = bus_read(cpu->bus, cpu->HL.full);
	return 8;
}

uint8_t LD_SP_n16(cpu_sm83 *cpu, uint16_t n16) {
	cpu->SP = n16;
	return 12;
}

uint8_t LD_r16_A(cpu_sm83 *cpu, register_ *r16) {
	bus_write(cpu->bus, r16->full, *cpu->A);
	return 8;
}

uint8_t LD_HLI_A(cpu_sm83 *cpu) {
	uint8_t cycles = LD_r16_A(cpu, &cpu->HL);
	cpu->HL.full++;
	return cycles;
}

uint8_t LD_HLD_A(cpu_sm83 *cpu) {
	uint8_t cycles = LD_r16_A(cpu, &cpu->HL);
	cpu->HL.full--;
	return cycles;
}

uint8_t LD_r8_n8(cpu_sm83 *cpu, uint8_t *r8, uint8_t n8) {
	*r8 = n8;
	return 8;
}

uint8_t LD_addr_HL_r8(cpu_sm83 *cpu, uint8_t *r8) {
	bus_write(cpu->bus, cpu->HL.full, *r8);
	return 8;
}

uint8_t INC_r8(cpu_sm83 *cpu, uint8_t *r8) {
	(*r8)++;

	// Z 	Set if result is 0.
	// N 	0
	// H 	Set if overflow from bit 3.
	cpu_sm83_set_flag(cpu, !(*r8), 0, (*r8 & 0x0F) == 0x00, cpu_sm83_get_flag_c(cpu));

	return 4;
}

uint8_t INC_r16(cpu_sm83 *cpu, uint16_t *r16) {
	(*r16)++;

	return 8;
}

uint8_t DEC_r8(cpu_sm83 *cpu, uint8_t *r8) {
	(*r8)--;

	// Z 	Set if result is 0
	// N 	1
	// H 	Set if overflow from bit 4
	cpu_sm83_set_flag(cpu, !(*r8), 1, !((*r8 + 1) & 0x0F), cpu_sm83_get_flag_c(cpu));

	return 4;
}

uint8_t DEC_r16(cpu_sm83 *cpu, uint16_t *r16) {
	(*r16)--;

	return 8;
}

uint8_t DEC_addr_HL(cpu_sm83 *cpu) {
	bus_write(cpu->bus, cpu->HL.full, bus_read(cpu->bus, cpu->HL.full) - 1);

	return 12;
}

uint8_t JR_e8(cpu_sm83 *cpu, uint8_t e8) {
	int8_t offset = (int8_t)e8;
	cpu->PC += offset; // TODO: Hmmmmm.... Pay attention to the value of PC here
	return 12;
}

uint8_t JR_cc_e8(cpu_sm83 *cpu, bool cc, uint8_t e8) {
	if (!cc) return 8;
	return JR_e8(cpu, e8);
}

uint8_t INC_addr_HL(cpu_sm83 *cpu) {
	uint8_t val = bus_read(cpu->bus, cpu->HL.full);
	bus_write(cpu->bus, cpu->HL.full, ++val);

	// Z 	Set if result is 0.
	// N 	0
	// H 	Set if overflow from bit 3.
	cpu_sm83_set_flag(cpu, !(val), 0, (val & 0x0F) == 0x00, cpu_sm83_get_flag_h(cpu));

	return 12;
}

uint8_t XOR_A_r8(cpu_sm83 *cpu, uint8_t *r8) {
	*cpu->A = (*r8) ^ (*cpu->A);
	cpu_sm83_set_flag(cpu, !(*cpu->A), 0, 0, 0);
	return 4;
}

uint8_t XOR_A_addrHL(cpu_sm83 *cpu) {
	*cpu->A = bus_read(cpu->bus, cpu->HL.full) ^ (*cpu->A);
	cpu_sm83_set_flag(cpu, !(*cpu->A), 0, 0, 0);
	return 8;
}

uint8_t XOR_A_A(cpu_sm83 *cpu) {
	*cpu->A = (*cpu->A) ^ (*cpu->A);
	cpu_sm83_set_flag(cpu, 1, 0, 0, 0);
	return 4;
}

uint8_t EI(cpu_sm83 *cpu) {
	cpu->ime_scheduled = true;
	return 4;
}

uint8_t DI(cpu_sm83 *cpu) {
	cpu->ime = false;
	return 4;
}

uint8_t LDH_n16_A(cpu_sm83 *cpu, uint16_t n16) {
	bus_write(cpu->bus, n16, *cpu->A);
	return 16;
}

uint8_t LDH_addr_C_A(cpu_sm83 *cpu) {
	bus_write(cpu->bus, (0xFF00 + *cpu->C), *cpu->A);
	return 8;
}

uint8_t LD_A_addr_r16(cpu_sm83 *cpu, register_ r16) {
	*cpu->A = bus_read(cpu->bus, r16.full);
	return 8;
}

uint8_t LD_A_n16(cpu_sm83 *cpu, uint16_t n16) {
	*cpu->A = bus_read(cpu->bus, n16);
	return 8;
}

uint8_t LDH_A_n16(cpu_sm83 *cpu, uint8_t a8) {
	*cpu->A = bus_read(cpu->bus, 0xFF00 | a8);
	return 12;
}

uint8_t LDH_a8_A(cpu_sm83 *cpu, uint8_t a8) {
	bus_write(cpu->bus, 0xFF00 | a8, *cpu->A);
	return 12;
}

uint8_t LDH_A_a8(cpu_sm83 *cpu, uint8_t a8) {
	*cpu->A = bus_read(cpu->bus, 0xFF00 | a8);
	return 12;
}

uint8_t ADC_A_r8(cpu_sm83 *cpu, uint8_t *r8) {
	uint8_t a = *cpu->A;
	uint8_t b = *r8;
	uint8_t c = cpu_sm83_get_flag_c(cpu);

	uint16_t result = a + b + c;

	bool hc = ((a & 0xF) + (b & 0xF) + c) > 0xF;
	bool carry = result > 0xFF;

	*cpu->A = (uint8_t)result;

	// Z	Set if result is 0.
	// N	0
	// H	Set if overflow from bit 3.
	// C	Set if overflow from bit 7.
	cpu_sm83_set_flag(cpu,
		!(*cpu->A),
		0,
		hc,
		carry
	);

	return 4;
}

uint8_t ADC_A_addr_HL(cpu_sm83 *cpu) {
	uint8_t a = *cpu->A;
	uint8_t b = bus_read(cpu->bus, cpu->HL.full);
	uint8_t c = cpu_sm83_get_flag_c(cpu);

	uint16_t result = a + b + c;

	bool hc = ((a & 0xF) + (b & 0xF) + c) > 0xF;
	bool carry = result > 0xFF;

	*cpu->A = (uint8_t)result;

	// Z	Set if result is 0.
	// N	0
	// H	Set if overflow from bit 3.
	// C	Set if overflow from bit 7.
	cpu_sm83_set_flag(cpu,
		!(*cpu->A),
		0,
		hc,
		carry
	);

	return 8;
}

uint8_t ADC_A_n8(cpu_sm83 *cpu, uint8_t n8) {
	uint8_t a = *cpu->A;
	uint8_t b = n8;
	uint8_t c = cpu_sm83_get_flag_c(cpu);

	uint16_t result = a + b + c;

	bool hc = ((a & 0xF) + (b & 0xF) + c) > 0xF;
	bool carry = result > 0xFF;

	*cpu->A = (uint8_t)result;

	// Z	Set if result is 0.
	// N	0
	// H	Set if overflow from bit 3.
	// C	Set if overflow from bit 7.
	cpu_sm83_set_flag(cpu,
		!(*cpu->A),
		0,
		hc,
		carry
	);

	return 8;
}

uint8_t ADD_A_r8(cpu_sm83 *cpu, uint8_t *r8) {
	uint8_t a = *cpu->A;
	uint8_t b = *r8;

	uint16_t result = a + b;

	bool hc = ((a & 0xF) + (b & 0xF)) > 0xF;
	bool carry = result > 0xFF;

	*cpu->A = (uint8_t)result;

	// Z	Set if result is 0.
	// N	0
	// H	Set if overflow from bit 3.
	// C	Set if overflow from bit 7.
	cpu_sm83_set_flag(cpu,
		!(*cpu->A),
		0,
		hc,
		carry
	);

	return 4;
}

uint8_t JP_n16(cpu_sm83 *cpu, uint16_t n16) {
	cpu->PC = n16;
	return 16;
}

uint8_t JP_cc_n16(cpu_sm83 *cpu, bool cc, uint16_t n16) {
	if (!cc) return 12;
	cpu->PC = n16;
	return 16;
}

uint8_t POP_r16(cpu_sm83 *cpu, register_ *r16) {
	bus_write(cpu->bus, cpu->SP++, r16->low);
	bus_write(cpu->bus, cpu->SP++, r16->high);
	return 12;
}

uint8_t POP_AF(cpu_sm83 *cpu) {
	LD_r8_n8(cpu, cpu->F, bus_read(cpu->bus, cpu->SP++));
	LD_r8_n8(cpu, cpu->A, bus_read(cpu->bus, cpu->SP++));

	// Flags:

	// Z
	//     Set from bit 7 of the popped low byte.
	// N
	//     Set from bit 6 of the popped low byte.
	// H
	//     Set from bit 5 of the popped low byte.
	// C
	//     Set from bit 4 of the popped low byte. 

	uint8_t low_byte = cpu->AF.low;
	cpu_sm83_set_flag(cpu,
		(low_byte >> 7) & 1,
		(low_byte >> 6) & 1,
		(low_byte >> 5) & 1,
		(low_byte >> 4) & 1
	);

	return 12;
}

uint8_t RET(cpu_sm83 *cpu) {
	uint16_t r16 = cpu->PC;
	uint8_t low = r16 & 0xFF;
	uint8_t high = r16 >> 8;

	bus_write(cpu->bus, cpu->SP++, low);
	bus_write(cpu->bus, cpu->SP++, high);

	return 16;
}

uint8_t RET_cc(cpu_sm83 *cpu, bool cc) {
	if (!cc) return 8;
	(void)RET(cpu);
	return 20;
}

uint8_t RETI(cpu_sm83 *cpu) {
	(void)EI(cpu);
	(void)RET(cpu);
	return 16;
}

uint8_t CALL_n16(cpu_sm83 *cpu, uint16_t n16) {
    bus_write(cpu->bus, --cpu->SP, n16 >> 8);
    bus_write(cpu->bus, --cpu->SP, n16 & 0xFF);

	return 24;
}

uint8_t CALL_cc_n16(cpu_sm83 *cpu, bool cc, uint16_t n16) {
	if (!cc) return 12;
	return CALL_n16(cpu, n16);
}

uint8_t BIT_u3_r8(cpu_sm83 *cpu, uint8_t u3, uint8_t *r8) {
	cpu_sm83_set_flag(cpu,
	   !(*r8 & (1 << u3)),
	   0,
	   1,
	   cpu_sm83_get_flag_c(cpu)
    );
	return 8;
}

uint8_t BIT_u3_addr_HL(cpu_sm83 *cpu, uint8_t u3) {
	cpu_sm83_set_flag(cpu,
	   !(bus_read(cpu->bus, cpu->HL.full) & (1 << u3)),
	   0,
	   1,
	   cpu_sm83_get_flag_c(cpu)
	);
	return 12;
}

uint8_t run_opcode_prefix(cpu_sm83 *cpu, uint8_t prefix) {
	printf("\nAttempting to run prefix opcode: %02x ... ", prefix);
	switch (prefix) {
		case 0x00:
			return NONE();
		case 0x70:
			return BIT_u3_r8(cpu, 6, cpu->B);
		case 0x71:
			return BIT_u3_r8(cpu, 6, cpu->C);
		case 0x72:
			return BIT_u3_r8(cpu, 6, cpu->D);
		case 0x73:
			return BIT_u3_r8(cpu, 6, cpu->E);
		case 0x74:
			return BIT_u3_r8(cpu, 6, cpu->H);
		case 0x75:
			return BIT_u3_r8(cpu, 6, cpu->L);
		case 0x76:
			return BIT_u3_addr_HL(cpu, 6);
		case 0x77:
			return BIT_u3_r8(cpu, 6, cpu->A);
		case 0x78:
			return BIT_u3_r8(cpu, 7, cpu->B);
		case 0x79:
			return BIT_u3_r8(cpu, 7, cpu->C);
		case 0x7a:
			return BIT_u3_r8(cpu, 7, cpu->D);
		case 0x7b:
			return BIT_u3_r8(cpu, 7, cpu->E);
		case 0x7c:
			return BIT_u3_r8(cpu, 7, cpu->H);
		case 0x7d:
			return BIT_u3_r8(cpu, 7, cpu->L);
		case 0x7e:
			return BIT_u3_addr_HL(cpu, 7);
		case 0x7f:
			return BIT_u3_r8(cpu, 7, cpu->A);
		default:
			return NONE();
	}
	return 4;
}

uint8_t run_opcode(cpu_sm83 *cpu, uint8_t opcode) {
	printf("Attempting to run opcode: %02x ... ", opcode);
	switch (opcode) {
		// NOP
		case 0x00:
			return NOP();
		case 0x01:
			return LD_r16_n16(cpu, &cpu->BC, fetch16(cpu));
		case 0x11:
			return LD_r16_n16(cpu, &cpu->DE, fetch16(cpu));
		case 0x21:
			return LD_r16_n16(cpu, &cpu->HL, fetch16(cpu));
		case 0x31:
			return LD_SP_n16(cpu, fetch16(cpu));
		case 0x02:
			return LD_r16_A(cpu, &cpu->BC);
		case 0x12:
			return LD_r16_A(cpu, &cpu->DE);
		case 0x0A:
			return LD_A_addr_r16(cpu, cpu->BC);
		case 0x1A:
			return LD_A_addr_r16(cpu, cpu->DE);
		case 0x2A: {
			uint8_t cycles = LD_A_addr_r16(cpu, cpu->HL);
			cpu->HL.full++;
			return cycles;
		}
		case 0x3A: {
			uint8_t cycles = LD_A_addr_r16(cpu, cpu->HL);
			cpu->HL.full--;
			return cycles;
		}
		case 0x20:
			return JR_cc_e8(cpu, !cpu_sm83_get_flag_z(cpu), fetch8(cpu));
		case 0x30:
			return JR_cc_e8(cpu, !cpu_sm83_get_flag_c(cpu), fetch8(cpu));
		case 0x22:
			return LD_HLI_A(cpu);
		case 0x32:
			return LD_HLD_A(cpu);
		case 0x0E:
			return LD_r8_n8(cpu, cpu->C, fetch8(cpu));
		case 0x1E:
			return LD_r8_n8(cpu, cpu->E, fetch8(cpu));
		case 0x2E:
			return LD_r8_n8(cpu, cpu->L, fetch8(cpu));
		case 0x3E:
			return LD_r8_n8(cpu, cpu->A, fetch8(cpu));
		case 0x06:
			return LD_r8_n8(cpu, cpu->B, fetch8(cpu));
		case 0x16:
			return LD_r8_n8(cpu, cpu->D, fetch8(cpu));
		case 0x26:
			return LD_r8_n8(cpu, cpu->H, fetch8(cpu));
		case 0x04:
			return INC_r8(cpu, cpu->B);
		case 0x14:
			return INC_r8(cpu, cpu->D);
		case 0x24:
			return INC_r8(cpu, cpu->H);
		case 0x34:
			return INC_addr_HL(cpu);
		case 0x0C:
			return INC_r8(cpu, cpu->C);
		case 0x1C:
			return INC_r8(cpu, cpu->E);
		case 0x2C:
			return INC_r8(cpu, cpu->L);
		case 0x3C:
			return INC_r8(cpu, cpu->A);
		case 0x03:
			return INC_r16(cpu, &cpu->BC.full);
		case 0x13:
			return INC_r16(cpu, &cpu->DE.full);
		case 0x23:
			return INC_r16(cpu, &cpu->HL.full);
		case 0x33:
			return INC_r16(cpu, &cpu->SP);
		case 0x05:
			return DEC_r8(cpu, cpu->B);
		case 0x15:
			return DEC_r8(cpu, cpu->D);
		case 0x25:
			return DEC_r8(cpu, cpu->H);
		case 0x35:
			return DEC_addr_HL(cpu);
		case 0x0D:
			return DEC_r8(cpu, cpu->C);
		case 0x1D:
			return DEC_r8(cpu, cpu->E);
		case 0x2D:
			return DEC_r8(cpu, cpu->L);
		case 0x3D:
			return DEC_r8(cpu, cpu->A);
		case 0x40:
			return LD_r8_r8(cpu, cpu->B, cpu->B);
		case 0x41:
			return LD_r8_r8(cpu, cpu->B, cpu->C);
		case 0x42:
			return LD_r8_r8(cpu, cpu->B, cpu->D);
		case 0x43:
			return LD_r8_r8(cpu, cpu->B, cpu->E);
		case 0x44:
			return LD_r8_r8(cpu, cpu->B, cpu->H);
		case 0x45:
			return LD_r8_r8(cpu, cpu->B, cpu->L);
		case 0x46:
			return LD_r8_addr_HL(cpu, cpu->B);
		case 0x47:
			return LD_r8_r8(cpu, cpu->B, cpu->A);
		case 0x48:
			return LD_r8_r8(cpu, cpu->C, cpu->B);
		case 0x49:
			return LD_r8_r8(cpu, cpu->C, cpu->C);
		case 0x4a:
			return LD_r8_r8(cpu, cpu->C, cpu->D);
		case 0x4b:
			return LD_r8_r8(cpu, cpu->C, cpu->E);
		case 0x4c:
			return LD_r8_r8(cpu, cpu->C, cpu->H);
		case 0x4d:
			return LD_r8_r8(cpu, cpu->C, cpu->L);
		case 0x4e:
			return LD_r8_addr_HL(cpu, cpu->C);
		case 0x4f:
			return LD_r8_r8(cpu, cpu->C, cpu->A);
		case 0x50:
			return LD_r8_r8(cpu, cpu->D, cpu->B);
		case 0x51:
			return LD_r8_r8(cpu, cpu->D, cpu->C);
		case 0x52:
			return LD_r8_r8(cpu, cpu->D, cpu->D);
		case 0x53:
			return LD_r8_r8(cpu, cpu->D, cpu->E);
		case 0x54:
			return LD_r8_r8(cpu, cpu->D, cpu->H);
		case 0x55:
			return LD_r8_r8(cpu, cpu->D, cpu->L);
		case 0x56:
			return LD_r8_addr_HL(cpu, cpu->D);
		case 0x57:
			return LD_r8_r8(cpu, cpu->D, cpu->A);
		case 0x58:
			return LD_r8_r8(cpu, cpu->E, cpu->B);
		case 0x59:
			return LD_r8_r8(cpu, cpu->E, cpu->C);
		case 0x5a:
			return LD_r8_r8(cpu, cpu->E, cpu->D);
		case 0x5b:
			return LD_r8_r8(cpu, cpu->E, cpu->E);
		case 0x5c:
			return LD_r8_r8(cpu, cpu->E, cpu->H);
		case 0x5d:
			return LD_r8_r8(cpu, cpu->E, cpu->L);
		case 0x5e:
			return LD_r8_addr_HL(cpu, cpu->E);
		case 0x5f:
			return LD_r8_r8(cpu, cpu->E, cpu->A);
		case 0x60:
			return LD_r8_r8(cpu, cpu->H, cpu->B);
		case 0x61:
			return LD_r8_r8(cpu, cpu->H, cpu->C);
		case 0x62:
			return LD_r8_r8(cpu, cpu->H, cpu->D);
		case 0x63:
			return LD_r8_r8(cpu, cpu->H, cpu->E);
		case 0x64:
			return LD_r8_r8(cpu, cpu->H, cpu->H);
		case 0x65:
			return LD_r8_r8(cpu, cpu->H, cpu->L);
		case 0x66:
			return LD_r8_addr_HL(cpu, cpu->H);
		case 0x67:
			return LD_r8_r8(cpu, cpu->H, cpu->A);
		case 0x68:
			return LD_r8_r8(cpu, cpu->L, cpu->B);
		case 0x69:
			return LD_r8_r8(cpu, cpu->L, cpu->C);
		case 0x6a:
			return LD_r8_r8(cpu, cpu->L, cpu->D);
		case 0x6b:
			return LD_r8_r8(cpu, cpu->L, cpu->E);
		case 0x6c:
			return LD_r8_r8(cpu, cpu->L, cpu->H);
		case 0x6d:
			return LD_r8_r8(cpu, cpu->L, cpu->L);
		case 0x6e:
			return LD_r8_addr_HL(cpu, cpu->L);
		case 0x6f:
			return LD_r8_r8(cpu, cpu->L, cpu->A);
		case 0x70:
			return LD_addr_HL_r8(cpu, cpu->B);
		case 0x71:
			return LD_addr_HL_r8(cpu, cpu->C);
		case 0x72:
			return LD_addr_HL_r8(cpu, cpu->D);
		case 0x73:
			return LD_addr_HL_r8(cpu, cpu->E);
		case 0x74:
			return LD_addr_HL_r8(cpu, cpu->H);
		case 0x75:
			return LD_addr_HL_r8(cpu, cpu->L);
		case 0x76:
			NOP(); // TODO: HALT
		case 0x77:
			return LD_r16_r8(cpu, &cpu->HL, cpu->A);
		case 0x78:
			return LD_r8_r8(cpu, cpu->A, cpu->B);
		case 0x79:
			return LD_r8_r8(cpu, cpu->A, cpu->C);
		case 0x7a:
			return LD_r8_r8(cpu, cpu->A, cpu->D);
		case 0x7b:
			return LD_r8_r8(cpu, cpu->A, cpu->E);
		case 0x7c:
			return LD_r8_r8(cpu, cpu->A, cpu->H);
		case 0x7d:
			return LD_r8_r8(cpu, cpu->A, cpu->L);
		case 0x7e:
			return LD_r8_addr_HL(cpu, cpu->A);
		case 0x7f:
			return LD_r8_r8(cpu, cpu->A, cpu->A);
		case 0xA8:
			return XOR_A_r8(cpu, cpu->B);
		case 0xA9:
			return XOR_A_r8(cpu, cpu->C);
		case 0xAA:
			return XOR_A_r8(cpu, cpu->D);
		case 0xAB:
			return XOR_A_r8(cpu, cpu->E);
		case 0xAC:
			return XOR_A_r8(cpu, cpu->H);
		case 0xAD:
			return XOR_A_r8(cpu, cpu->L);
		case 0xAE:
			return XOR_A_addrHL(cpu);
		case 0xAF:
			return XOR_A_A(cpu);
		case 0xE0:
			return LDH_a8_A(cpu, fetch8(cpu));
		case 0xF0:
			return LDH_A_a8(cpu, fetch8(cpu));
		case 0xE2:
			return LDH_addr_C_A(cpu);
		case 0xC0:
			return RET_cc(cpu, !cpu_sm83_get_flag_z(cpu));
		case 0xC1:
			return POP_r16(cpu, &cpu->BC);
		case 0xC2:
			return JP_cc_n16(cpu, !cpu_sm83_get_flag_z(cpu), fetch16(cpu));
		case 0xC3:
			return JP_n16(cpu, fetch16(cpu));
		case 0xC4:
			return CALL_cc_n16(cpu, !cpu_sm83_get_flag_z(cpu), fetch16(cpu));
		case 0xC8:
			return RET_cc(cpu, cpu_sm83_get_flag_z(cpu));
		case 0xC9:
			return RET(cpu);
		case 0xCA:
			return JP_cc_n16(cpu, cpu_sm83_get_flag_z(cpu), fetch16(cpu));
		case 0xCB:
			return run_opcode_prefix(cpu, fetch8(cpu));
		case 0xCC:
			return CALL_cc_n16(cpu, cpu_sm83_get_flag_z(cpu), fetch16(cpu));
		case 0xCD:
			return CALL_n16(cpu, fetch16(cpu));
		case 0xD0:
			return RET_cc(cpu, !cpu_sm83_get_flag_c(cpu));
		case 0xD1:
			return POP_r16(cpu, &cpu->DE);
		case 0xD2:
			return JP_cc_n16(cpu, !cpu_sm83_get_flag_c(cpu), fetch16(cpu));
		case 0xD4:
			return CALL_cc_n16(cpu, !cpu_sm83_get_flag_c(cpu), fetch16(cpu));
		case 0xD8:
			return RET_cc(cpu, cpu_sm83_get_flag_c(cpu));
		case 0xD9:
			return RETI(cpu);
		case 0xDA:
			return JP_cc_n16(cpu, cpu_sm83_get_flag_c(cpu), fetch16(cpu));
		case 0xDC:
			return CALL_cc_n16(cpu, cpu_sm83_get_flag_c(cpu), fetch16(cpu));
		case 0xE1:
			return POP_r16(cpu, &cpu->HL);
		case 0xF1:
			return POP_AF(cpu);
		default:
			return NONE();
	}
	return 4;
}

uint8_t cpu_sm83_step(cpu_sm83 *cpu) {
	// TODO: Fix the fact that I'm literally overriding the ROM with the bus currently...
	// Need to split RAM and ROM, I think
	if (cpu->ime_scheduled) {
		cpu->ime = true;
		cpu->ime_scheduled = false;
	}

	return run_opcode(cpu, fetch8(cpu));
}

void debug_print_registers(cpu_sm83 *cpu) {
	printf("\n~~~~~~~ CPU registers ~~~~~~~\n");

	printf("A: %02x\n", *cpu->A);
	printf("F: %02x\n", *cpu->F);
	printf("B: %02x\n", *cpu->B);
	printf("C: %02x\n", *cpu->C);
	printf("D: %02x\n", *cpu->D);
	printf("E: %02x\n", *cpu->E);
	printf("H: %02x\n", *cpu->H);
	printf("L: %02x\n", *cpu->L);

	printf("\nAF: %04x\n", cpu->AF.full);
	printf("BC: %04x\n", cpu->BC.full);
	printf("DE: %04x\n", cpu->DE.full);
	printf("HL: %04x\n", cpu->HL.full);

	printf("\nSP: %04x\n", cpu->SP);
	printf("PC: %04x\n", cpu->PC);

	printf("\n~~~~~ End CPU registers ~~~~~\n");
}

void cpu_sm83_debug_loop(cpu_sm83 *cpu) {
	printf("CPU debug loop! Press ENTER to step forward, Q to quit, S to step to next unimplemented instruction, P to print registers!\n");
	for (;;) {
		char c = getchar();
		if (c == 'q' || c == 'Q') {
			debug_print_registers(cpu);
			break;
		} else if (c == 's' || c == 'S') {
			while (cpu_sm83_step(cpu)) printf("Success!\n");
			printf("Not implemented! ");

			getchar();
			continue;
		} else if (c == 'p' || c == 'p') {
			debug_print_registers(cpu);
			getchar();
			continue;
		}

		bool implemented = cpu_sm83_step(cpu);

		if(implemented)
			printf("Success! ");
		else
			printf("Not implemented! ");
	}
}
