/**
 * @file jvm_stack.h
 * @brief Pilha da JVM (JVM Stack), registro PC e funções de gerenciamento de frames.
 *
 * A especificação JVM define que cada thread possui sua própria JVM Stack,
 * composta por Frames empilhados a cada invocação de método. Como esta
 * implementação é single-thread, a pilha e o registro PC são representados
 * como contexto global único.
 *
 * O registro PC (@p pc_register) é uma variável global sincronizada com o
 * campo @p pc do frame corrente pelo interpretador a cada push/pop.
 */

#ifndef JVM_STACK_H
#define JVM_STACK_H

#include <stdint.h>

/**
 * @brief Declaração antecipada de Frame para evitar dependência circular com frame.h.
 */
typedef struct Frame Frame;

/**
 * @brief Pilha de execução da JVM.
 *
 * Mantém o frame corrente (topo da pilha), o consumo de memória atual e o
 * limite máximo configurado. Frames são encadeados via o campo @p previous_frame
 * da struct Frame, formando uma lista ligada implícita.
 */
typedef struct JVMStack {
    Frame *current_frame;        /**< Ponteiro para o Frame no topo da pilha (frame em execução). */
    uint32_t current_size_bytes; /**< Tamanho total atual da pilha em bytes (soma dos frames empilhados). */
    uint32_t max_size_bytes;     /**< Limite máximo de memória da pilha em bytes (equivalente à flag -Xss). */
    int frame_count;             /**< Número de frames atualmente empilhados. */
} JVMStack;

/**
 * @brief Códigos de status retornados pelas operações da JVMStack.
 */
typedef enum {
    JVM_STACK_OK = 0,           /**< Operação concluída com sucesso. */
    JVM_STACK_OVERFLOW_ERROR,   /**< Limite de frames excedido — equivalente a java.lang.StackOverflowError. */
    JVM_STACK_OUT_OF_MEMORY,    /**< Memória insuficiente para alocar o frame — equivalente a java.lang.OutOfMemoryError. */
    JVM_STACK_INVALID_STATE     /**< Erro genérico de estado inválido (e.g. pop em pilha vazia). */
} JVMStackStatus;

/**
 * @brief Registro PC (Program Counter) global.
 *
 * Sempre aponta para o offset, dentro do bytecode do método corrente,
 * da próxima instrução a ser buscada pelo interpretador. É sincronizado
 * com o campo @p pc do frame corrente a cada push e pop de frame.
 */
extern uint32_t pc_register;

// ---------------------------------------------------------------------------------------
// Funções principais
// ---------------------------------------------------------------------------------------

/**
 * @brief Inicializa a JVM Stack com o limite de memória especificado.
 *
 * Define @p current_frame como NULL, zera os contadores e configura
 * @p max_size_bytes. Deve ser chamada uma única vez antes de qualquer push.
 *
 * @param stack          Ponteiro para a JVMStack a inicializar.
 * @param max_size_bytes Tamanho máximo da pilha em bytes (e.g. 1 * 1024 * 1024 para 1 MB).
 */
void jvm_stack_init(JVMStack *stack, uint32_t max_size_bytes);

/**
 * @brief Empilha um Frame já alocado no topo da JVM Stack.
 *
 * Verifica se o acréscimo de @p frame_size_bytes ultrapassa @p max_size_bytes
 * antes de empilhar. Em caso de sucesso, encadeia o frame ao @p current_frame
 * anterior via @p previous_frame, atualiza @p current_frame e incrementa
 * @p current_size_bytes e @p frame_count.
 *
 * @param stack            Ponteiro para a JVMStack.
 * @param frame            Ponteiro para o Frame alocado por frame_create() a ser empilhado.
 * @param frame_size_bytes Tamanho em bytes do frame, calculado por jvm_stack_frame_size().
 * @return JVM_STACK_OK em sucesso, JVM_STACK_OVERFLOW_ERROR ou JVM_STACK_OUT_OF_MEMORY em erro.
 */
JVMStackStatus jvm_stack_push(JVMStack *stack, Frame *frame, uint32_t frame_size_bytes);

/**
 * @brief Desempilha o Frame corrente e restaura o frame anterior.
 *
 * Remove o frame do topo da pilha, restaura @p current_frame para
 * @p previous_frame, decrementa @p current_size_bytes e @p frame_count.
 * O frame removido NÃO é destruído aqui — a responsabilidade de chamar
 * frame_destroy() é de quem recebe o ponteiro retornado.
 *
 * @param stack            Ponteiro para a JVMStack.
 * @param frame_size_bytes Tamanho em bytes do frame sendo removido.
 * @return Ponteiro para o Frame removido, ou NULL se a pilha estava vazia.
 */
Frame* jvm_stack_pop(JVMStack *stack, uint32_t frame_size_bytes);

/**
 * @brief Verifica se a JVM Stack está vazia.
 *
 * @param stack Ponteiro para a JVMStack.
 * @return 1 se não há frame corrente (pilha vazia), 0 caso contrário.
 */
int jvm_stack_is_empty(JVMStack *stack);

/**
 * @brief Calcula o tamanho em bytes ocupado por um Frame com os parâmetros dados.
 *
 * O tamanho é a soma da struct Frame com os arrays de variáveis locais
 * (@p max_locals slots) e pilha de operandos (@p max_stack slots), cada
 * slot com 4 bytes (uint32_t). Usado por jvm_stack_push() e jvm_stack_pop()
 * para manter @p current_size_bytes consistente.
 *
 * @param max_locals Número de slots de variáveis locais do frame.
 * @param max_stack  Profundidade máxima da pilha de operandos do frame.
 * @return Tamanho total em bytes do frame.
 */
uint32_t jvm_stack_frame_size(uint16_t max_locals, uint16_t max_stack);

#endif
