#include <stdlib.h>
#include <stdio.h>
#include "java_types.h"

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

    // Define o tamanho e o tipo
    arr->length = length;
    arr->type = type;

    // Aloca o vetor de elementos zerados via calloc 
    if (length > 0) {
        arr->elements = (int32_t*) calloc(length, sizeof(int32_t));
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
            total_instance_fields++;
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

    // Atribuir a referência apontando para a classe na Method Area
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