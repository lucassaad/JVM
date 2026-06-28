/**
 * @file attributes.h
 * @brief Estruturas que representam os atributos de um arquivo `.class` Java.
 *
 * Define as structs para todos os tipos de atributos suportados: atributos de campo
 * (ConstantValue), de método (Exceptions, Code), de classe (InnerClasses, SourceFile)
 * e sub-atributos de depuração do Code (LineNumberTable, LocalVariableTable).
 */

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <stdint.h>
#include <stdio.h>
#include "class_file.h"

/**
 * @brief Atributo ConstantValue — utilizado exclusivamente em campos estáticos.
 *
 * Associa um valor constante a um campo `static final`. O índice aponta para
 * uma entrada do constant pool do tipo Integer, Float, Long, Double ou String.
 */
typedef struct {
    uint16_t constantvalue_index; /**< Índice no constant pool para o valor constante do campo. */
} ConstantValue_attribute;

/**
 * @brief Atributo Exceptions — utilizado na estrutura de métodos.
 *
 * Lista as exceções verificadas (checked exceptions) que um método pode lançar,
 * declaradas com a cláusula `throws` em Java.
 */
typedef struct {
    uint16_t number_of_exceptions;    /**< Número de exceções declaradas. */
    uint16_t *exception_index_table;  /**< Array de índices no constant pool para entradas CONSTANT_Class de cada exceção. */
} Exceptions_attribute;

/**
 * @brief Entrada de uma classe interna — sub-estrutura do atributo InnerClasses.
 *
 * Descreve o relacionamento entre uma classe interna e sua classe externa,
 * incluindo os modificadores de acesso da classe interna.
 */
typedef struct {
    uint16_t inner_class_info_index;   /**< Índice no constant pool para CONSTANT_Class da classe interna. */
    uint16_t outer_class_info_index;   /**< Índice no constant pool para CONSTANT_Class da classe externa (0 se anônima). */
    uint16_t inner_name_index;         /**< Índice no constant pool para CONSTANT_Utf8 com o nome simples da classe interna (0 se anônima). */
    uint16_t inner_class_access_flags; /**< Máscara de bits com os modificadores de acesso da classe interna. */
} InnerClass_info;

/**
 * @brief Atributo InnerClasses — pertence à estrutura global da Classe.
 *
 * Presente em qualquer classe que referencie ou declare classes internas.
 * Contém uma entrada para cada par (classe interna, classe externa) referenciado.
 */
typedef struct {
    uint16_t number_of_classes; /**< Número de entradas na tabela de classes internas. */
    InnerClass_info *classes;   /**< Array de descrições de classes internas. */
} InnerClasses_attribute;

/**
 * @brief Atributo SourceFile — pertence à estrutura global da Classe.
 *
 * Registra o nome do arquivo fonte `.java` que gerou este `.class`,
 * utilizado em mensagens de erro e stack traces.
 */
typedef struct {
    uint16_t sourcefile_index; /**< Índice no constant pool para CONSTANT_Utf8 com o nome do arquivo fonte (e.g. "Main.java"). */
} SourceFile_attribute;

/**
 * @brief Entrada da tabela de números de linha — sub-atributo do Code (depuração).
 *
 * Mapeia um offset de bytecode para o número de linha correspondente no fonte Java.
 */
typedef struct {
    uint16_t start_pc;    /**< Offset no bytecode onde a linha Java começa. */
    uint16_t line_number; /**< Número da linha no arquivo fonte `.java` correspondente. */
} LineNumber_info;

/**
 * @brief Atributo LineNumberTable — sub-atributo opcional do Code (depuração).
 *
 * Tabela que associa offsets de bytecode a números de linha no fonte Java.
 * Usada por depuradores e na geração de stack traces.
 */
typedef struct {
    uint16_t line_number_table_length; /**< Número de entradas na tabela. */
    LineNumber_info *line_number_table; /**< Array de mapeamentos (offset → linha). */
} LineNumberTable_attribute;

