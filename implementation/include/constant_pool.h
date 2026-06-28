/**
 * @file constant_pool.h
 * @brief Estruturas e funções relacionadas ao Constant Pool de um arquivo `.class`.
 *
 * Define todas as structs que representam os 17 tipos de entradas do constant pool,
 * conforme a Java Virtual Machine Specification, além do enum de tags e das funções
 * de leitura e validação do pool.
 */

#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <stdio.h>
#include <stdint.h>
#include "class_file.h"

/**
 * @brief Entrada do tipo Class no constant pool.
 *
 * Representa uma referência simbólica a uma classe ou interface.
 */
typedef struct CONSTANT_Class_info {
    uint8_t tag;        /**< Tag identificadora: CONSTANT_Class (7). */
    uint16_t name_index; /**< Índice para uma entrada CONSTANT_Utf8 com o nome binário da classe (e.g. "java/lang/Object"). */
} CONSTANT_Class_info;

/**
 * @brief Entrada do tipo Fieldref no constant pool.
 *
 * Representa uma referência simbólica a um campo de classe ou instância.
 */
typedef struct {
    uint8_t tag;                  /**< Tag identificadora: CONSTANT_Fieldref (9). */
    uint16_t class_index;         /**< Índice para uma entrada CONSTANT_Class com a classe que declara o campo. */
    uint16_t name_and_type_index; /**< Índice para uma entrada CONSTANT_NameAndType com nome e descritor do campo. */
} CONSTANT_Fieldref_info;

/**
 * @brief Entrada do tipo Methodref no constant pool.
 *
 * Representa uma referência simbólica a um método de classe (não de interface).
 */
typedef struct {
    uint8_t tag;                  /**< Tag identificadora: CONSTANT_Methodref (10). */
    uint16_t class_index;         /**< Índice para uma entrada CONSTANT_Class com a classe que declara o método. */
    uint16_t name_and_type_index; /**< Índice para uma entrada CONSTANT_NameAndType com nome e descritor do método. */
} CONSTANT_Methodref_info;

/**
 * @brief Entrada do tipo InterfaceMethodref no constant pool.
 *
 * Representa uma referência simbólica a um método declarado em uma interface.
 */
typedef struct {
    uint8_t tag;                  /**< Tag identificadora: CONSTANT_InterfaceMethodref (11). */
    uint16_t class_index;         /**< Índice para uma entrada CONSTANT_Class com a interface que declara o método. */
    uint16_t name_and_type_index; /**< Índice para uma entrada CONSTANT_NameAndType com nome e descritor do método. */
} CONSTANT_InterfaceMethodref_info;

/**
 * @brief Entrada do tipo String no constant pool.
 *
 * Representa uma constante literal do tipo String.
 */
typedef struct {
    uint8_t tag;          /**< Tag identificadora: CONSTANT_String (8). */
    uint16_t string_index; /**< Índice para uma entrada CONSTANT_Utf8 com o conteúdo da string. */
} CONSTANT_String_info;

/**
 * @brief Entrada do tipo Integer no constant pool.
 *
 * Representa uma constante inteira de 32 bits (int).
 */
typedef struct {
    uint8_t tag;    /**< Tag identificadora: CONSTANT_Integer (3). */
    uint32_t bytes; /**< Valor inteiro de 32 bits em formato big-endian. */
} CONSTANT_Integer_info;

/**
 * @brief Entrada do tipo Float no constant pool.
 *
 * Representa uma constante de ponto flutuante de 32 bits (float).
 */
typedef struct {
    uint8_t tag;    /**< Tag identificadora: CONSTANT_Float (4). */
    uint32_t bytes; /**< Bits do valor float IEEE 754 de 32 bits em formato big-endian. */
} CONSTANT_Float_info;

/**
 * @brief Entrada do tipo Long no constant pool.
 *
 * Representa uma constante inteira de 64 bits (long).
 * Ocupa duas entradas consecutivas no constant pool (a seguinte fica inacessível).
 */
