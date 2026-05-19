#ifndef CLASS_FILE_H
#define CLASS_FILE_H

#include <stdint.h>
#include "constant_pool.h"

// Definição genérica interna de cada item do Pool
typedef struct {
    uint8_t tag;
    uint8_t info[];  
} cp_info;

// Estrutura principal do arquivo .class
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info **constant_pool;
    // As structs de fields, methods e attributes serão adicionadas aqui
} ClassFile;

#endif