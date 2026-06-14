#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "class_file.h"


typedef struct Frame {
    uint32_t *local_variables;      // Array de slots de 32 bits (tamanho dinâmico)
    uint32_t *operand_stack;        // Array (LIFO) de slots de 32 bits (tamanho dinâmico)
    int32_t sp;                     // Stack pointer para o topo da pilha de operandos

    uint16_t max_locals;            // Tamanho limite do vetor de variáveis locais
    uint16_t max_stack;             // Tamanho limite da pilha de operandos

    cp_info **constant_pool;        // Ponteiro para o Constant Pool da classe atual
    struct Frame *previous_frame;   // Ponteiro para o frame chamador
} Frame;

typedef struct JVMStack {
    Frame *current_frame;          // Ponteiro para o frame do método em execução (topo)
    uint32_t max_size_bytes;       // Limite máximo da pilha da JVM em bytes
    uint32_t current_size_bytes;   // Tamanho atual da pilha da JVM em bytes
    uint32_t frame_count;          // Conta quantos frames possui a pilha da JVM
} JVMStack;

// Assinaturas das funções para criar o frame e para liberá-lo
Frame* create_frame(uint16_t max_locals, uint16_t max_stack, cp_info **cp, Frame *previous);
void free_frame(Frame *frame);

// Assinaturas para funções de gerenciamento da pilha da JVM
JVMStack* create_jvm_stack(uint32_t max_size_bytes);
int push_frame(JVMStack *stack, uint16_t max_locals, uint16_t max_stack, cp_info **cp);
void pop_frame(JVMStack *stack);
void free_jvm_stack(JVMStack *stack);

#endif // FRAME_H