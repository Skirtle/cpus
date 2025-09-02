#include <stdio.h>
#include <stdlib.h>

#define REGISTER_COUNT 7
#define REGISTER_NAMES (char[REGISTER_COUNT]){'A', 'B', 'C', 'D', 'E', 'H', 'L'}


typedef struct Register {
    char name; // For debugging purposes
    char value;
} Register; 

Register* initialize_registers();

int main(int argc, int* argv) {

    Register* registers = initialize_registers();
    if (registers == NULL) return 1;

    char names[] = {'A', 'B', 'C', 'D', 'E', 'H', 'L'};

    for (int i = 0; i < REGISTER_COUNT; i++) {
        printf("Register %c: %d\n", registers[i].name, registers[i].value);
    }

    free(registers);
    return 0;
}


Register* initialize_registers() {
    Register* registers = malloc(sizeof(Register) * REGISTER_COUNT);
    if (registers == NULL) return NULL; 

    for (int i = 0; i < REGISTER_COUNT; i++) {
        // A, B, C, D, E, H, L
        Register reg;
        reg.name = REGISTER_NAMES[i];
        reg.value = 0;
        registers[i] = reg;
    }

    return registers;
}