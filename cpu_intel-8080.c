#ifndef INTEL_8080
#define INTEL_8080

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "registers.h"

#define DEBUG true
#define MAX_PROGRAM_SIZE 64
#define MEMORY_WIDTH 8


// Structs
typedef struct CPU {
    uint8_register A, B, C, D, E, H, L, M, flag;
    uint16_register BC, DE, HL;
    uint16_t stack_pointer;
    uint16_t program_counter; 
    uint8_t memory[MAX_PROGRAM_SIZE];
    bool running;
} CPU;

typedef struct Instruction {
    char* name; // For debugging
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
void update_uint16_registers(CPU* cpu);
void update_flags(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value);
char* get_register_name(uint8_t reg);
int initialize_cpu(CPU* cpu, char* filename);
uint8_t fetch(CPU* cpu);
uint8_register* get_register_ptr(CPU* cpu, uint8_t reg);


// Opcode functions
void NOP(CPU* cpu, uint8_t opcode);
void HLT(CPU* cpu, uint8_t opcode);

// // Data management (8-bit only)
void MVI(CPU* cpu, uint8_t opcode);
void MOV(CPU* cpu, uint8_t opcode);

// Arithmetic and logic opcodes (8-bit only)
void ADD(CPU* cpu, uint8_t opcode); // Add register to A
void ADI(CPU* cpu, uint8_t opcode); // Add immediate to A
void ADC(CPU* cpu, uint8_t opcode); // Add register to A with carry
void ACI(CPU* cpu, uint8_t opcode); // Add immediate to A with carry

void SUB(CPU* cpu, uint8_t opcode); // Subtract register from A
void SUI(CPU* cpu, uint8_t opcode); // Subtract immediate from A
void SBB(CPU* cpu, uint8_t opcode); // Subtract register from A with borrow
void SBI(CPU* cpu, uint8_t opcode); // Subtract immediate from A with borrow

void INR(CPU* cpu, uint8_t opcode); // Increment register
void DCR(CPU* cpu, uint8_t opcode); // Decrement register

void ANA(CPU* cpu, uint8_t opcode); // AND register with A
void ANI(CPU* cpu, uint8_t opcode); // AND immediate with A
void ORA(CPU* cpu, uint8_t opcode); // OR register with A
void ORI(CPU* cpu, uint8_t opcode); // OR immediate with A
void XRA(CPU* cpu, uint8_t opcode); // ExclusiveOR register with A
void XRI(CPU* cpu, uint8_t opcode); // ExclusiveOR immediate with A
void CMP(CPU* cpu, uint8_t opcode); // Compare register with A
void CPI(CPU* cpu, uint8_t opcode); // Compare immediate with A

// Start!
int main(int argc, char* argv[]) {
    char* filename;
    bool running = true;
    if (argc == 1) filename = "program.mc";
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

    // Fetch-decode-execute loop
    while (cpu.running) {
        if (cpu.program_counter >= MAX_PROGRAM_SIZE) cpu.program_counter = 0;
        uint8_t opcode = cpu.memory[cpu.program_counter];
        Instruction inst = opcode_lookup[opcode];

        if (inst.size == 0) {
            printf("Invalid opcode (0x%02x) detected, exitting\n", opcode);
            return 1;
        }

        inst.execute(&cpu, opcode);
        cpu.program_counter += inst.size;
    }

    if (DEBUG) {
        printf("\nEnding conditions:\n");
        print_cpu_registers(&cpu);
        printf("\n");
        print_cpu_memory(&cpu);
        printf("\n");
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
    // Main registers
    initialize_uint8_register(&cpu->A, 'A', 0);
    initialize_uint8_register(&cpu->B, 'B', 0);
    initialize_uint8_register(&cpu->C, 'C', 0);
    initialize_uint8_register(&cpu->D, 'D', 0);
    initialize_uint8_register(&cpu->E, 'E', 0);
    initialize_uint8_register(&cpu->H, 'H', 0);
    initialize_uint8_register(&cpu->L, 'L', 0);
    initialize_uint8_register(&cpu->M, 'M', 0);

    // Register pairs
    initialize_uint16_register(&cpu->BC, "BC", 0);
    initialize_uint16_register(&cpu->DE, "DE", 0);
    initialize_uint16_register(&cpu->HL, "HL", 0);

    cpu->stack_pointer = 0;
    cpu->program_counter = 0;

    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) cpu->memory[i] = 0;

    // Get program data, put into memory
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return 1;
    }

    if (DEBUG) printf("Opened %s\n", filename);
    fread(cpu->memory, 1, sizeof(cpu->memory), file);
    fclose(file);
    cpu->running = true;
    
    // Set flag register
    initialize_uint8_register(&cpu->flag, 'F', 0);

    return 0;
}

