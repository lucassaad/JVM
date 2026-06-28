/**
 * @file class_file.h
 * @brief Estruturas que representam o formato binário de um arquivo `.class` Java.
 *
 * Define as structs principais usadas para armazenar em memória o conteúdo
 * de um `.class` após a leitura binária: entradas do constant pool, atributos,
 * campos, métodos e o próprio ClassFile.
 */

#ifndef CLASS_FILE_H
#define CLASS_FILE_H

#include <stdint.h>

/**
 * @brief Definição genérica interna de cada item do Pool
 *
 * Cada entrada do constant pool é armazenada como um par (tag, ponteiro),
 * onde o ponteiro aponta para a struct específica do tipo indicado pela tag
 * (e.g. CONSTANT_Class_info, CONSTANT_Utf8_info).
 */
typedef struct {
    uint8_t tag;  /**< Tag que identifica o tipo da entrada (ver enum cp_tags). */
    void *info;   /**< Ponteiro para a struct específica da entrada. */
} cp_info;

/**
 * @brief Estrutura do Atributo
 *
 * Representa um atributo genérico de um campo, método ou do próprio ClassFile.
 * O campo @p info pode apontar para structs específicas como Code_attribute
 * ou ConstantValue_attribute, conforme o nome do atributo.
 */
typedef struct {
    uint16_t attribute_name_index; /**< Índice no constant pool para o nome do atributo (CONSTANT_Utf8). */
    uint32_t attribute_length;     /**< Tamanho em bytes do conteúdo do atributo. */
    void *info;                    /**< Ponteiro para a struct específica do atributo, ou bytes brutos. */
} attribute_info;

/**
 * @brief Estrutura de um Field (Variável de Classe)
 *
 * Representa a declaração de um campo (estático ou de instância) conforme
 * descrito na especificação JVM. O campo @p static_value armazena o valor
 * estático atual do campo quando ele é declarado como `static`.
 */
typedef struct {
    uint16_t access_flags;      /**< Máscara de bits com os modificadores de acesso (public, static, final, etc.). */
    uint16_t name_index;        /**< Índice no constant pool para o nome do campo (CONSTANT_Utf8). */
    uint16_t descriptor_index;  /**< Índice no constant pool para o descritor de tipo do campo (CONSTANT_Utf8). */
    uint16_t attributes_count;  /**< Número de atributos associados ao campo. */
    attribute_info *attributes; /**< Array de atributos do campo (e.g. ConstantValue). */
    uint64_t static_value;      /**< Valor atual do campo estático em tempo de execução. */
} field_info;

/**
 * @brief Estrutura de um Método
 *
 * Representa a declaração de um método conforme descrito na especificação JVM.
 * O bytecode executável está no atributo Code, presente em @p attributes.
 */
typedef struct {
    uint16_t access_flags;      /**< Máscara de bits com os modificadores de acesso (public, static, native, etc.). */
    uint16_t name_index;        /**< Índice no constant pool para o nome do método (CONSTANT_Utf8). */
    uint16_t descriptor_index;  /**< Índice no constant pool para o descritor de assinatura (CONSTANT_Utf8). */
    uint16_t attributes_count;  /**< Número de atributos associados ao método. */
    attribute_info *attributes; /**< Array de atributos do método (e.g. Code, Exceptions). */
} method_info;

/**
 * @brief Estrutura principal do arquivo .class
 *
 * Representa o conteúdo completo de um arquivo `.class` Java carregado em memória,
 * seguindo o formato definido pela Java Virtual Machine Specification.
 * É preenchida pela função read_classfile() e consultada pelo viewer e pelo interpretador.
 */
typedef struct {
    uint32_t magic;                  /**< Número mágico obrigatório: deve ser 0xCAFEBABE. */
    uint16_t minor_version;          /**< Versão menor do formato do arquivo .class. */
    uint16_t major_version;          /**< Versão maior (e.g. 55 = Java 11, 61 = Java 17). */
    uint16_t constant_pool_count;    /**< Número de entradas no constant pool + 1 (índices de 1 a count-1). */
    cp_info **constant_pool;         /**< Array de ponteiros para as entradas do constant pool. */
    uint16_t access_flags;           /**< Máscara de bits com os modificadores da classe (public, final, interface, etc.). */
    uint16_t this_class;             /**< Índice no constant pool para a entrada CONSTANT_Class desta classe. */
    uint16_t super_class;            /**< Índice no constant pool para a entrada CONSTANT_Class da superclasse (0 se Object). */
    uint16_t interfaces_count;       /**< Número de interfaces implementadas. */
    uint16_t *interfaces;            /**< Array de índices no constant pool para as interfaces (CONSTANT_Class). */
    uint16_t fields_count;           /**< Número de campos declarados. */
    field_info *fields;              /**< Array de campos da classe. */
    uint16_t methods_count;          /**< Número de métodos declarados. */
    method_info *methods;            /**< Array de métodos da classe. */
    uint16_t attributes_count;       /**< Número de atributos do ClassFile. */
    attribute_info *attributes;      /**< Array de atributos do ClassFile (e.g. SourceFile). */
} ClassFile;

#endif
