/**
 * @file jvm_stack.c
 * @brief Implementação da pilha de frames da JVM (lista encadeada simples).
 *
 * A JVMStack é uma lista encadeada de frames linkados pelo campo
 * frame->previous_frame. O frame do topo é sempre stack->current_frame.
 *
 * Gerenciamento do pc_register global:
 * - jvm_stack_push() atualiza pc_register para frame->pc do novo frame (sempre 0).
 * - jvm_stack_pop() restaura pc_register para frame->pc do frame chamador, ou 0
 *   se a pilha ficou vazia.
 * - O interpretador sincroniza pc_register novamente ao final de cada ciclo,
 *   garantindo coerência após instruções que alteram o PC (desvios, invocações).
 *
 * Cálculo de tamanho (jvm_stack_frame_size):
 * - Estimativa conservadora: sizeof(Frame) + max_locals*4 + max_stack*4 bytes.
 * - Usado por push/pop para rastrear current_size_bytes e detectar overflow.
 * - O limite max_size_bytes é configurável; 0 desabilita a verificação.
 *
 * jvm_stack_pop() NÃO libera o frame removido — retorna o ponteiro para que
 * frame_pop_method() possa copiar o valor de retorno para o frame chamador
 * antes de chamar frame_destroy().
 */

#include <stdio.h>
#include <stdlib.h>
#include "jvm_stack.h"
#include "frame.h"

// Registro PC global (single-thread)
// Reflete qual linha de código deve ser executada naquele frame corrente
uint32_t pc_register = 0;

// Inicialização da Pilha da JVM
void jvm_stack_init(JVMStack *stack, uint32_t max_size_bytes) {
    stack->current_frame = NULL;
    stack->current_size_bytes = 0;
    stack->max_size_bytes = max_size_bytes;
    stack->frame_count = 0;
    pc_register = 0;
}

// Cálculo do tamanho do frame que será empilhado (push) ou desempilhado (pop)
// para acompanhar o tamanho atual da Pilha da JVM
uint32_t jvm_stack_frame_size(uint16_t max_locals, uint16_t max_stack) {
    // Tamanho aproximado: struct Frame + vetores de slots de 32 bits.
    return (uint32_t)sizeof(Frame)
         + (uint32_t)max_locals * sizeof(uint32_t)
         + (uint32_t)max_stack  * sizeof(uint32_t);
}

// Empilha o frame criado pela função create_frame (retorna o ponteiro do frame) na Pilha da JVM
JVMStackStatus jvm_stack_push(JVMStack *stack, Frame *frame, uint32_t frame_size_bytes) {
    if (frame == NULL) {
        // frame_create já deve ter reportado a falha de malloc (OutOfMemoryError)
        return JVM_STACK_OUT_OF_MEMORY;
    }

    // Verifica StackOverflowError, impedindo que o tamanho da pilha ultrapasse max_size_bytes
    if (stack->max_size_bytes > 0 &&
        stack->current_size_bytes + frame_size_bytes > stack->max_size_bytes) {
        return JVM_STACK_OVERFLOW_ERROR;
    }

    // O antigo frame corrente vira o "previous frame" do frame criado (que tornou-se o corrente)
    frame->previous_frame = stack->current_frame;
    // Encadeia o novo frame como o corrente
    stack->current_frame = frame;

    // Atualiza o tamanho da Pilha da JVM e a quantidade de frames nessa pilha
    stack->current_size_bytes += frame_size_bytes;
    stack->frame_count++;

    // O PC passa a se referir ao novo frame corrente, indicando qual é a próxima instrução a ser executada
    pc_register = frame->pc;

    return JVM_STACK_OK;
}

// Desempilha o frame do topo da Pilha da JVM (frame corrente)
Frame* jvm_stack_pop(JVMStack *stack, uint32_t frame_size_bytes) {

    // Se o frame corrente for NULL, a Pilha da JVM está vazia
    if (stack->current_frame == NULL) {
        return NULL;
    }

    // Guarda o ponteiro para o frame a ser desempilhado
    Frame *popped = stack->current_frame;

    stack->current_frame = popped->previous_frame;
    stack->frame_count--;

    if (stack->current_size_bytes >= frame_size_bytes) {
        stack->current_size_bytes -= frame_size_bytes;
    } else {
        stack->current_size_bytes = 0;
    }

    // Restaura o PC para o frame que voltou a ser o corrente (chamador)
    if (stack->current_frame != NULL) {
        pc_register = stack->current_frame->pc;
    } else {
        pc_register = 0;
    }

    // Retorna o ponteiro para o frame que foi desempilhado, caso seja necessário
    // recuperar (empilhar) o valor de retorno dele para o frame chamador
    return popped;
}

// Verifica se a Pilha da JVM é vazia (retorna 1) ou não (retorna 0)
int jvm_stack_is_empty(JVMStack *stack) {
    return stack->current_frame == NULL;
}