void print_cpu_registers(CPU* cpu) {

    printf("%c = %d, ", cpu->A.name, cpu->A.value);
    printf("%c = %d, ", cpu->B.name, cpu->B.value);
    printf("%c = %d, ", cpu->C.name, cpu->C.value);
    printf("%c = %d, ", cpu->D.name, cpu->D.value);
    printf("%c = %d, ", cpu->E.name, cpu->E.value);
    printf("%c = %d, ", cpu->H.name, cpu->H.value);
    printf("%c = %d, ", cpu->L.name, cpu->L.value);
    printf("%c = %d\n", cpu->M.name, cpu->M.value);

    printf("%s = 0x%04x, ", cpu->BC.name, cpu->BC.value);
    printf("%s = 0x%04x, ", cpu->DE.name, cpu->DE.value);
    printf("%s = 0x%04x\n", cpu->HL.name, cpu->HL.value);

    printf("SP = %d, PC = %d\n", cpu->stack_pointer, cpu->program_counter);
    // TODO: Split the Flag print into its multiple flags
    printf("Flag = %d\nRunning = %d", cpu->flag.value, cpu->running); 
}

void print_cpu_memory(CPU* cpu) {
    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) {
        if (i % MEMORY_WIDTH == 0) printf("0x%0*x\t", MEMORY_WIDTH, i);

        printf("%02x ", cpu->memory[i]);
        if (i % MEMORY_WIDTH == MEMORY_WIDTH - 1) printf("\n");
    }
}

void print_binary(uint8_t byte) {
    printf("0b");
    for (int i = 7; i >= 0; i--) {
        int num = (byte >> i) & 1;
        printf("%d", num);
    }
}

uint8_t fetch(CPU* cpu) {
    return cpu->memory[cpu->program_counter];
}

uint8_register* get_register_ptr(CPU* cpu, uint8_t reg) {
    switch (reg) {
        case 0:
            return &cpu->B;
        case 1:
            return &cpu->C;
        case 2:
            return &cpu->D;
        case 3:
            return &cpu->E;
        case 4:
            return &cpu->E;
        case 5:
            return &cpu->H;
        case 6:
            return &cpu->M;
        case 7:
            return &cpu->A;
    }
}

char* get_register_name(uint8_t reg) {
    switch (reg) {
        case 0:
            return "B";
        case 1:
            return "C";
        case 2:
            return "D";
        case 3:
            return "E";
        case 4:
            return "E";
        case 5:
            return "H";
        case 6:
            return "M";
        case 7:
            return "A";
    }
}

void update_uint16_registers(CPU* cpu) {
    uint16_register reg16_bc = uint8_to_uint16_register(&cpu->B, &cpu->C);
    uint16_register reg16_de = uint8_to_uint16_register(&cpu->D, &cpu->E);
    uint16_register reg16_hl = uint8_to_uint16_register(&cpu->H, &cpu->L);

    cpu->BC.value = reg16_bc.value;
    cpu->DE.value = reg16_de.value;
    cpu->HL.value = reg16_hl.value;
}

void update_flags(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value) {
    /*
    * | S | Z | 0 | AC | 0 | P | 1 | CY |
    * S = Sign bit, bit7 == 1, mask = 0x80
    * Z = Zero bit, byte == 0, mask = 0x40
    * A = AC, does low nibble overflow/borrow to/from high nibble, mask = 0x10
    * P = Even bit, bit0 == 0, mask = 0x04
    * C = CY, does whole byte overflow/borrow, mask = 0x01
    */

    uint8_t curr_flags = cpu->flag.value;


    // S.Z.P.A.C.
    // Set Sign to 1 if negative, otherwise 0
    if ((cpu->A.value & 0x80) == 0x80)  curr_flags |= 0x80;
    else curr_flags &= ~0x80;

    // Set Zero to 1 if equal to 0, otherwise 0
    if (cpu->A.value == 0) curr_flags |= 0x40;
    else curr_flags &= ~0x40;

    int parity = 0;
    int temp = reg_a_value;
    for (int i = 0; i < 7; i++) {
        parity += temp % 2;
        temp >>= 1;
    }

    // Set Pairty to 1 if even, otherwise 0
    // TODO: Check if the SUM of the bits is even, not the number calculated
    if (parity % 2 == 0)  curr_flags |= 0x04;
    else curr_flags &= ~0x04;

    // Set CY to 1 if overflow/borrow, otherwise 0
    if ((uint16_t)reg_a_value + (uint16_t)added_value >= 256) curr_flags |= 0x01;
    else curr_flags &= ~0x01;

    // Set AC to 1 if nibble overflow/borrow, otherwise 0
    if ((reg_a_value & 0x0F) + (added_value & 0x0F) > 0X0F) curr_flags |= 0x10;
    else curr_flags &= ~ 0x10;
    
    return;
}

