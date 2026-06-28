/**
 * @file java_types.h
 * @brief Representação em memória dos tipos Java: arrays e objetos.
 *
 * Define as estruturas lógicas utilizadas em tempo de execução para representar
 * arrays e objetos Java alocados na heap simulada, além das funções de alocação
 * correspondentes. Arrays de tipos de 64 bits (long, double) alocam o dobro
 * de slots para acomodar os dois words de 32 bits de cada elemento.
 */

#ifndef JAVA_TYPES_H
#define JAVA_TYPES_H

#include <stdint.h>
#include "class_file.h"

/**
 * @brief Códigos de tipo de elemento para arrays primitivos.
 *
 * Correspondem aos valores do operando do opcode `newarray` definidos
 * pela Java Virtual Machine Specification. O valor T_REFERENCE (12) é
 * uma extensão desta implementação para arrays de referência (`anewarray`).
 */
typedef enum {
    T_BOOLEAN  = 4,  /**< Array de boolean (1 bit lógico, armazenado em byte). */
    T_CHAR     = 5,  /**< Array de char (16 bits sem sinal). */
    T_FLOAT    = 6,  /**< Array de float (IEEE 754 32 bits). */
    T_DOUBLE   = 7,  /**< Array de double (IEEE 754 64 bits, 2 slots por elemento). */
    T_BYTE     = 8,  /**< Array de byte (8 bits com sinal). */
    T_SHORT    = 9,  /**< Array de short (16 bits com sinal). */
    T_INT      = 10, /**< Array de int (32 bits com sinal). */
    T_LONG     = 11, /**< Array de long (64 bits com sinal, 2 slots por elemento). */
    T_REFERENCE = 12 /**< Array de referências a objetos ou outros arrays. */
} ArrayType;

/**
 * @brief Estrutura lógica de um array Java em tempo de execução.
 *
 * Representa um array alocado dinamicamente. Para tipos de 64 bits
 * (T_LONG e T_DOUBLE), o array @p elements contém o dobro de slots
 * de 32 bits, pois cada elemento ocupa dois slots consecutivos
 * (big-endian: índice par = high word, índice ímpar = low word).
 * Para T_REFERENCE, cada elemento de @p elements armazena um ponteiro
 * de 32 bits (ou índice) para o objeto referenciado.
 */
typedef struct {
    ArrayType type;    /**< Tipo dos elementos do array (determina o layout de @p elements). */
    int32_t length;    /**< Número lógico de elementos do array (conforme visto em Java). */
    int32_t *elements; /**< Bloco contíguo de slots de 32 bits que armazena os elementos. */
} Array;

/**
 * @brief Estrutura lógica de um objeto Java em tempo de execução.
 *
 * Representa uma instância de classe alocada dinamicamente. O vetor
 * @p fields armazena os campos de instância (não estáticos) na ordem
 * de declaração da classe, usando slots de 32 bits. Campos do tipo
 * long e double ocupam dois slots consecutivos.
 */
typedef struct {
    ClassFile *class_ref; /**< Ponteiro para o ClassFile da classe instanciada, na Method Area. */
    int32_t *fields;      /**< Vetor de slots de 32 bits com os valores dos campos de instância. */
} Object;

/**
 * @brief Aloca e inicializa um novo array do tipo e comprimento especificados.
 *
 * Para T_LONG e T_DOUBLE, aloca @p length * 2 slots de 32 bits.
 * Para os demais tipos, aloca @p length slots. Todos os elementos
 * são inicializados com zero.
 *
 * @param length Número lógico de elementos do array (deve ser >= 0).
 * @param type   Tipo dos elementos, conforme o enum ArrayType.
 * @return Ponteiro para o Array alocado, ou NULL em falha de alocação.
 */
Array* create_new_array(int32_t length, ArrayType type);

/**
 * @brief Aloca e inicializa um novo objeto da classe especificada.
 *
 * Conta os campos de instância de @p class_ref via count_object_fields(),
 * aloca o vetor @p fields com o número de slots necessário (considerando
 * que long e double ocupam 2 slots) e inicializa todos com zero.
 *
 * @param class_ref Ponteiro para o ClassFile da classe a instanciar.
 * @return Ponteiro para o Object alocado, ou NULL em falha de alocação.
 */
Object* create_new_object(ClassFile *class_ref);

/**
 * @brief Conta o número de slots de 32 bits necessários para os campos de instância de uma classe.
 *
 * Percorre os campos de @p class_ref ignorando os campos estáticos (ACC_STATIC).
 * Para cada campo de instância, soma 1 slot para tipos de 32 bits e 2 slots
 * para long ('J') e double ('D'), conforme o primeiro caractere do descritor.
 *
 * @param class_ref Ponteiro para o ClassFile cuja contagem de slots será calculada.
 * @return Total de slots de 32 bits necessários para armazenar todos os campos de instância.
 */
uint16_t count_object_fields(ClassFile *class_ref);

#endif // JAVA_TYPES_H
