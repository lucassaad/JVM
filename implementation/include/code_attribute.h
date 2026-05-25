#ifndef CODE_ATTRIBUTE_H
#define CODE_ATTRIBUTE_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{

    uint16_t max_stack;
    uint16_t max_locals;

    uint32_t code_length;
    uint8_t *code;

} Code_attribute;

//   Lê um atributo Code do arquivo .class

Code_attribute *read_code_attribute(FILE *fp);

//   Exibe o atributo Code

void print_code_attribute(Code_attribute *code);

//   Libera memória

void free_code_attribute(Code_attribute *code);

#endif