typedef struct {
    uint8_t tag;        /**< Tag identificadora: CONSTANT_Long (5). */
    uint32_t high_bytes; /**< 32 bits mais significativos do valor long, em big-endian. */
    uint32_t low_bytes;  /**< 32 bits menos significativos do valor long, em big-endian. */
} CONSTANT_Long_info;

/**
 * @brief Entrada do tipo Double no constant pool.
 *
 * Representa uma constante de ponto flutuante de 64 bits (double).
 * Ocupa duas entradas consecutivas no constant pool (a seguinte fica inacessível).
 */
typedef struct {
    uint8_t tag;        /**< Tag identificadora: CONSTANT_Double (6). */
    uint32_t high_bytes; /**< 32 bits mais significativos do valor double IEEE 754, em big-endian. */
    uint32_t low_bytes;  /**< 32 bits menos significativos do valor double IEEE 754, em big-endian. */
} CONSTANT_Double_info;

/**
 * @brief Entrada do tipo NameAndType no constant pool.
 *
 * Representa o par (nome, descritor) de um campo ou método, sem indicar a classe.
 */
typedef struct {
    uint8_t tag;             /**< Tag identificadora: CONSTANT_NameAndType (12). */
    uint16_t name_index;     /**< Índice para uma entrada CONSTANT_Utf8 com o nome simples do campo ou método. */
    uint16_t descriptor_index; /**< Índice para uma entrada CONSTANT_Utf8 com o descritor de tipo. */
} CONSTANT_NameAndType_info;

/**
 * @brief Entrada do tipo Utf8 no constant pool.
 *
 * Armazena uma string codificada em Modified UTF-8. Usada para nomes, descritores
 * e valores literais de string. O array @p bytes é terminado em `\0` para
 * compatibilidade com funções C de string.
 */
typedef struct {
    uint8_t tag;      /**< Tag identificadora: CONSTANT_Utf8 (1). */
    uint16_t length;  /**< Número de bytes da string (sem contar o terminador nulo). */
    uint8_t bytes[];  /**< Conteúdo da string em Modified UTF-8, terminado em '\0'. */
} CONSTANT_Utf8_info;

/**
 * @brief Entrada do tipo MethodHandle no constant pool.
 *
 * Representa um handle para um método, campo ou construtor, usado em lambdas e
 * chamadas dinâmicas via invokedynamic.
 */
typedef struct {
    uint8_t tag;             /**< Tag identificadora: CONSTANT_MethodHandle (15). */
    uint8_t reference_kind;  /**< Tipo do handle (valores 1–9 conforme a JVMS). */
    uint16_t reference_index; /**< Índice para a entrada correspondente (Fieldref, Methodref, etc.). */
} CONSTANT_MethodHandle_info;

/**
 * @brief Entrada do tipo MethodType no constant pool.
 *
 * Representa o tipo de um método (apenas descritor, sem nome ou classe).
 */
typedef struct {
    uint8_t tag;              /**< Tag identificadora: CONSTANT_MethodType (16). */
    uint16_t descriptor_index; /**< Índice para uma entrada CONSTANT_Utf8 com o descritor do método. */
} CONSTANT_MethodType_info;

/**
 * @brief Entrada do tipo Dynamic no constant pool.
 *
 * Representa uma constante computada dinamicamente via bootstrap method.
 */
typedef struct {
    uint8_t tag;                          /**< Tag identificadora: CONSTANT_Dynamic (17). */
    uint16_t bootstrap_method_attr_index; /**< Índice na tabela BootstrapMethods do atributo homônimo. */
    uint16_t name_and_type_index;         /**< Índice para uma entrada CONSTANT_NameAndType. */
} CONSTANT_Dynamic_info;

/**
 * @brief Entrada do tipo InvokeDynamic no constant pool.
 *
 * Usada pela instrução `invokedynamic` para referência a um método bootstrap.
 */
typedef struct {
    uint8_t tag;                          /**< Tag identificadora: CONSTANT_InvokeDynamic (18). */
    uint16_t bootstrap_method_attr_index; /**< Índice na tabela BootstrapMethods do atributo homônimo. */
    uint16_t name_and_type_index;         /**< Índice para uma entrada CONSTANT_NameAndType. */
} CONSTANT_InvokeDynamic_info;

