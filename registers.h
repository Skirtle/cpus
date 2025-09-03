#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>
#include <string.h>

typedef struct uint8_register {
    uint8_t value;
    char name;
} uint8_register;

typedef struct uint16_register {
    uint16_t value;
    char name[3];
} uint16_register;

typedef struct int8_register {
    int8_t value;
    char name;
} int8_register;

typedef struct int16_register {
    int16_t value;
    char name[3];
} int16_register;

void combine_names(char* dest, char c1, char c2) {
    dest[0] = c1;
    dest[1] = c2;
    dest[2] = '\0';
}

uint16_register uint8_to_uint16_register(uint8_register* reg_a, uint8_register* reg_b) {
    uint16_register reg_c;
    combine_names(reg_c.name, reg_a->name, reg_b->name);
    reg_c.value = (((uint16_t) reg_a->value) << 8) | reg_b->value;
    return reg_c;
}

#endif