// Opcode table
void initialize_opcode_lookup() {
    // Set all opcodes to be invalid
    for (int i = 0; i < 256; i++) opcode_lookup[i] = (Instruction) {"INVALID", HLT, 0}; // Size = 0 for invalid opcodes

    int count = 0;
    // Create all MOV opcodes
    for (int i = 0x40; i <= 0x7F; i++) {
        char name[10] = "MOV ";
        char* dest_name = get_register_name((i >> 3) & 7);
        char* src_name = get_register_name(i & 7);
        strcat(name, dest_name);
        strcat(name, ", ");
        strcat(name, src_name);
        opcode_lookup[i] = (Instruction) {name, MOV, 1};
        count++;
    }

    // Create all MVI opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = (i << 3) | 6;
        char name[7] = "MVI, ";
        strcat(name, get_register_name(i));
        opcode_lookup[opcode] = (Instruction) {name, MVI, 2};
        count++;
    }

    // Create all ADD opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 128 + i;
        char name[7] = "ADD ";
        strcat(name, get_register_name(i));
        opcode_lookup[opcode] = (Instruction) {name, ADD, 1};
        count++;
    }

    opcode_lookup[0x00] = (Instruction) {"NOP", NOP, 1};
    opcode_lookup[0x66] = (Instruction) {"HLT", HLT, 1};

    count += 2;

    if (DEBUG) printf("\n%d/256 (%0.2f%%) opcodes implemented", count, (double) count / 2.56);

}

// Opcode function defenitions
void NOP(CPU* cpu, uint8_t opcode) {
    if (DEBUG) printf("NOP\t\t// No operation\n");
}
void HLT(CPU* cpu, uint8_t opcode) { 
    cpu->running = false;
    if (DEBUG) printf("HLT\t\t// Stop the CPU\n");
}

// Data management
void MOV(CPU* cpu, uint8_t opcode) {
    uint8_register* dest_ptr = get_register_ptr(cpu, (opcode >> 3) & 7);
    uint8_register* src_ptr = get_register_ptr(cpu, opcode & 7);
    dest_ptr->value = src_ptr->value;
    if (DEBUG) printf("MOV %c, %c\t// Copy value %d from register %c to register %c\n", dest_ptr->name, src_ptr->name, src_ptr->value,src_ptr->name, dest_ptr->name);
    update_uint16_registers(cpu);
}
void MVI(CPU* cpu, uint8_t opcode) {
    int reg_number = (opcode >> 3) & 7;

    uint8_register* dest_ptr = get_register_ptr(cpu, reg_number);
    int ival = cpu->memory[cpu->program_counter + 1];
    dest_ptr->value = ival;
    if (DEBUG) printf("MVI %c, %d\t// Copy immediate value %d to register %c\n", dest_ptr->name, ival, ival, dest_ptr->name);

    // TODO: Potentially update the register pair possibily affected by this move
    // Now to update any potential register pairs that could of changed from this
    // Only need to change if the register is B, C, D, E, H, or L. Which is most of them.
    update_uint16_registers(cpu);

}

// Arithmetic and logic opcodes (8-bit only)
void ADD(CPU* cpu, uint8_t opcode) { // Add register to A
    // TODO: Add flag things
    uint8_t temp_a = cpu->A.value;
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    cpu->A.value += reg->value;

    if (DEBUG) printf("ADD %c\t\t// Add value %d from register %c to A\n", reg->name, reg->value, reg->name);
}
void ADI(CPU* cpu, uint8_t opcode) {} // Add immediate to A
void ADC(CPU* cpu, uint8_t opcode) {} // Add register to A with carry
void ACI(CPU* cpu, uint8_t opcode) {} // Add immediate to A with carry

void SUB(CPU* cpu, uint8_t opcode) {} // Subtract register from A
void SUI(CPU* cpu, uint8_t opcode) {} // Subtract immediate from A
void SBB(CPU* cpu, uint8_t opcode) {} // Subtract register from A with borrow
void SBI(CPU* cpu, uint8_t opcode) {} // Subtract immediate from A with borrow

void INR(CPU* cpu, uint8_t opcode) {} // Increment register
void DCR(CPU* cpu, uint8_t opcode) {} // Decrement register

void ANA(CPU* cpu, uint8_t opcode) {} // AND register with A
void ANI(CPU* cpu, uint8_t opcode) {} // AND immediate with A
void ORA(CPU* cpu, uint8_t opcode) {} // OR register with A
void ORI(CPU* cpu, uint8_t opcode) {} // OR immediate with A
void XRA(CPU* cpu, uint8_t opcode) {} // ExclusiveOR register with A
void XRI(CPU* cpu, uint8_t opcode) {} // ExclusiveOR immediate with A
void CMP(CPU* cpu, uint8_t opcode) {} // Compare register with A
void CPI(CPU* cpu, uint8_t opcode) {} // Compare immediate with A


#endif