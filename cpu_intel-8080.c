#ifndef INTEL_8080
#define INTEL_8080

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "registers.h"
#include "colors.h"

#define DEBUG true
#define MAX_PROGRAM_SIZE 64
#define MEMORY_WIDTH 8
#define MAX_PORTS 256

#define OPCODE_COLOR BLUE
#define COMMENT_COLOR DIM
#define IMMEDIATE_COLOR YELLOW
#define REGISTER_COLOR YELLOW_BRIGHT

// Structs
typedef struct CPU {
    uint8_register A, B, C, D, E, H, L, M, flag;
    uint16_register BC, DE, HL;
    uint16_t stack_pointer;
    uint16_t program_counter; 
    uint8_t memory[MAX_PROGRAM_SIZE];
    uint8_t ports[MAX_PORTS];
    bool running;
} CPU;

typedef struct Instruction {
    char name[16]; // For debugging
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
void update_uint8_registers(CPU* cpu);
void update_flags_add(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value);
void update_flags_sub(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value);
void update_16_reg(CPU* cpu); // TODO: Implement. Updates BC, DE, and HL based off of their components
void update_8_reg(CPU* cpu); // TODO: Implement. Updates the components of BC, DE, and HL
void update_flag_S(CPU* cpu); // Sign
void update_flag_Z(CPU* cpu); // Zero
void update_flag_A(CPU* cpu, uint8_t a, uint8_t b, uint8_t c); // Nibble carry/borrow
void update_flag_P(CPU* cpu); // Parity
void update_flag_C(CPU* cpu, uint8_t a, uint8_t b, uint8_t c); // Byte carry/borrow
void print_8bit_registers(CPU* cpu);
void print_16bit_registers(CPU* cpu);
void print_states_and_flags(CPU* cpu);
char* get_register_name(uint8_t reg);
int initialize_cpu(CPU* cpu, char* filename);
uint8_t fetch(CPU* cpu);
uint8_t get_flag_S(CPU* cpu); // Sign
uint8_t get_flag_Z(CPU* cpu); // Zero
uint8_t get_flag_P(CPU* cpu); // Parity
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

// Input and output
void OUT(CPU* cpu, uint8_t opcode); // Write A to output port

// Start!
int main(int argc, char* argv[]) {
    char* filename;
    bool running = true;
    if (argc == 1) filename = "program.bin";
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
    bool error_stop = false;
    while (cpu.running) {
        if (cpu.program_counter >= MAX_PROGRAM_SIZE) cpu.program_counter = 0;
        uint8_t opcode = cpu.memory[cpu.program_counter];
        Instruction inst = opcode_lookup[opcode];

        if (inst.size == 0) {
            printf("%sInvalid opcode (0x%02x) detected, exitting\n%s", RED, opcode, RESET);
            error_stop = true;
            break;
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
// Initialize
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
    initialize_uint8_register(&cpu->flag, 'F', 0b00000010);

    return 0;
}

// Printing
void print_cpu_registers(CPU* cpu) {
    print_8bit_registers(cpu);
    print_16bit_registers(cpu);
    print_states_and_flags(cpu);
}  
void print_cpu_memory(CPU* cpu) {
    int last_index = -1;

    for (int i = MAX_PROGRAM_SIZE - 1; i >= 0; i--) {
        if (cpu->memory[i] != 0) {
            last_index = i + 1;
            break;
        }
    }

    int last_row = (last_index - 1) / MEMORY_WIDTH;

    for (int i = 0; i < MAX_PROGRAM_SIZE; i++) {
        // Address block
        int current_row = i / MEMORY_WIDTH;
        if (current_row <= last_row) printf("%s", GREEN);
        else printf("%s", DIM);

        if (i % MEMORY_WIDTH == 0) printf("0x%0*x\t", MEMORY_WIDTH, i);
        printf("%s", RESET);

        // Actual data
        if (i < last_index) printf("%s", GREEN);
        else printf("%s", DIM);
        printf("%02x %s", cpu->memory[i], RESET);
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
void print_8bit_registers(CPU* cpu) {
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->A.name, IMMEDIATE_COLOR, cpu->A.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->B.name, IMMEDIATE_COLOR, cpu->B.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->C.name, IMMEDIATE_COLOR, cpu->C.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->D.name, IMMEDIATE_COLOR, cpu->D.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->E.name, IMMEDIATE_COLOR, cpu->E.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->H.name, IMMEDIATE_COLOR, cpu->H.value);
    printf("%s%c = %s%d, ", REGISTER_COLOR, cpu->L.name, IMMEDIATE_COLOR, cpu->L.value);
    printf("%s%c = %s%d\n%s", REGISTER_COLOR, cpu->M.name, IMMEDIATE_COLOR, cpu->M.value, RESET);
}
void print_16bit_registers(CPU* cpu) {
    printf("%s%s = %s0x%04x, ", REGISTER_COLOR, cpu->BC.name, IMMEDIATE_COLOR, cpu->BC.value);
    printf("%s%s = %s0x%04x, ", REGISTER_COLOR, cpu->DE.name, IMMEDIATE_COLOR, cpu->DE.value);
    printf("%s%s = %s0x%04x\n%s", REGISTER_COLOR, cpu->HL.name, IMMEDIATE_COLOR, cpu->HL.value, RESET);
}
void print_states_and_flags(CPU* cpu) {
    printf("%sSP = %d, PC = %d, ", MAGENTA, cpu->stack_pointer, cpu->program_counter);
    // TODO: Split the Flag print into its multiple flags
    printf("Flag = ");
    print_binary(cpu->flag.value);
    printf(" (0x%02x), Running = %d%s",cpu->flag.value, cpu->running, RESET); 
}

// Getting
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
            return &cpu->H;
        case 5:
            return &cpu->L;
        case 6:
            return &cpu->M;
        case 7:
            return &cpu->A;
    }
}
char* get_register_name(uint8_t reg) {
    char* names[] = {"B", "C", "D", "E", "H", "L", "M", "A"};
    return names[reg & 7];
}

// Updating registers
void update_uint16_registers(CPU* cpu) {
    uint16_register reg16_bc = uint8_to_uint16_register(&cpu->B, &cpu->C);
    uint16_register reg16_de = uint8_to_uint16_register(&cpu->D, &cpu->E);
    uint16_register reg16_hl = uint8_to_uint16_register(&cpu->H, &cpu->L);

    cpu->BC.value = reg16_bc.value;
    cpu->DE.value = reg16_de.value;
    cpu->HL.value = reg16_hl.value;
}
void update_uint8_registers(CPU* cpu) {
    cpu->B.value = (uint8_t) (cpu->BC.value >> 8);
    cpu->C.value = (uint8_t) (cpu->BC.value & 0x0F);

    cpu->D.value = (uint8_t) (cpu->DE.value >> 8);
    cpu->E.value = (uint8_t) (cpu->DE.value & 0x0F);

    cpu->H.value = (uint8_t) (cpu->HL.value >> 8);
    cpu->L.value = (uint8_t) (cpu->HL.value & 0x0F);
}

// Updating flags SZ0A0P1C
uint8_t get_flag_S(CPU* cpu) { // Sign
    // Set Sign to 1 if negative, otherwise 0
    if ((cpu->A.value & 0x80) == 0x80)  return 0x80;
    return 0;
}
uint8_t get_flag_Z(CPU* cpu) { // Zero
    // Set Zero to 1 if equal to 0, otherwise 0
    if (cpu->A.value == 0) return 0x40;
    return 0;
}
uint8_t get_flag_P(CPU* cpu) { // Parity
    int parity = 0;
    int temp = cpu->A.value;
    for (int i = 0; i <= 7; i++) {
        parity += temp & 1;
        temp >>= 1;
    }

    // Set Pairty to 1 if even number of bits, otherwise 0
    if (parity % 2 == 0)  return 0x04;
    return 0;
}
void update_flag_S(CPU* cpu) {
    // Set Sign to 1 if negative, otherwise 0
    if ((cpu->A.value & 0x80) == 0x80)  cpu->flag.value |= 0x80;
    cpu->flag.value &= ~0x80;
}
void update_flag_Z(CPU* cpu) {
    // Set Zero to 1 if equal to 0, otherwise 0
    if (cpu->A.value == 0) cpu->flag.value |= 0x40;
    cpu->flag.value &= ~0x40;
}
void update_flag_P(CPU* cpu) {
    int parity = 0;
    int temp = cpu->A.value;
    for (int i = 0; i <= 7; i++) {
        parity += temp & 1;
        temp >>= 1;
    }

    // Set Pairty to 1 if even number of bits, otherwise 0
    if (parity % 2 == 0)  cpu->flag.value |= 0x04;
    cpu->flag.value &= ~0x04;
}
void update_flags_add(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value) {
    uint8_t curr_flags = cpu->flag.value;
    uint8_t a = reg_a_value;
    uint8_t b = added_value;
    uint8_t c = 0;
    if ((0x88 <= opcode && opcode <= 0x8F) || opcode == 0xCE) c = 1; // If the opcode is for ADC (0x88-0x8F) or ACI (0xCE) 
    c *= cpu->flag.value & 1;

    // S.Z.P.A.C.
    get_flag_S(cpu);
    get_flag_Z(cpu);
    get_flag_P(cpu);

    // Set CY to 1 if overflow, otherwise 0
    if ((uint16_t)a + (uint16_t)b + c > 0xFF) curr_flags |= 0x01;
    else curr_flags &= ~0x01;

    // Set AC to 1 if nibble overflow, otherwise 0
    if ((a & 0x0F) + (b & 0x0F) + c > 0x0F) curr_flags |= 0x10;
    else curr_flags &= ~ 0x10;

    cpu->flag.value = curr_flags;
}
void update_flags_sub(CPU* cpu, uint8_t opcode, uint8_t reg_a_value, uint8_t added_value) {
    uint8_t curr_flags = cpu->flag.value;
    uint8_t a = reg_a_value;
    uint8_t b = added_value;
    uint8_t c = 0;
    if ((0x98 <= opcode && opcode <= 0x9F) || opcode == 0xDE) c = 1;
    c *= cpu->flag.value & 1;

    // S.Z.P.A.C.
    uint8_t s_flag = get_flag_S(cpu);
    uint8_t z_flag = get_flag_Z(cpu);
    uint8_t p_flag = get_flag_P(cpu);

    curr_flags = curr_flags | s_flag | z_flag | p_flag;

    // Set CY to 1 if borrow, otherwise 0
    if (a < b + c) curr_flags |= 0x01;
    else curr_flags &= ~0x01;

    // Set AC to 1 if nibble borrow, otherwise 0
    if ((a & 0x0F) < ((b + c) & 0x0F)) curr_flags |= 0x10;
    else curr_flags &= ~0x10;

    cpu->flag.value = curr_flags;
}

// Opcode table
void initialize_opcode_lookup() {
    // Set all opcodes to be invalid
    for (int i = 0; i < 256; i++) opcode_lookup[i] = (Instruction) {"INVALID", HLT, 0}; // Size = 0 for invalid opcodes

    // Create all MOV opcodes
    for (int i = 0x40; i <= 0x7F; i++) {
        char* dest_name = get_register_name((i >> 3) & 7);
        char* src_name = get_register_name(i & 7);
        opcode_lookup[i] = (Instruction) {"", MOV, 1};
        snprintf(opcode_lookup[i].name, sizeof(opcode_lookup[i].name), "MOV %s, %s", dest_name, src_name);
    }

    // Create all MVI opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = (i << 3) | 6;
        opcode_lookup[opcode] = (Instruction) {"", MVI, 2};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "MVI %s", get_register_name(i));
    }

    // Create all ADD opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 128 + i;
        opcode_lookup[opcode] = (Instruction) {"", ADD, 1};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "ADD %s", get_register_name(i));
    }

    // Create all ADC opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 136 + i;
        opcode_lookup[opcode] = (Instruction) {"", ADC, 1};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "ADC %s", get_register_name(i));
    }

    // Create all SUB opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 144 + i;
        opcode_lookup[opcode] = (Instruction) {"", SUB, 1};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "SUB %s", get_register_name(i));
    }

    // Create all SBB opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 152 + i;
        opcode_lookup[opcode] = (Instruction) {"", SBB, 1};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "SBB %s", get_register_name(i));
    }

    // Create all INR and DCR opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t inr_opcode = 4 + (i << 3);
        uint8_t dcr_opcode = 5 + (i << 3);
        opcode_lookup[inr_opcode] = (Instruction) {"", INR, 1};
        opcode_lookup[dcr_opcode] = (Instruction) {"", DCR, 1};
        snprintf(opcode_lookup[inr_opcode].name, sizeof(opcode_lookup[inr_opcode].name), "INR %s", get_register_name(i));
        snprintf(opcode_lookup[dcr_opcode].name, sizeof(opcode_lookup[dcr_opcode].name), "DCR %s", get_register_name(i));
    }

    // Create all ANA, ORA, XRA opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t ana_opcode = 160 + i;
        uint8_t ora_opcode = 176 + i;
        uint8_t xra_opcode = 1668 + i;
        opcode_lookup[ana_opcode] = (Instruction) {"", ANA, 1};
        opcode_lookup[ora_opcode] = (Instruction) {"", ORA, 1};
        opcode_lookup[xra_opcode] = (Instruction) {"", XRA, 1};
        snprintf(opcode_lookup[ana_opcode].name, sizeof(opcode_lookup[ana_opcode].name), "ANA %s", get_register_name(i));
        snprintf(opcode_lookup[ora_opcode].name, sizeof(opcode_lookup[ora_opcode].name), "ORA %s", get_register_name(i));
        snprintf(opcode_lookup[xra_opcode].name, sizeof(opcode_lookup[xra_opcode].name), "XRA %s", get_register_name(i));
    }

    // Create all CMP opcodes
    for (int i = 0; i <= 7; i++) {
        uint8_t opcode = 184 + i;
        opcode_lookup[opcode] = (Instruction) {"", CMP, 1};
        snprintf(opcode_lookup[opcode].name, sizeof(opcode_lookup[opcode].name), "CMP %s", get_register_name(i));
    }

    opcode_lookup[0x00] = (Instruction) {"NOP", NOP, 1};
    opcode_lookup[0x76] = (Instruction) {"HLT", HLT, 1};
    opcode_lookup[0xC6] = (Instruction) {"ADI", ADI, 2};
    opcode_lookup[0xCE] = (Instruction) {"ACI", ACI, 2};
    opcode_lookup[0xD3] = (Instruction) {"OUT", OUT, 2};
    opcode_lookup[0xD6] = (Instruction) {"SUI", SUI, 2};
    opcode_lookup[0xDE] = (Instruction) {"SBI", SBI, 2};
    opcode_lookup[0xE6] = (Instruction) {"ANI", ANI, 2};
    opcode_lookup[0xF6] = (Instruction) {"ORI", ORI, 2};
    opcode_lookup[0xEE] = (Instruction) {"XRI", XRI, 2};
    opcode_lookup[0xFE] = (Instruction) {"CPI", CPI, 2};

    int count = 0;
    for (int i = 0; i < 256; i++) { 
        if (opcode_lookup[i].size != 0) {
            count += 1;
            // printf("\nOpcode %d: %s", i, opcode_lookup[i].name);
        }
    }
    if (DEBUG) printf("\n%d/256 (%0.2f%%) opcodes implemented", count, count / 2.56);

    // TODO: All size 0 instructions become NOP after we do all the counting, will need to also fix the error checking in main

}

