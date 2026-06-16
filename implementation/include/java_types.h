#ifndef JAVA_TYPES_H
#define JAVA_TYPES_H

#include <stdint.h>
#include "class_file.h" 

typedef enum {
    T_BOOLEAN = 4,
    T_CHAR    = 5,
    T_FLOAT   = 6,
    T_DOUBLE  = 7,
    T_BYTE    = 8,
    T_SHORT   = 9,
    T_INT     = 10,
    T_LONG    = 11,
    T_REFERENCE = 12 
} ArrayType;

// Estrutura Lógica de Arrays
typedef struct {
    ArrayType type;      // Armazena o tipo do array
    int32_t length;      // Armazena o tamanho do array
    int32_t *elements;   // Ponteiro para um bloco contíguo de elementos
} Array;

// Estrutura Lógica de Objetos
typedef struct {
    ClassFile *class_ref; // Ponteiro para a classe correspondente na Área de Métodos
    int32_t *fields;      // Ponteiro para o vetor com as variáveis de instância
} Object;


// Funções de alocação
Array* create_new_array(int32_t length, ArrayType type);
Object* create_new_object(ClassFile *class_ref);
uint16_t count_object_fields(ClassFile *class_ref);

#endif // JAVA_TYPES_H