/**
 * @brief Entrada da tabela de variáveis locais — sub-atributo do Code (depuração).
 *
 * Descreve o nome, tipo e escopo (range de PCs) de uma variável local em um método.
 */
typedef struct {
    uint16_t start_pc;         /**< Offset no bytecode onde a variável entra em escopo. */
    uint16_t length;           /**< Número de bytes de bytecode em que a variável está em escopo. */
    uint16_t name_index;       /**< Índice no constant pool para CONSTANT_Utf8 com o nome da variável. */
    uint16_t descriptor_index; /**< Índice no constant pool para CONSTANT_Utf8 com o descritor de tipo da variável. */
    uint16_t index;            /**< Índice no array de variáveis locais do frame onde a variável é armazenada. */
} LocalVariable_info;

/**
 * @brief Atributo LocalVariableTable — sub-atributo opcional do Code (depuração).
 *
 * Tabela que mapeia slots de variáveis locais a nomes e tipos legíveis,
 * utilizada por depuradores para inspecionar variáveis durante a execução.
 */
typedef struct {
    uint16_t local_variable_table_length;  /**< Número de entradas na tabela. */
    LocalVariable_info *local_variable_table; /**< Array de descrições de variáveis locais. */
} LocalVariableTable_attribute;

/**
 * @brief Entrada da tabela de exceções — sub-estrutura do atributo Code.
 *
 * Define um handler de exceção: o intervalo de bytecode protegido e o offset
 * do handler a ser chamado quando uma exceção do tipo indicado for lançada.
 */
typedef struct {
    uint16_t start_pc;   /**< Offset de início do intervalo protegido (inclusivo). */
    uint16_t end_pc;     /**< Offset de fim do intervalo protegido (exclusivo). */
    uint16_t handler_pc; /**< Offset do bytecode do handler de exceção. */
    uint16_t catch_type; /**< Índice no constant pool para CONSTANT_Class da exceção tratada (0 = finally). */
} ExceptionTable_info;

/**
 * @brief Atributo Code — utilizado na estrutura de métodos.
 *
 * Contém o bytecode executável de um método, junto com informações de tamanho
 * da pilha de operandos, variáveis locais, tabela de exceções e sub-atributos
 * de depuração. É o principal atributo consultado pelo interpretador.
 */
typedef struct {
    uint16_t max_stack;                    /**< Profundidade máxima da pilha de operandos durante a execução. */
    uint16_t max_locals;                   /**< Número máximo de slots de variáveis locais (incluindo parâmetros). */
    uint32_t code_length;                  /**< Número de bytes de bytecode. */
    uint8_t *code;                         /**< Array de bytes do bytecode do método. */
    uint16_t exception_table_length;       /**< Número de entradas na tabela de exceções. */
    ExceptionTable_info *exception_table;  /**< Tabela de handlers de exceção. */
    uint16_t attributes_count;             /**< Número de sub-atributos do Code. */
    attribute_info *attributes;            /**< Array de sub-atributos (e.g. LineNumberTable, LocalVariableTable). */
} Code_attribute;

// ---------------------------------------------------------------------------------------

/**
 * @brief Lê o payload específico de um atributo do arquivo `.class`.
 *
 * Com base no nome do atributo (@p attr_name), aloca e preenche a struct
 * correspondente lendo os bytes de @p file. Para atributos desconhecidos,
 * retorna um buffer bruto de @p attr_length bytes.
 *
 * @param cf          Ponteiro para o ClassFile (usado para resolver nomes do constant pool).
 * @param file        Ponteiro para o arquivo `.class` posicionado no início do payload.
 * @param attr_name   Nome do atributo como string C (e.g. "Code", "ConstantValue").
 * @param attr_length Tamanho em bytes do payload do atributo.
 * @return Ponteiro para a struct alocada do atributo, ou buffer bruto para atributos desconhecidos.
 */
void* read_specific_attribute_info(ClassFile *cf, FILE *file, const char* attr_name, uint32_t attr_length);

#endif