// Opcode function defenitions
void NOP(CPU* cpu, uint8_t opcode) {
    if (DEBUG) printf("%sNOP\t\t// No operation\n%s", DIM, RESET);
}
void HLT(CPU* cpu, uint8_t opcode) { 
    cpu->running = false;
    if (DEBUG) printf("%s%sHLT%s%s\t\t// Stop the CPU\n%s", DIM, RED, RESET, DIM, RESET);
}

// Data management (8-bit only)
void MOV(CPU* cpu, uint8_t opcode) {
    uint8_register* dest_ptr = get_register_ptr(cpu, (opcode >> 3) & 7);
    uint8_register* src_ptr = get_register_ptr(cpu, opcode & 7);
    dest_ptr->value = src_ptr->value;
    if (DEBUG) printf("%sMOV %s%c, %c\t%s%s// Copy value %d from register %c to register %c\n%s", OPCODE_COLOR, REGISTER_COLOR, dest_ptr->name, src_ptr->name, RESET, DIM, src_ptr->value,src_ptr->name, dest_ptr->name, RESET);
    update_uint16_registers(cpu);
}
void MVI(CPU* cpu, uint8_t opcode) {
    int reg_number = (opcode >> 3) & 7;

    uint8_register* dest_ptr = get_register_ptr(cpu, reg_number);
    int ival = cpu->memory[cpu->program_counter + 1];
    dest_ptr->value = ival;
    if (DEBUG) printf("%sMVI %s%c, %s%d\t%s%s// Copy immediate value %d to register %c\n%s", OPCODE_COLOR, REGISTER_COLOR, dest_ptr->name, IMMEDIATE_COLOR, ival, RESET, DIM, ival, dest_ptr->name, RESET);

    // TODO: Potentially update the register pair possibily affected by this move
    // Now to update any potential register pairs that could of changed from this
    // Only need to change if the register is B, C, D, E, H, or L. Which is most of them.
    update_uint16_registers(cpu);

}

