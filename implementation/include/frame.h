/**
 * @file frame.h
 * @brief Estrutura e funções do Frame de execução da JVM.
 *
 * Define o Frame — a unidade de execução empilhada na JVMStack a cada invocação
 * de método — e todas as funções para criação, destruição, acesso às variáveis
 * locais, manipulação da pilha de operandos e gerenciamento do ciclo de vida
 * de chamada e retorno de métodos.
 *
 * Cada slot do vetor de variáveis locais e da pilha de operandos possui 32 bits.
 * Valores do tipo `long` e `double` ocupam 2 slots consecutivos (big-endian: o slot
 * de índice mais baixo guarda os 32 bits mais significativos).
 * A representação interna é `uint32_t` (bits crus); funções auxiliares fazem
 * a conversão de/para `int32_t`, `float`, `int64_t` e `double` via `memcpy`.
 */

#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "class_file.h"
#include "attributes.h"
#include "jvm_stack.h"

/**
 * @brief Frame de execução da JVM.
 *
 * Criado a cada invocação de método e destruído ao retornar. Contém o vetor
 * de variáveis locais, a pilha de operandos, uma referência à constant pool
 * da classe corrente, o bytecode do método e o program counter local.
 * Frames são encadeados via @p previous_frame formando a call stack.
 */
typedef struct Frame {
    uint32_t *local_vars;          /**< Vetor de variáveis locais (tamanho dinâmico: max_locals slots de 32 bits). */
    uint16_t max_locals;           /**< Número máximo de slots de variáveis locais, conforme o atributo Code. */

    uint32_t *operand_stack;       /**< Pilha de operandos (tamanho dinâmico: max_stack slots de 32 bits), slot 0 = base. */
    uint16_t max_stack;            /**< Profundidade máxima da pilha de operandos, conforme o atributo Code. */
    uint16_t sp;                   /**< Stack pointer: índice do próximo slot livre (topo = sp-1). */

    cp_info **constant_pool;       /**< Referência para a constant pool da classe do método corrente. */
    uint16_t constant_pool_count;  /**< Número de entradas na constant pool referenciada. */

    uint8_t *code;                 /**< Array de bytes do bytecode do método corrente. */
    uint32_t code_length;          /**< Tamanho em bytes do array de bytecode. */
    uint32_t pc;                   /**< Program counter local: offset do próximo opcode a executar dentro de @p code. */

    struct Frame *previous_frame;  /**< Frame do método chamador, ou NULL se este for o frame inicial. */
} Frame;


// ---------------------------------------------------------------------------------------
// Funções de criação/destruição
// ---------------------------------------------------------------------------------------

/**
 * @brief Cria um novo Frame a partir do atributo Code de um método.
 *
 * Aloca o vetor de variáveis locais e a pilha de operandos com os tamanhos
 * definidos por `max_locals` e `max_stack` do atributo Code. O PC é
 * inicializado em 0 e o SP em 0 (pilha vazia).
 *
 * @param code_attr           Atributo Code do método, contendo max_stack, max_locals e bytecode.
 * @param constant_pool       Referência à constant pool da classe do método.
 * @param constant_pool_count Número de entradas da constant pool.
 * @return Ponteiro para o Frame alocado, ou NULL em falha de alocação.
 */
Frame* frame_create(Code_attribute *code_attr, cp_info **constant_pool, uint16_t constant_pool_count);

/**
 * @brief Libera toda a memória associada a um Frame.
 *
 * Desaloca o vetor de variáveis locais, a pilha de operandos e a própria struct.
 * Não destrói o @p previous_frame — a responsabilidade é da JVMStack.
 *
 * @param frame Ponteiro para o Frame a ser destruído.
 */
void frame_destroy(Frame *frame);


// ---------------------------------------------------------------------------------------
// Funções de acesso ao vetor de Variáveis Locais (slots de 32 bits)
// ---------------------------------------------------------------------------------------

/**
 * @brief Armazena um valor inteiro de 32 bits em uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @param value Valor inteiro a armazenar.
 */
void frame_set_local_int(Frame *frame, uint16_t index, int32_t value);

/**
 * @brief Lê um valor inteiro de 32 bits de uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @return Valor inteiro armazenado no slot.
 */
int32_t frame_get_local_int(Frame *frame, uint16_t index);

