#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <stdint.h>
#include <stdio.h>
#include "class_file.h"

// Estrutura dos Atributos
typedef struct {
    uint16_t constantvalue_index;
} ConstantValue_attribute;

typedef struct {
    uint16_t number_of_exceptions;
    uint16_t *exception_index_table;
} Exceptions_attribute;

typedef struct {
    uint16_t inner_class_info_index;
    uint16_t outer_class_info_index;
    uint16_t inner_name_index;
    uint16_t inner_class_access_flags;
} InnerClass_info;

typedef struct {
    uint16_t number_of_classes;
    InnerClass_info *classes;
} InnerClasses_attribute;

typedef struct {
    uint16_t sourcefile_index;
} SourceFile_attribute;

typedef struct {
    uint16_t start_pc;
    uint16_t line_number;
} LineNumber_info;

typedef struct {
    uint16_t line_number_table_length;
    LineNumber_info *line_number_table;
} LineNumberTable_attribute;

typedef struct {
    uint16_t start_pc;
    uint16_t length;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t index;
} LocalVariable_info;

typedef struct {
    uint16_t local_variable_table_length;
    LocalVariable_info *local_variable_table;
} LocalVariableTable_attribute;

typedef struct {
    uint16_t start_pc;
    uint16_t end_pc;
    uint16_t handler_pc;
    uint16_t catch_type;
} ExceptionTable_info;

typedef struct {
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    uint16_t exception_table_length;
    ExceptionTable_info *exception_table;
    uint16_t attributes_count;
    attribute_info *attributes;
} Code_attribute;

// ---------------------------------------------------------------------------------------
// Função para ler o payload específico do atributo do arquivo .class
void* read_specific_attribute_info(ClassFile *cf, FILE *file, const char* attr_name, uint32_t attr_length);

#endif