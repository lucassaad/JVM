#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

// Estrutura que representa uma instrução da JVM
typedef struct {
    char *name;          // Mnemônico 
    int operand_bytes;   // Quantos bytes extras essa instrução consome no array de code
} instruction;

extern instruction opcode_table[256];

#endif