/**
 * @brief Armazena um valor float de 32 bits em uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @param value Valor float a armazenar.
 */
void frame_set_local_float(Frame *frame, uint16_t index, float value);

/**
 * @brief Lê um valor float de 32 bits de uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @return Valor float armazenado no slot.
 */
float frame_get_local_float(Frame *frame, uint16_t index);

/**
 * @brief Armazena um valor long de 64 bits em duas variáveis locais consecutivas.
 *
 * Ocupa os slots @p index e @p index+1 em big-endian:
 * @p index armazena os 32 bits mais significativos.
 *
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do primeiro slot (mais significativo).
 * @param value Valor long a armazenar.
 */
void frame_set_local_long(Frame *frame, uint16_t index, int64_t value);

/**
 * @brief Lê um valor long de 64 bits de duas variáveis locais consecutivas.
 *
 * Reconstrói o valor a partir dos slots @p index (high) e @p index+1 (low).
 *
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do primeiro slot (mais significativo).
 * @return Valor long reconstituído.
 */
int64_t frame_get_local_long(Frame *frame, uint16_t index);

/**
 * @brief Armazena um valor double de 64 bits em duas variáveis locais consecutivas.
 *
 * Ocupa os slots @p index e @p index+1 em big-endian:
 * @p index armazena os 32 bits mais significativos.
 *
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do primeiro slot (mais significativo).
 * @param value Valor double a armazenar.
 */
void frame_set_local_double(Frame *frame, uint16_t index, double value);

/**
 * @brief Lê um valor double de 64 bits de duas variáveis locais consecutivas.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do primeiro slot (mais significativo).
 * @return Valor double reconstituído.
 */
double frame_get_local_double(Frame *frame, uint16_t index);

/**
 * @brief Armazena uma referência de 32 bits (objeto ou array) em uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @param ref   Referência (endereço ou índice) de 32 bits a armazenar.
 */
void frame_set_local_ref(Frame *frame, uint16_t index, uint32_t ref);

/**
 * @brief Lê uma referência de 32 bits (objeto ou array) de uma variável local.
 * @param frame Ponteiro para o Frame corrente.
 * @param index Índice do slot no vetor de variáveis locais.
 * @return Referência de 32 bits armazenada no slot.
 */
uint32_t frame_get_local_ref(Frame *frame, uint16_t index);


// ---------------------------------------------------------------------------------------
// Funções de manipulação da Pilha de Operandos
// ---------------------------------------------------------------------------------------

/**
 * @brief Empilha um valor inteiro de 32 bits na pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @param value Valor inteiro a empilhar.
 */
void frame_push_int(Frame *frame, int32_t value);

/**
 * @brief Desempilha e retorna um valor inteiro de 32 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Valor inteiro desempilhado.
 */
int32_t frame_pop_int(Frame *frame);

/**
 * @brief Empilha um valor float de 32 bits na pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @param value Valor float a empilhar.
 */
void frame_push_float(Frame *frame, float value);

/**
 * @brief Desempilha e retorna um valor float de 32 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Valor float desempilhado.
 */
float frame_pop_float(Frame *frame);

/**
 * @brief Empilha um valor long de 64 bits na pilha de operandos (ocupa 2 slots).
 *
 * O slot de índice mais baixo (empilhado primeiro) recebe os 32 bits mais significativos.
 *
 * @param frame Ponteiro para o Frame corrente.
 * @param value Valor long a empilhar.
 */
void frame_push_long(Frame *frame, int64_t value);

/**
 * @brief Desempilha e retorna um valor long de 64 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Valor long desempilhado.
 */
int64_t frame_pop_long(Frame *frame);

/**
 * @brief Empilha um valor double de 64 bits na pilha de operandos (ocupa 2 slots).
 * @param frame Ponteiro para o Frame corrente.
 * @param value Valor double a empilhar.
 */
void frame_push_double(Frame *frame, double value);

/**
 * @brief Desempilha e retorna um valor double de 64 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Valor double desempilhado.
 */
double frame_pop_double(Frame *frame);

/**
 * @brief Empilha uma referência de 32 bits (objeto ou array) na pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @param ref   Referência de 32 bits a empilhar.
 */
void frame_push_ref(Frame *frame, uint32_t ref);

/**
 * @brief Desempilha e retorna uma referência de 32 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Referência de 32 bits desempilhada.
 */
uint32_t frame_pop_ref(Frame *frame);

