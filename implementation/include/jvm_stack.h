#ifndef JVM_STACK_H
#define JVM_STACK_H

#include <stdint.h>

// Declaração da struct Frame para evitar dependência circular com frame.h
typedef struct Frame Frame;

// ---------------------------------------------------------------------------------------
// Pilha da JVM (JVM Stack) e Registro PC
//
// A especificação define que cada thread possui sua própria JVM Stack (composta
// por Frames) e seu próprio registro PC. Como esta implementação é single-thread,
// representamos a Pilha da JVM como uma struct global de contexto.
// ---------------------------------------------------------------------------------------

typedef struct JVMStack {
    Frame *current_frame;       // Ponteiro para o Frame no topo da pilha (frame corrente)

    uint32_t current_size_bytes; // Tamanho atual da Pilha da JVM, em bytes
    uint32_t max_size_bytes;      // Limite máximo de memória da pilha (-Xss), em bytes

    int frame_count;              // Número de frames atualmente na pilha
} JVMStack;

// Códigos de erro retornados pelas operações da pilha
typedef enum {
    JVM_STACK_OK = 0,

    // Acesso à memória não autorizada (overflow)
    JVM_STACK_OVERFLOW_ERROR,   // Equivalente a java.lang.StackOverflowError

    // Pilha da JVM não tem espaço suficiente para alocar determinado frame 
    JVM_STACK_OUT_OF_MEMORY,     // Equivalente a java.lang.OutOfMemoryError

    // Caso para outros tipos de erros
    JVM_STACK_INVALID_STATE
} JVMStackStatus;

// ---------------------------------------------------------------------------------------
// Registro PC (Program Counter)
//
// Aqui representado como variável global de contexto. Sempre aponta para o
// offset (dentro do bytecode do método corrente) da próxima instrução a ser
// executada pelo interpretador.
extern uint32_t pc_register;

// ---------------------------------------------------------------------------------------
// Funções principais

// Inicializa a Pilha da JVM.
void jvm_stack_init(JVMStack *stack, uint32_t max_size_bytes);

// Empilha um Frame já alocado (por frame_create) no topo da pilha.
// Recebe o tamanho do frame para cálculo e verificação do tamanho atual da Pilha da JVM.
JVMStackStatus jvm_stack_push(JVMStack *stack, Frame *frame, uint32_t frame_size_bytes);

// Desempilha (remove) o Frame corrente, restaurando o frame anterior como
// current_frame.
// Retorna o ponteiro para o Frame removido, ou NULL se a pilha estava vazia.
Frame* jvm_stack_pop(JVMStack *stack, uint32_t frame_size_bytes);

// Retorna 1 se a pilha estiver vazia (sem frame corrente), 0 caso contrário.
int jvm_stack_is_empty(JVMStack *stack);

// Calcula o tamanho em bytes ocupado por um Frame com os parâmetros dados,
// usado tanto no push quanto no pop para manter current_size_bytes consistente.
uint32_t jvm_stack_frame_size(uint16_t max_locals, uint16_t max_stack);

#endif