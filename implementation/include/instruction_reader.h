#ifndef INSTRUCTION_READER_H
#define INSTRUCTION_READER_H

#include "class_file.h"
#include "attributes.h"

// Função para varrer o array de bytes de um atributo Code, decodifica os opcodes e imprime as instruções mnemônicas correspondentes (ex: aload_0) junto com seus operandos.
void read_instructions(
    FILE *out,
    ClassFile* cf,
    Code_attribute* code
);

#endif