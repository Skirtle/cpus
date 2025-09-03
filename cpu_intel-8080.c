#ifndef INTEL_8080
#define INTEL_8080

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "registers.h"

#define DEBUG 1
#define MAX_PROGRAM_SIZE 64

typedef struct CPU {
    uint8_register A, B, C, D, E, H, L, status;
    uint16_register BC, DE, HL;
    uint16_t stack_pointer;
    uint16_t program_counter; 
    uint8_t memory[MAX_PROGRAM_SIZE];
} CPU;

void initialize_uint8_register(uint8_register* reg, char c, uint8_t v);
void initialize_uint16_register(uint16_register* reg, char* s, uint16_t v);
void initialize_cpu(CPU* cpu);
void print_cpu_registers(CPU* cpu);
void print_cpu_memory(CPU* cpu);

int main(int argc, int* argv) {
    char* filename = "program.asm";
    FILE* file = fopen(filename, "r");
    if (file == NULL) return 1;


    CPU cpu;
    initialize_cpu(&cpu);

    fread(cpu.memory, 1, sizeof(cpu.memory), file);
    printf("\n%ld\n", sizeof(cpu.memory));

    print_cpu_registers(&cpu);
    print_cpu_memory(&cpu);

    fclose(file);
    return 0;
}

void initialize_uint8_register(uint8_register* reg, char c, uint8_t v) {
    reg->name = c;
    reg->value = v;
}

void initialize_uint16_register(uint16_register* reg, char* s, uint16_t v) {
    strcpy(reg->name, s);
    reg->value = v;
}

void initialize_cpu(CPU* cpu) {
    initialize_uint8_register(&cpu->A, 'A', 0);
    initialize_uint8_register(&cpu->B, 'B', 0);
    initialize_uint8_register(&cpu->C, 'C', 0);
    initialize_uint8_register(&cpu->D, 'D', 0);
    initialize_uint8_register(&cpu->E, 'E', 0);
    initialize_uint8_register(&cpu->H, 'H', 0);
    initialize_uint8_register(&cpu->L, 'L', 0);

    initialize_uint16_register(&cpu->BC, "BC", 0);
    initialize_uint16_register(&cpu->DE, "DE", 0);
    initialize_uint16_register(&cpu->HL, "HL", 0);

    cpu->stack_pointer = 0;
    cpu->program_counter = 0;

    initialize_uint8_register(&cpu->L, 'S', 0);

    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) {
        cpu->memory[i] = 0;
    }
}

void print_cpu_registers(CPU* cpu) {

    printf("%c = %d, ", cpu->A.name, cpu->A.value);
    printf("%c = %d, ", cpu->B.name, cpu->B.value);
    printf("%c = %d, ", cpu->C.name, cpu->C.value);
    printf("%c = %d, ", cpu->D.name, cpu->D.value);
    printf("%c = %d, ", cpu->E.name, cpu->E.value);
    printf("%c = %d, ", cpu->H.name, cpu->H.value);
    printf("%c = %d\n", cpu->L.name, cpu->L.value);

    printf("%s = %d, ", cpu->BC.name, cpu->BC.value);
    printf("%s = %d, ", cpu->DE.name, cpu->DE.value);
    printf("%s = %d\n", cpu->HL.name, cpu->HL.value);

    printf("SP = %d, PC = %d\n", cpu->stack_pointer, cpu->program_counter);
    // TODO: Split the Status print into its multiple flags
    printf("Status = %d\n", cpu->status.value); 
}

void print_cpu_memory(CPU* cpu) {
    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) {
        if (i % 8 == 0) printf("0x%08x\t", i);

        printf("%02x ", cpu->memory[i]);
        if (i % 8 == 7) printf("\n");
    }
    printf("\n");
}

#endif