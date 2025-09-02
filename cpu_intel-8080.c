#include <stdio.h>
#include <stdlib.h>

#define REGISTER_COUNT 7
#define REGISTER_NAMES (char[REGISTER_COUNT]){'A', 'B', 'C', 'D', 'E', 'H', 'L'}
#define DEBUG 1


typedef struct Register {
    char name; // For debugging purposes
    char value;
} Register; 

Register* initialize_registers(char default_value);
void print_registers(Register* registers);
void print_register(Register* reg);

int main(int argc, int* argv) {
    // Initializing registers
    Register* registers = initialize_registers(0);
    if (registers == NULL) return 1;

    if (DEBUG) {
        printf("Initialized registers ");
        print_registers(registers);
    }

    // Status registers (flags)
    Register status_register;
    status_register.name = 'F';
    status_register.value = 0;

    free(registers);
    return 0;
}


Register* initialize_registers(char default_value) {
    Register* registers = malloc(sizeof(Register) * REGISTER_COUNT);
    if (registers == NULL) return NULL; 

    for (int i = 0; i < REGISTER_COUNT; i++) {
        Register temp_register;
        temp_register.name = REGISTER_NAMES[i];
        temp_register.value = default_value;
        registers[i] = temp_register;
    }

    return registers;
}

void print_registers(Register* registers) {
    for (int i = 0; i < REGISTER_COUNT; i++) {
        printf("%c = %d", registers[i].name, registers[i].value);
        if (i != REGISTER_COUNT - 1) printf(", ");
        else printf("\n");
    }
}

void print_register(Register* reg) {
    printf("Register %c: %d\n", reg->name, reg->value);
}