/**
 * @brief Entrada do tipo Module no constant pool.
 *
 * Representa o nome de um módulo Java (sistema de módulos introduzido no Java 9).
 */
typedef struct {
    uint8_t tag;        /**< Tag identificadora: CONSTANT_Module (19). */
    uint16_t name_index; /**< Índice para uma entrada CONSTANT_Utf8 com o nome do módulo. */
} CONSTANT_Module_info;

/**
 * @brief Entrada do tipo Package no constant pool.
 *
 * Representa o nome de um pacote Java, usado em declarações de módulos.
 */
typedef struct {
    uint8_t tag;        /**< Tag identificadora: CONSTANT_Package (20). */
    uint16_t name_index; /**< Índice para uma entrada CONSTANT_Utf8 com o nome do pacote (formato binário, e.g. "java/lang"). */
} CONSTANT_Package_info;

//---------------------------------------------------------------------------------------

/**
 * @brief Dicionário de tags usado pela JVM para identificar o tipo de constante lida.
 *
 * Cada valor corresponde à tag que precede a entrada no arquivo `.class`.
 * Tags 2, 13 e 14 não existem na especificação atual.
 * Entradas do tipo Long (5) e Double (6) ocupam dois slots consecutivos no pool.
 */
typedef enum cp_tags {
    CONSTANT_Utf8              = 1,  /**< String Modified UTF-8. */
    CONSTANT_Integer           = 3,  /**< Constante inteira de 32 bits. */
    CONSTANT_Float             = 4,  /**< Constante float IEEE 754 de 32 bits. */
    CONSTANT_Long              = 5,  /**< Constante long de 64 bits (ocupa 2 slots). */
    CONSTANT_Double            = 6,  /**< Constante double IEEE 754 de 64 bits (ocupa 2 slots). */
    CONSTANT_Class             = 7,  /**< Referência simbólica a uma classe ou interface. */
    CONSTANT_String            = 8,  /**< Literal String. */
    CONSTANT_Fieldref          = 9,  /**< Referência simbólica a um campo. */
    CONSTANT_Methodref         = 10, /**< Referência simbólica a um método de classe. */
    CONSTANT_InterfaceMethodref = 11, /**< Referência simbólica a um método de interface. */
    CONSTANT_NameAndType       = 12, /**< Par (nome, descritor) de campo ou método. */
    CONSTANT_MethodHandle      = 15, /**< Handle de método para chamadas dinâmicas. */
    CONSTANT_MethodType        = 16, /**< Tipo de método (somente descritor). */
    CONSTANT_Dynamic           = 17, /**< Constante dinâmica via bootstrap. */
    CONSTANT_InvokeDynamic     = 18, /**< Referência a bootstrap para invokedynamic. */
    CONSTANT_Module            = 19, /**< Nome de módulo. */
    CONSTANT_Package           = 20  /**< Nome de pacote. */
} cp_tags;

/**
 * @brief Lê dinamicamente os bytes do arquivo de acordo com a tag fornecida.
 *
 * Aloca e preenche a struct correspondente ao tipo de entrada do constant pool
 * indicado por @p tag, lendo os bytes necessários de @p file.
 *
 * @param tag  Tag que identifica o tipo da entrada a ser lida.
 * @param file Ponteiro para o arquivo `.class` posicionado no início dos dados da entrada.
 * @return Ponteiro genérico (void*) para a struct alocada, ou NULL em caso de erro.
 */
void* constant_pool_reader(cp_tags tag, FILE *file);

/**
 * @brief Varre o Constant Pool e valida os índices internos de cada entrada.
 *
 * Verifica se todos os índices cruzados (e.g. class_index aponta para CONSTANT_Class,
 * name_index aponta para CONSTANT_Utf8) são válidos e apontam para o tipo correto.
 *
 * @param cf Ponteiro para o ClassFile cujo constant pool será validado.
 * @return 1 se todas as referências são válidas, 0 caso contrário.
 */
int check_constant_pool_references(ClassFile *cf);

#endif
