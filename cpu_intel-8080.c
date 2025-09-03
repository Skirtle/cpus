#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "registers.h"

#define REGISTER_COUNT 7
#define REGISTER_NAMES (char[REGISTER_COUNT]){'A', 'B', 'C', 'D', 'E', 'H', 'L'}
#define DEBUG 1


uint8_register* initialize_registers(char default_value);
void print_uint8_registers(uint8_register* registers);
void print_uint8_register(uint8_register* reg);

int main(int argc, int* argv) {
    // Initializing registers
    uint8_register* registers = initialize_registers(0);
    if (registers == NULL) return 1;

    if (DEBUG) {
        printf("Initialized registers\n");
        print_uint8_registers(registers);
    }

    // Status registers (flags)
    uint8_register status_register;
    status_register.name = 'F';
    status_register.value = 0;
    if (DEBUG) print_uint8_register(&status_register);

    free(registers);
    return 0;
}


uint8_register* initialize_registers(char default_value) {
    uint8_register* registers = malloc(sizeof(uint8_register) * REGISTER_COUNT);
    if (registers == NULL) return NULL; 

    for (int i = 0; i < REGISTER_COUNT; i++) {
        uint8_register temp_register;
        temp_register.name = REGISTER_NAMES[i];
        temp_register.value = default_value;
        registers[i] = temp_register;
    }

    return registers;
}

void print_uint8_registers(uint8_register* registers) {
    for (int i = 0; i < REGISTER_COUNT; i++) {
        print_uint8_register(&registers[i]);
    }
}

void print_uint8_register(uint8_register* reg) {
    printf("uint8_register %c: %d\n", reg->name, reg->value);
}

