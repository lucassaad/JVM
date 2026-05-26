#include <stdio.h>
#include <stdlib.h>

#include "../include/code_attribute.h"
#include "../include/utils.h"

Code_attribute *read_code_attribute(FILE *file)
{
    Code_attribute *code = malloc(sizeof(Code_attribute));
    if (code == NULL) return NULL;

    fread(&code->max_stack, sizeof(uint16_t), 1, file);
    code->max_stack = byteswap_u2(code->max_stack);

    fread(&code->max_locals, sizeof(uint16_t), 1, file);
    code->max_locals = byteswap_u2(code->max_locals);

    fread(&code->code_length, sizeof(uint32_t), 1, file);
    code->code_length = byteswap_u4(code->code_length);

    code->code = malloc(code->code_length);
    fread(code->code, 1, code->code_length, file);

    // Pular exception_table
    uint16_t exception_table_length;
    fread(&exception_table_length, sizeof(uint16_t), 1, file);
    exception_table_length = byteswap_u2(exception_table_length);
    fseek(file, exception_table_length * 8, SEEK_CUR);

    // Pular sub-atributos (LineNumberTable, LocalVariableTable, etc.)
    uint16_t attributes_count;
    fread(&attributes_count, sizeof(uint16_t), 1, file);
    attributes_count = byteswap_u2(attributes_count);

    for (int i = 0; i < attributes_count; i++) {
        fseek(file, 2, SEEK_CUR);
        uint32_t attr_len;
        fread(&attr_len, sizeof(uint32_t), 1, file);
        attr_len = byteswap_u4(attr_len);
        fseek(file, attr_len, SEEK_CUR);
    }

    return code;
}

void free_code_attribute(Code_attribute *code)
{
    if (code == NULL)
        return;

    free(code->code);

    free(code);
}