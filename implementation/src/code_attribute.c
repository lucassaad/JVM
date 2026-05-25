#include <stdio.h>
#include <stdlib.h>

#include "../include/code_attribute.h"
#include "../include/utils.h"

Code_attribute *read_code_attribute(FILE *file)
{
    Code_attribute *code = malloc(sizeof(Code_attribute));

    if (code == NULL)
        return NULL;

    // max_stacks
    fread(&code->max_stack, sizeof(uint16_t), 1, file);
    code->max_stack = byteswap_u2(code->max_stack);

    //max_locals
    fread(&code->max_locals, sizeof(uint16_t), 1, file);
    code->max_locals = byteswap_u2(code->max_locals);

    //code_length
    fread(&code->code_length, sizeof(uint32_t), 1, file);
    code->code_length = byteswap_u4(code->code_length);

    // bytecode
    code->code = malloc(code->code_length);

    fread(code->code, 1, code->code_length, file);

    return code;
}

void print_code_attribute(Code_attribute *code)
{
    printf("Code Attribute\n");

    printf("max_stack: %u\n", code->max_stack);

    printf("max_locals: %u\n", code->max_locals);

    printf("code_length: %u\n", code->code_length);

    for (uint32_t i = 0; i < code->code_length; i++)
    {
        printf("%02X ", code->code[i]);
    }

    printf("\n");
}

void free_code_attribute(Code_attribute *code)
{
    if (code == NULL)
        return;

    free(code->code);

    free(code);
}