// Arithmetic and logic opcodes (8-bit only)
void ADD(CPU* cpu, uint8_t opcode) { // Add register to A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    uint8_t a = cpu->A.value;
    uint8_t b = reg->value;
    cpu->A.value += reg->value;
    if (DEBUG) printf("%sADD %s%c\t%s\t%s// Add value %d from register %c to A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, DIM, reg->value, reg->name, RESET);
    update_flags_add(cpu, opcode, a, reg->value);
}
void ADI(CPU* cpu, uint8_t opcode) { // Add immediate to A
    uint8_t a = cpu->A.value;
    uint8_t b = cpu->memory[cpu->program_counter + 1];
    cpu->A.value += b;
    if (DEBUG) printf("%sADI %s%u\t%s\t%s// Add immediate value %u to A\n%s", OPCODE_COLOR, IMMEDIATE_COLOR, b, RESET, DIM, b, RESET);
    update_flags_add(cpu, opcode, a, b);
} 
void ADC(CPU* cpu, uint8_t opcode) { // Add register to A with carry
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    uint8_t a = cpu->A.value;
    uint8_t b = reg->value;
    uint8_t c = cpu->flag.value & 1;
    cpu->A.value = a + b + c;
    if (DEBUG) printf("%sADC %s%c\t%s\t%s// Add value %d and carry %d from register %c to A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, DIM, reg->value, c, reg->name, RESET);
    update_flags_add(cpu, opcode, a, reg->value);
} 
void ACI(CPU* cpu, uint8_t opcode) { // Add immediate to A with carry
    uint8_t a = cpu->A.value;
    uint8_t b = cpu->memory[cpu->program_counter + 1];
    uint8_t c = cpu->flag.value & 1;
    cpu->A.value += b + c;
    if (DEBUG) printf("%sACI %s%u\t%s\t%s// Add immediate value %u and carry %d to A\n%s", OPCODE_COLOR, IMMEDIATE_COLOR, b, RESET, DIM, b, c, RESET);
    update_flags_add(cpu, opcode, a, b);
}
void SUB(CPU* cpu, uint8_t opcode) { // Subtract register from A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    uint8_t a = cpu->A.value;
    uint8_t b = reg->value;
    cpu->A.value -= reg->value;
    if (DEBUG) printf("%sSUB %s%c\t\t%s%s// Subtract value %d from register %c from A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->value, reg->name, RESET);
    update_flags_sub(cpu, opcode, a, reg->value);
} 
void SUI(CPU* cpu, uint8_t opcode) { // Subtract immediate from A
    uint8_t a = cpu->A.value;
    uint8_t b = cpu->memory[cpu->program_counter + 1];
    cpu->A.value -= b;
    if (DEBUG) printf("%sSUI %s%u\t\t%s%s// Subtract immediate value %u from A\n%s", OPCODE_COLOR, IMMEDIATE_COLOR, b, RESET, COMMENT_COLOR, b, RESET);
    update_flags_sub(cpu, opcode, a, b);
} 
void SBB(CPU* cpu, uint8_t opcode) { // Subtract register from A with borrow
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    uint8_t a = cpu->A.value;
    uint8_t b = reg->value;
    uint8_t c = cpu->flag.value & 1;
    cpu->A.value = a - b - c;
    if (DEBUG) printf("%sSBB %s%c\t\t%s%s// Subtract value %d and borrow %d from register %c from A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->value, c, reg->name, RESET);
    update_flags_sub(cpu, opcode, a, reg->value);
} 
void SBI(CPU* cpu, uint8_t opcode) { // Subtract immediate from A with borrow
    uint8_t a = cpu->A.value;
    uint8_t b = cpu->memory[cpu->program_counter + 1];
    uint8_t c = cpu->flag.value & 1;
    cpu->A.value -= b - c;
    if (DEBUG) printf("%sSBB %s%u\t\t%s%s// Subtract immediate value %u and borrow %u from A\n%s", OPCODE_COLOR, IMMEDIATE_COLOR, b, RESET, COMMENT_COLOR, b, c, RESET);
    update_flags_sub(cpu, opcode, a, b);
} 
void INR(CPU* cpu, uint8_t opcode) { // Increment register
    uint8_register* reg = get_register_ptr(cpu, (opcode >> 3) & 7);
    reg->value++;
    if (DEBUG) printf("%sINR %s%c\t%s\t%s// Increment register %c\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, DIM, reg->name, RESET);
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    if (reg->value - 1 == 0x0F) { cpu->flag.value |= 0x10; } // carry = true
    else { cpu->flag.value &= ~ 0x10; } // carry = false
}
void DCR(CPU* cpu, uint8_t opcode) { // Decrement register
    uint8_register* reg = get_register_ptr(cpu, (opcode >> 3) & 7);
    reg->value--;
    if (DEBUG) printf("%sDCR %s%c\t%s\t%s// Decrement register %c\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, DIM, reg->name, RESET);
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    if (reg->value + 1 == 0x10) {cpu->flag.value |= 0x10; } // borrow = true
    else {cpu->flag.value &= ~0x10; } // borrow = false
}
void ANA(CPU* cpu, uint8_t opcode) { // AND register with A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    cpu->A.value = cpu->A.value & reg->value;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x01; // CY = 0, always
    cpu->flag.value |= 0x10; // AC = 1 for ANA/ANI only
    if (DEBUG) printf("%sANA %s%c\t\t%s%s// Logical AND register %c with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->name, RESET);
}
void ANI(CPU* cpu, uint8_t opcode) { // AND immediate with A
    uint8_t val = cpu->memory[cpu->program_counter + 1];
    cpu->A.value = cpu->A.value & val;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x01; // CY = 0, always
    cpu->flag.value |= 0x10; // AC = 1 for ANA/ANI only
    if (DEBUG) printf("%sANI %s%u\t\t%s%s// Logical AND immediate %u with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, val, RESET, COMMENT_COLOR, val, RESET);
}
void ORA(CPU* cpu, uint8_t opcode) { // OR register with A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    cpu->A.value = cpu->A.value | reg->value;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x11; // CY = AC = 0
    if (DEBUG) printf("%sORA %s%c\t\t%s%s// Logical OR register %c with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->name, RESET);
}
void ORI(CPU* cpu, uint8_t opcode) { // OR immediate with A
    uint8_t val = cpu->memory[cpu->program_counter + 1];
    cpu->A.value = cpu->A.value | val;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x11; // CY = AC = 0
    if (DEBUG) printf("%sORI %s%u\t\t%s%s// Logical OR immediate %u with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, val, RESET, COMMENT_COLOR, val, RESET);
}
void XRA(CPU* cpu, uint8_t opcode) { // Exclusive OR register with A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    cpu->A.value = cpu->A.value ^ reg->value;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x11; // CY = AC = 0
    if (DEBUG) printf("%sXRA %s%c\t\t%s%s// Logical OR register %c with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->name, RESET);
}
void XRI(CPU* cpu, uint8_t opcode) { // Exclusive OR immediate with A
    uint8_t val = cpu->memory[cpu->program_counter + 1];
    cpu->A.value = cpu->A.value ^ val;
    update_flag_S(cpu);
    update_flag_Z(cpu);
    update_flag_P(cpu);
    cpu->flag.value &= ~0x11; // CY = AC = 0
    if (DEBUG) printf("%sXRI %s%u\t\t%s%s// Logical OR immediate %u with register A\n%s", OPCODE_COLOR, REGISTER_COLOR, val, RESET, COMMENT_COLOR, val, RESET);
}
void CMP(CPU* cpu, uint8_t opcode) { // Compare register with A
    uint8_register* reg = get_register_ptr(cpu, opcode & 7);
    uint8_t a = cpu->A.value;
    uint8_t b = reg->value;
    cpu->A.value -= reg->value;
    update_flags_sub(cpu, opcode, a, reg->value);
    cpu->A.value = a;
    if (DEBUG) printf("%sCMP %s%c\t\t%s%s// Compare value %d from register %c to A\n%s", OPCODE_COLOR, REGISTER_COLOR, reg->name, RESET, COMMENT_COLOR, reg->value, reg->name, RESET);
}
void CPI(CPU* cpu, uint8_t opcode) { // Compare immediate with A
    uint8_t a = cpu->A.value;
    uint8_t b = cpu->memory[cpu->program_counter + 1];
    cpu->A.value -= b;
    update_flags_sub(cpu, opcode, a, b);
    cpu->A.value = a;
    if (DEBUG) printf("%sCPI %s%u\t\t%s%s// Compare immediate value %u to A\n%s", OPCODE_COLOR, IMMEDIATE_COLOR, b, RESET, COMMENT_COLOR, b, RESET);
}

// Input and output
void OUT(CPU* cpu, uint8_t opcode) { // Write A to output port
    uint8_t port_number = cpu->memory[cpu->program_counter + 1];
    cpu->ports[port_number] = cpu->A.value;
    if (DEBUG) printf("%sOUT %d\t\t// ", DIM, port_number);
    if (port_number == 0) {
        if (DEBUG) printf("Write register A (0x%02x) to output\n%s", cpu->A.value, RESET);
        printf("OUTPUT: %u\n", cpu->A.value);
    }
    else if (port_number == 1) { // Print all main 8-bit registers
        if (DEBUG) printf("Write all 8-bit registers to output\n%s", RESET);
        print_8bit_registers(cpu);
    }
    else if (port_number == 2) { // Print all main 16-bit registers
        if (DEBUG) printf("Write all 16-bit registers to output\n%s", RESET);
        print_16bit_registers(cpu);
    }

    else if (port_number == 3) { // Print all other CPU information
        if (DEBUG) printf("Write all SP, PC, flag, and state to output to output\n%s", RESET);
        print_states_and_flags(cpu);
        printf("\n");
    }

    if (DEBUG) printf("%s", RESET);
} 


#endif