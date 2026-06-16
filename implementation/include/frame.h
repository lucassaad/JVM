#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "class_file.h"
#include "attributes.h"
#include "jvm_stack.h"

// ---------------------------------------------------------------------------------------
// Estrutura do Frame
//
// Cada slot do vetor de Variáveis Locais e da Pilha de Operandos possui 32 bits.
// Valores do tipo long e double ocupam 2 slots consecutivos (big-endian: o slot
// de índice mais baixo guarda os 32 bits mais significativos).
// Usamos uint32_t como representação "crua" dos bits (sem sinal); funções auxiliares fazem
// a conversão de/para int32_t, float, int64_t e double.

typedef struct Frame {
    uint32_t *local_vars;         // Ponteiro para Vetor de Variáveis Locais (tamanho dinâmico)
    uint16_t max_locals;

    uint32_t *operand_stack;     // Ponteiro para Pilha de Operandos (tamanho dinâmico), slot 0 = base
    uint16_t max_stack;
    uint16_t sp;                 // Stack pointer: índice do próximo slot LIVRE (topo = sp-1) da Pilha de Operandos

    cp_info **constant_pool;     // Referência para a Constant Pool da classe do método corrente
    uint16_t constant_pool_count;

    uint8_t *code;               // Ponteiro para o array com os opcodes do método corrente (código bytecode)
    uint32_t code_length;
    uint32_t pc;                 // Program Counter local ao frame (offset dentro de 'code')

    struct Frame *previous_frame; // Frame anterior (chamador), NULL se for o frame inicial
} Frame;


// Funções de criação/destruição

// Cria um novo Frame a partir do atributo Code de um método, alocando o vetor
// de variáveis locais e a pilha de operandos com os tamanhos de max_locals/max_stack (presentes no atributo Code).
Frame* frame_create(Code_attribute *code_attr, cp_info **constant_pool, uint16_t constant_pool_count);

// Libera toda a memória associada a um Frame (vetores internos + struct).
void frame_destroy(Frame *frame);


// Funções de acesso ao vetor de Variáveis Locais (slots de 32 bits)

// Funções para números inteiros (32 bits)
void frame_set_local_int(Frame *frame, uint16_t index, int32_t value);
int32_t frame_get_local_int(Frame *frame, uint16_t index);

// Funções para float (32 bits, mas aceita números com vírgula com precisão simples)
void frame_set_local_float(Frame *frame, uint16_t index, float value);
float frame_get_local_float(Frame *frame, uint16_t index);

// long/double ocupam 2 slots (64 bits): [index] e [index+1]
// Funções para long (números inteiros de 64 bits)
void frame_set_local_long(Frame *frame, uint16_t index, int64_t value);
int64_t frame_get_local_long(Frame *frame, uint16_t index);

// Funções para double (64 bits, mas aceita números com vírgula com precisão dupla)
void frame_set_local_double(Frame *frame, uint16_t index, double value);
double frame_get_local_double(Frame *frame, uint16_t index);

// Caso de alocar Objetos e Arrays (a ref é de 32 bits)
void frame_set_local_ref(Frame *frame, uint16_t index, uint32_t ref);
uint32_t frame_get_local_ref(Frame *frame, uint16_t index);


// Funções de manipulação da Pilha de Operandos (categoria 1: 1 slot, categoria 2: 2 slots)

// Funções para números inteiros (32 bits)
void frame_push_int(Frame *frame, int32_t value);
int32_t frame_pop_int(Frame *frame);

// Funções para float (32 bits, mas aceita números com vírgula com precisão simples)
void frame_push_float(Frame *frame, float value);
float frame_pop_float(Frame *frame);

// Funções para long (números inteiros de 64 bits)
void frame_push_long(Frame *frame, int64_t value);
int64_t frame_pop_long(Frame *frame);

// Funções para double (64 bits, mas aceita números com vírgula com precisão dupla)
void frame_push_double(Frame *frame, double value);
double frame_pop_double(Frame *frame);

// Funções para Objetos e Arrays (chamados "referências", com 32 bits)
void frame_push_ref(Frame *frame, uint32_t ref);
uint32_t frame_pop_ref(Frame *frame);

// Push/pop genérico de um único slot "cru" de 32 bits (usado por pop/dup/swap - não olham para o tipo de dado,
// apenas para os bits que o formam)
void frame_push_raw(Frame *frame, uint32_t raw);
uint32_t frame_pop_raw(Frame *frame);

// ---------------------------------------------------------------------------------------
// Funções de Ciclo de Vida do Frame: Push/Pop em chamadas de método
// ---------------------------------------------------------------------------------------

// Tipos de retorno de método, usados por frame_pop_method para saber quantos
// slots (e de que forma) copiar o valor de retorno para a Pilha de Operandos do frame chamador.
typedef enum {
    RETURN_VOID,   // return / método void
    RETURN_INT,    // ireturn (int, boolean, byte, char, short)
    RETURN_FLOAT,  // freturn
    RETURN_LONG,   // lreturn
    RETURN_DOUBLE, // dreturn
    RETURN_REF     // areturn
} ReturnType;

// Calcula o número de slots de 32 bits ocupados pelos argumentos de um método,
// a partir do method_descriptor (string lida da Constant Pool).
// is_instance_method = 1 adiciona +1 slot para o "this" (referência ao próprio objeto) no início.
// Não conta o tipo de retorno (após o ')').
uint16_t descriptor_count_arg_slots(const char *method_descriptor, int is_instance_method);

// Cria um novo Frame, move os argumentos do método do novo frame do topo da Pilha de Operandos do frame chamador
// para o Vetor de Variáveis Locais do novo frame (índices 0..num_arg_slots-1, preservando a
// ordem), empilha o novo frame na JVMStack e atualiza o PC (vai para o novo frame).
// Retorna o status final (StackOverflowError / OutOfMemoryError / OK).
JVMStackStatus frame_push_method(JVMStack *stack, Code_attribute *code_attr,
                                  cp_info **constant_pool, uint16_t constant_pool_count,
                                  uint16_t num_arg_slots);

// Realiza o término normal de um método:
//  - lê o valor de retorno (se houver, conforme 'return_type') do topo da
//    Pilha de Operandos do frame atual;
//  - desempilha (pop) o frame atual da JVMStack e libera sua memória;
//  - empilha o valor de retorno na Pilha de Operandos do novo frame corrente
//    (frame chamador), se 'return_type' é diferente de RETURN_VOID e se ainda existir um
//    frame chamador.
// Retorna 1 se ainda restar um frame corrente (execução continua no chamador),
// ou 0 se a pilha ficou vazia (o método finalizado era o último/inicial).
int frame_pop_method(JVMStack *stack, ReturnType return_type);

#endif