/**
 * @brief Empilha um slot "cru" de 32 bits na pilha de operandos sem interpretação de tipo.
 *
 * Usado pelas instruções de manipulação de pilha (`pop`, `dup`, `swap`) que
 * operam sobre bits brutos independentemente do tipo do dado.
 *
 * @param frame Ponteiro para o Frame corrente.
 * @param raw   Valor bruto de 32 bits a empilhar.
 */
void frame_push_raw(Frame *frame, uint32_t raw);

/**
 * @brief Desempilha e retorna um slot "cru" de 32 bits da pilha de operandos.
 * @param frame Ponteiro para o Frame corrente.
 * @return Valor bruto de 32 bits desempilhado.
 */
uint32_t frame_pop_raw(Frame *frame);


// ---------------------------------------------------------------------------------------
// Funções de Ciclo de Vida do Frame: Push/Pop em chamadas de método
// ---------------------------------------------------------------------------------------

/**
 * @brief Tipos de retorno de método.
 *
 * Usado por frame_pop_method() para determinar quantos slots copiar
 * e como interpretar o valor de retorno ao restaurar o frame chamador.
 */
typedef enum {
    RETURN_VOID,   /**< Método void (`return`): nenhum valor é empilhado no chamador. */
    RETURN_INT,    /**< `ireturn`: retorno int, boolean, byte, char ou short (1 slot). */
    RETURN_FLOAT,  /**< `freturn`: retorno float (1 slot). */
    RETURN_LONG,   /**< `lreturn`: retorno long (2 slots). */
    RETURN_DOUBLE, /**< `dreturn`: retorno double (2 slots). */
    RETURN_REF     /**< `areturn`: retorno de referência a objeto ou array (1 slot). */
} ReturnType;

/**
 * @brief Conta o número de slots de 32 bits ocupados pelos argumentos de um método.
 *
 * Percorre o descritor de método (e.g. `"(ILjava/lang/String;D)V"`) e soma
 * os slots de cada parâmetro: primitivos de 32 bits = 1 slot, `long`/`double` = 2 slots,
 * tipos referência = 1 slot. O tipo de retorno (após `)`) é ignorado.
 *
 * @param method_descriptor  String do descritor de método lida da constant pool.
 * @param is_instance_method 1 para métodos de instância (adiciona +1 slot para `this`), 0 para estáticos.
 * @return Número total de slots de variáveis locais necessários para os argumentos.
 */
uint16_t descriptor_count_arg_slots(const char *method_descriptor, int is_instance_method);

/**
 * @brief Cria e empilha um novo Frame na JVMStack para invocar um método.
 *
 * Aloca o novo Frame com base no atributo Code, transfere os @p num_arg_slots
 * argumentos do topo da pilha de operandos do frame chamador para as variáveis
 * locais do novo frame (índices 0..num_arg_slots-1, preservando a ordem),
 * empilha o novo frame e atualiza o PC.
 *
 * @param stack               Ponteiro para a JVMStack.
 * @param code_attr           Atributo Code do método a ser invocado.
 * @param constant_pool       Constant pool da classe do método.
 * @param constant_pool_count Número de entradas da constant pool.
 * @param num_arg_slots       Número de slots de argumentos a transferir do chamador.
 * @return Status da operação: JVM_STACK_OK, JVM_STACK_OVERFLOW_ERROR ou JVM_STACK_OUT_OF_MEMORY.
 */
JVMStackStatus frame_push_method(JVMStack *stack, Code_attribute *code_attr,
                                  cp_info **constant_pool, uint16_t constant_pool_count,
                                  uint16_t num_arg_slots);

/**
 * @brief Realiza o término normal de um método e restaura o frame chamador.
 *
 * Lê o valor de retorno do topo da pilha de operandos do frame atual (se
 * @p return_type != RETURN_VOID), desempilha e libera o frame atual da JVMStack,
 * e empilha o valor de retorno na pilha de operandos do frame chamador restaurado.
 *
 * @param stack       Ponteiro para a JVMStack.
 * @param return_type Tipo do valor de retorno, que determina quantos slots copiar.
 * @return 1 se ainda restar um frame corrente (execução continua no chamador),
 *         0 se a pilha ficou vazia (método finalizado era o inicial).
 */
int frame_pop_method(JVMStack *stack, ReturnType return_type);

#endif
