#ifndef INTEL_8080
#define INTEL_8080

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "registers.h"

#define DEBUG true
#define MAX_PROGRAM_SIZE 64


// Structs
typedef struct CPU {
    uint8_register A, B, C, D, E, H, L, status;
    uint16_register BC, DE, HL;
    uint16_t stack_pointer;
    uint16_t program_counter; 
    uint8_t memory[MAX_PROGRAM_SIZE];
    bool running;
} CPU;

typedef struct Instruction {
    char* mnemonic; // For debugging
    void (*execute)(CPU* cpu, uint8_t opcode);
    uint8_t size; // Size of the instruction
} Instruction;

// Global variables
Instruction opcode_lookup[256];

// Function prototypes
void initialize_uint8_register(uint8_register* reg, char c, uint8_t v);
void initialize_uint16_register(uint16_register* reg, char* s, uint16_t v);
void print_cpu_registers(CPU* cpu);
void print_cpu_memory(CPU* cpu);
void print_binary(uint8_t byte);
void initialize_opcode_lookup();
uint8_t fetch(CPU* cpu);
int initialize_cpu(CPU* cpu, char* filename);

// Opcode functions
void HLT(CPU* cpu, uint8_t opcode);
void MVI_B(CPU* cpu, uint8_t opcode);
void MOV(CPU* cpu, uint8_t opcode);

// Start!
int main(int argc, char* argv[]) {
    char* filename;
    bool running = true;
    if (argc == 1) filename = "program.asm";
    else filename = argv[1];

    CPU cpu;
    int code = initialize_cpu(&cpu, filename);
    if (code){ // Failed to initialize the CPU
        printf("Failed to initalize CPU, exit code %d\n", code);
        return 1;
    } 
    
    if (DEBUG) printf("Initializing opcode lookup table");
    initialize_opcode_lookup();

    if (DEBUG) {
        printf("\nStarting conditions:\n");
        print_cpu_registers(&cpu);
        printf("\n");
        print_cpu_memory(&cpu);
        printf("\n");
    }

    printf("Starting FED loop\n");
    // Fetch-decode-execute loop
    while (cpu.running) {
        printf("Starting loop\n");
        uint8_t opcode = cpu.memory[cpu.program_counter];
        Instruction inst = opcode_lookup[opcode];
        if (DEBUG) printf("%s\n", inst.mnemonic);
        inst.execute(&cpu, opcode);
        cpu.program_counter += inst.size;
    }


    return 0;
}

// Function defenitions
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
    cpu->running = true;
    return 0;
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
    printf("Status = %d\nRunning = %d", cpu->status.value, cpu->running); 
}

void print_cpu_memory(CPU* cpu) {
    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) {
        if (i % 8 == 0) printf("0x%08x\t", i);

        printf("%02x ", cpu->memory[i]);
        if (i % 8 == 7) printf("\n");
    }
}

void print_binary(uint8_t byte) {
    printf("0b");
    for (int i = 7; i >= 0; i--) {
        int num = (byte >> i) & 1;
        printf("%d", num);
    }
}

uint8_t fetch(CPU* cpu) { return cpu->memory[cpu->program_counter]; }

// Opcode table
void initialize_opcode_lookup() {
    opcode_lookup[0x66] = (Instruction) {"HLT", HLT, 1};
    opcode_lookup[0x3E] = (Instruction) {"MVI_B", MVI_B, 2};
    opcode_lookup[0x47] = (Instruction) {"MOV B, A", MOV, 1};
}

// Opcode function defenitions
void HLT(CPU* cpu, uint8_t opcode) { cpu->running = false; }
void MVI_B(CPU* cpu, uint8_t opcode) {
    cpu->B.value = cpu->memory[cpu->stack_pointer + 1];
    cpu->stack_pointer++;
}
void MOV(CPU* cpu, uint8_t opcode) {

}


#endif