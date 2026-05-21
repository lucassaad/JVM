#ifndef CLASS_FILE_H
#define CLASS_FILE_H

#include <stdint.h>
#include "constant_pool.h"

// Definição genérica interna de cada item do Pool
typedef struct {
    uint8_t tag;
    uint8_t info[];  
} cp_info;

// Estrutura do Atributo
typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t *info;
} attribute_info;

// Estrutura de um Field (Variável de Classe)
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} field_info;

// Estrutura de um Field (Variável de Classe)
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} method_info;

// Estrutura principal do arquivo .class
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info **constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t *interfaces;
    uint16_t fields_count;
    field_info *fields;
    uint16_t methods_count;
    method_info *methods;

    // As structs de fields, methods e attributes serão adicionadas aqui
} ClassFile;

#endif