/**
 * @file reader.h
 * @brief Leitura e validação binária de arquivos `.class` Java.
 *
 * Declara a função principal de leitura (read_classfile()), que percorre
 * sequencialmente o formato binário big-endian de um `.class` e preenche
 * uma struct ClassFile em memória, e as funções de validação semântica
 * que verificam a consistência das referências internas após a leitura.
 *
 * Fluxo típico de uso:
 * -# Abrir o arquivo `.class` com fopen().
 * -# Chamar read_classfile() para preencher o ClassFile.
 * -# As validações são executadas internamente por read_classfile() após cada seção.
 * -# Usar o ClassFile preenchido no viewer ou no interpretador.
 */

#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "class_file.h"

// ---------------------------------------------------------------------------------------
// Funções Auxiliares
// ---------------------------------------------------------------------------------------

/**
 * @brief Lê e aloca dinamicamente um array de atributos a partir do arquivo.
 *
 * Para cada um dos @p count atributos, lê o attribute_name_index e o
 * attribute_length, resolve o nome via constant pool de @p cf para
 * identificar o tipo do atributo, e delega a leitura do payload a
 * read_specific_attribute_info(). Retorna o array alocado com todos
 * os atributos preenchidos.
 *
 * @param cf    Ponteiro para o ClassFile (necessário para resolver nomes do constant pool).
 * @param count Número de atributos a ler.
 * @param file  Ponteiro para o arquivo `.class` posicionado no início dos atributos.
 * @return Array de attribute_info alocado dinamicamente, ou NULL em falha.
 */
attribute_info* read_attributes_array(ClassFile *cf, uint16_t count, FILE *file);

// ---------------------------------------------------------------------------------------
// Funções de Validação
// ---------------------------------------------------------------------------------------

/**
 * @brief Valida o campo this_class do ClassFile.
 *
 * Verifica se this_class é um índice válido no constant pool e se a
 * entrada apontada é do tipo CONSTANT_Class.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_this_class(ClassFile *cf);

/**
 * @brief Valida o campo super_class do ClassFile.
 *
 * Verifica se super_class é 0 (classe Object ou interface sem superclasse)
 * ou um índice válido para uma entrada CONSTANT_Class. Também valida a
 * consistência com a flag ACC_INTERFACE.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_super_class(ClassFile *cf);

/**
 * @brief Valida os índices do array de interfaces do ClassFile.
 *
 * Para cada entrada do array de interfaces, verifica se o índice é válido
 * no constant pool e se aponta para uma entrada CONSTANT_Class.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se todos os índices são válidos, 0 caso contrário.
 */
int validate_interfaces(ClassFile *cf);

/**
 * @brief Valida as declarações de campos (fields) do ClassFile.
 *
 * Para cada field_info, verifica se name_index e descriptor_index são
 * índices válidos no constant pool e se apontam para entradas CONSTANT_Utf8.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se todos os campos são válidos, 0 caso contrário.
 */
int validate_fields(ClassFile *cf);

/**
 * @brief Valida as declarações de métodos do ClassFile.
 *
 * Para cada method_info, verifica se name_index e descriptor_index são
 * índices válidos no constant pool e se apontam para entradas CONSTANT_Utf8.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se todos os métodos são válidos, 0 caso contrário.
 */
int validate_methods(ClassFile *cf);

/**
 * @brief Valida os atributos de nível de ClassFile.
 *
 * Para cada attribute_info do ClassFile, verifica se attribute_name_index
 * é um índice válido no constant pool e aponta para uma entrada CONSTANT_Utf8.
 *
 * @param cf Ponteiro para o ClassFile a validar.
 * @return 1 se todos os atributos são válidos, 0 caso contrário.
 */
int validate_attributes(ClassFile *cf);

// ---------------------------------------------------------------------------------------
// Função Principal
// ---------------------------------------------------------------------------------------

/**
 * @brief Lê e preenche um ClassFile a partir de um arquivo `.class` binário.
 *
 * Percorre sequencialmente o formato definido pela Java Virtual Machine Specification,
 * lendo e convertendo de big-endian para a endianness da plataforma cada seção:
 * magic, versões, constant pool, access_flags, this_class, super_class,
 * interfaces, fields, methods e attributes. Após cada seção, executa a
 * validação semântica correspondente.
 *
 * Pré-condição: @p cf deve apontar para uma struct ClassFile já alocada e
 * @p file deve estar posicionado no início do arquivo.
 *
 * @param cf   Ponteiro para o ClassFile a ser preenchido.
 * @param file Ponteiro para o arquivo `.class` aberto para leitura binária.
 * @return 1 em caso de sucesso, 0 se ocorrer erro de leitura ou falha de validação.
 */
int read_classfile(ClassFile *cf, FILE *file);

#endif
