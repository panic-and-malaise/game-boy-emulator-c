#ifndef CPU_SM83_H
#define CPU_SM83_H

#include <stdbool.h>
#include <stdint.h>

#include "register.h"

typedef struct cpu_sm83 {
	struct bus* bus;

	register_ AF;
	register_ BC;
	register_ DE;
	register_ HL;

	uint16_t SP;
	uint16_t PC;

	uint8_t *A;
	uint8_t *F;

	uint8_t *B;
	uint8_t *C;

	uint8_t *D;
	uint8_t *E;

	uint8_t *H;
	uint8_t *L;

	bool ime;
	bool ime_scheduled;
	bool ie;
	bool flag_if;
} cpu_sm83;

int cpu_sm83_init(cpu_sm83 *cpu, struct bus* bus);
uint8_t cpu_sm83_step(cpu_sm83 *cpu);

uint8_t cpu_sm83_fetch8(cpu_sm83 *cpu);

void cpu_sm83_set_flag(cpu_sm83 *cpu, bool z, bool n, bool h, bool c);

bool cpu_sm83_get_flag_z(cpu_sm83 *cpu);
bool cpu_sm83_get_flag_n(cpu_sm83 *cpu);
bool cpu_sm83_get_flag_h(cpu_sm83 *cpu);
bool cpu_sm83_get_flag_c(cpu_sm83 *cpu);

void cpu_sm83_debug_loop(cpu_sm83 *cpu);

#endif // !CPU_SM83_H
