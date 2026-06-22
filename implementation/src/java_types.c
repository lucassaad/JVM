#include <stdlib.h>
#include <stdio.h>
#include "java_types.h"
#include "constant_pool.h"

Array* create_new_array(int32_t length, ArrayType type) {
    if (length < 0) {
        fprintf(stderr, "NegativeArraySizeException: Tentativa de instanciar array com tamanho negativo (%d).\n", length);
        return NULL;
    }

    // Aloca o cabeçalho (struct Array) via malloc 
    Array* arr = (Array*) malloc(sizeof(Array));
    if (arr == NULL) {
        fprintf(stderr, "OutOfMemoryError: Falha ao alocar cabecalho do Array na memoria nativa.\n");
        return NULL;
    }

    arr->length = length;
    arr->type = type;

    // Se for tipo de 64 bits (long ou double), dobra a capacidade lógica a ser alocada
    int32_t alloc_length = length;
    if (type == T_LONG || type == T_DOUBLE) {
        alloc_length = length * 2;
    }

    // Aloca o vetor de elementos zerados via calloc 
    if (alloc_length > 0) {
        arr->elements = (int32_t*) calloc(alloc_length, sizeof(int32_t));
        if (arr->elements == NULL) {
            fprintf(stderr, "OutOfMemoryError: Falha ao alocar elementos do Array.\n");
            free(arr);
            return NULL;
        }
    } else {
        arr->elements = NULL; 
    }

    return arr;
}

// Rotina para contabilizar a quantidade total de atributos da classe 
uint16_t count_object_fields(ClassFile *class_ref) {
    if (class_ref == NULL) return 0;
    
    uint16_t total_instance_fields = 0;
    
    // Varre o vetor de fields carregado na Method Area
    for (int i = 0; i < class_ref->fields_count; i++) {
        // A flag 0x0008 (ACC_STATIC) define se um field é da classe (static) ou do objeto (instância)
        if ((class_ref->fields[i].access_flags & 0x0008) == 0) {
            
            // Busca o índice do descritor na Constant Pool
            uint16_t desc_idx = class_ref->fields[i].descriptor_index;
            cp_info *cp = class_ref->constant_pool[desc_idx];
            CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)cp->info;
            
            // Verifica a primeira letra do descritor
            char first_char = (char)utf8->bytes[0];
            
            // Se for 'J' (long) ou 'D' (double), ocupa 2 slots de instância
            if (first_char == 'J' || first_char == 'D') {
                total_instance_fields += 2;
            } else {
                total_instance_fields += 1;
            }
        }
    }

    return total_instance_fields;
}

Object* create_new_object(ClassFile *class_ref) {
    if (class_ref == NULL) {
        fprintf(stderr, "Erro Interno: Tentativa de instanciar objeto de uma classe nula.\n");
        return NULL;
    }

    // Consultar metadados para saber a quantidade de campos da classe 
    uint16_t M_fields = count_object_fields(class_ref);

    // Usar malloc para alocar a estrutura do objeto 
    Object* obj = (Object*) malloc(sizeof(Object));
    if (obj == NULL) {
        fprintf(stderr, "OutOfMemoryError: Falha ao alocar objeto da classe na memoria nativa.\n");
        return NULL;
    }

    obj->class_ref = class_ref;

    // Usar calloc para o vetor fields (garante inicialização com zeros/null) 
    if (M_fields > 0) {
        obj->fields = (int32_t*) calloc(M_fields, sizeof(int32_t));
        if (obj->fields == NULL) {
            fprintf(stderr, "OutOfMemoryError: Falha ao alocar o vetor de atributos do Objeto.\n");
            free(obj);
            return NULL;
        }
    } else {
        obj->fields = NULL;
    }

    return obj;
}