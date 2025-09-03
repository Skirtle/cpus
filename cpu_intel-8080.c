#ifndef INTEL_8080
#define INTEL_8080

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "registers.h"

#define DEBUG true
#define MAX_PROGRAM_SIZE 64

typedef struct CPU {
    uint8_register A, B, C, D, E, H, L, status;
    uint16_register BC, DE, HL;
    uint16_t stack_pointer;
    uint16_t program_counter; 
    uint8_t memory[MAX_PROGRAM_SIZE];
} CPU;

typedef struct Instruction {
    char* mnemonic;
    uint8_t size;
    uint8_t cycles;
} Instruction;

void initialize_uint8_register(uint8_register* reg, char c, uint8_t v);
void initialize_uint16_register(uint16_register* reg, char* s, uint16_t v);
void print_cpu_registers(CPU* cpu);
void print_cpu_memory(CPU* cpu);
void print_binary(uint8_t byte);
void execute(CPU* cpu, Instruction inst);
int initialize_cpu(CPU* cpu, char* filename);
uint8_t fetch(CPU* cpu);
Instruction decode(uint8_t opcode);

// Data movement
void MVI(CPU* cpu, char dest_reg, uint8_t immediate); // Move immedate to dest_reg
void MOV(CPU* cpu, char src_reg, char dest_reg); // Move value from src_reg to dest_reg
void LXI(CPU* cpu, char rp[2], uint16_t immediate); // Load 16-bit immediate into register pair (RP)

int main(int argc, char* argv[]) {
    char* filename;
    if (argc == 1) filename = "program.asm";
    else filename = argv[1];

    CPU cpu;
    if (initialize_cpu(&cpu, filename)) return 1; // Failed to initialize the CPU
    if (DEBUG) print_cpu_registers(&cpu);
    if (DEBUG) print_cpu_memory(&cpu);




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

int initialize_cpu(CPU* cpu, char* filename) {
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

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return 1;
    }
    if (DEBUG) printf("Opened %s\n", filename);
    fread(cpu->memory, 1, sizeof(cpu->memory), file);
    fclose(file);

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

void print_binary(uint8_t byte) {
    printf("0b");
    for (int i = 7; i >= 0; i--) {
        int num = (byte >> i) & 1;
        printf("%d", num);
    }
    printf("\n");
}

void execute(CPU* cpu, Instruction inst) {
    return;
}
uint8_t fetch(CPU* cpu) {
    return 0;
}
Instruction decode(uint8_t opcode) {
    Instruction inst;
    inst.mnemonic = "NOP";
    inst.size = 0;
    inst.cycles = 0;
    return inst;
}

#endif