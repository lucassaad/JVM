/**
 * @file viewer.h
 * @brief Funções de exibição legível do conteúdo de um arquivo `.class` (modo Leitor/Exibidor).
 *
 * Implementa o modo de operação `-l` da JVM: em vez de executar o bytecode,
 * imprime a estrutura completa do ClassFile em formato legível por humanos,
 * espelhando a saída tanto para stdout quanto para um arquivo de texto.
 *
 * As funções principais percorrem as seções do ClassFile (constant pool,
 * interfaces, campos, métodos, atributos) e as funções auxiliares resolvem
 * flags de acesso, versões Java e referências do constant pool para strings.
 */

#ifndef VIEWER_H
#define VIEWER_H

#include <stdint.h>
#include "class_file.h"
#include "attributes.h"

// ---------------------------------------------------------------------------------------
// Funções Principais de Exibição
// ---------------------------------------------------------------------------------------

/**
 * @brief Exibe as informações gerais do arquivo `.class`.
 *
 * Imprime número mágico, versões minor/major (com nome da versão Java),
 * flags de acesso da classe, nome da classe, superclasse e contagens
 * de entradas do constant pool, interfaces, campos, métodos e atributos.
 *
 * @param out Ponteiro para o arquivo de saída (pode ser stdout ou arquivo de texto).
 * @param cf  Ponteiro para o ClassFile a ser exibido.
 */
void print_general_information(FILE *out, ClassFile *cf);

/**
 * @brief Exibe todas as entradas mapeadas na tabela do Constant Pool.
 *
 * Percorre o constant pool de índice 1 até constant_pool_count-1, imprimindo
 * o índice, o tipo (tag) e os campos de cada entrada com formatação específica
 * por tipo. Entradas do tipo Long e Double avançam dois índices.
 *
 * @param out Ponteiro para o arquivo de saída.
 * @param cf  Ponteiro para o ClassFile cujo constant pool será exibido.
 */
void print_constant_pool(FILE *out, ClassFile *cf);

/**
 * @brief Exibe a lista de interfaces implementadas pela classe.
 *
 * Para cada índice no array de interfaces, resolve o nome binário da interface
 * via constant pool e o imprime com seu índice.
 *
 * @param out Ponteiro para o arquivo de saída.
 * @param cf  Ponteiro para o ClassFile cujas interfaces serão exibidas.
 */
void print_interfaces(FILE *out, ClassFile *cf);

/**
 * @brief Exibe os campos (variáveis) declarados na classe.
 *
 * Para cada field_info, imprime índice, flags de acesso, nome, descritor
 * e os atributos associados (e.g. ConstantValue).
 *
 * @param out Ponteiro para o arquivo de saída.
 * @param cf  Ponteiro para o ClassFile cujos campos serão exibidos.
 */
void print_fields(FILE *out, ClassFile *cf);

/**
 * @brief Exibe os métodos declarados na classe.
 *
 * Para cada method_info, imprime índice, flags de acesso, nome, descritor
 * e os atributos associados (e.g. Code com bytecode disassemblado,
 * Exceptions, LineNumberTable).
 *
 * @param out Ponteiro para o arquivo de saída.
 * @param cf  Ponteiro para o ClassFile cujos métodos serão exibidos.
 */
void print_methods(FILE *out, ClassFile *cf);

/**
 * @brief Exibe os atributos de nível de ClassFile.
 *
 * Imprime cada attribute_info do array de atributos do ClassFile,
 * decodificando os tipos conhecidos (e.g. SourceFile, InnerClasses).
 *
 * @param out Ponteiro para o arquivo de saída.
 * @param cf  Ponteiro para o ClassFile cujos atributos serão exibidos.
 */
void print_attributes(FILE *out, ClassFile *cf);

// ---------------------------------------------------------------------------------------
// Funções Auxiliares
// ---------------------------------------------------------------------------------------

/**
 * @brief Converte o número da versão Major para a string da versão Java correspondente.
 *
 * Exemplos: 55 → "Java 11", 61 → "Java 17", 65 → "Java 21".
 *
 * @param major_version Número da versão major lido do ClassFile.
 * @return String estática com o nome da versão Java (e.g. "Java 17"), ou "Unknown" se não mapeada.
 */
const char* get_java_version_string(uint16_t major_version);

/**
 * @brief Converte a máscara de bits de access_flags de classe para uma string legível.
 *
 * Percorre os bits definidos (ACC_PUBLIC, ACC_FINAL, ACC_INTERFACE, ACC_ABSTRACT, etc.)
 * e retorna uma string com os modificadores separados por espaço.
 *
 * @param flags Máscara de bits de access_flags lida do ClassFile.
 * @return String estática com os modificadores de acesso (e.g. "public final").
 */
const char* get_class_access_flags_string(uint16_t flags);

/**
 * @brief Resolve e retorna o nome binário de uma classe a partir do constant pool.
 *
 * Acessa a entrada CONSTANT_Class em @p class_index e, a partir dela,
 * obtém o nome via entrada CONSTANT_Utf8 referenciada.
 *
 * @param cf          Ponteiro para o ClassFile que contém o constant pool.
 * @param class_index Índice no constant pool para a entrada CONSTANT_Class.
 * @return String com o nome binário da classe (e.g. "java/lang/Object"), ou NULL se inválido.
 */
const char* get_class_name_string(ClassFile *cf, uint16_t class_index);

/**
 * @brief Exibe as flags de acesso específicas de um campo (field).
 *
 * Interpreta os bits de @p flags conforme os modificadores válidos para campos:
 * ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_VOLATILE,
 * ACC_TRANSIENT, ACC_SYNTHETIC e ACC_ENUM.
 *
 * @param out   Ponteiro para o arquivo de saída.
 * @param flags Máscara de bits de access_flags do field_info.
 */
void print_field_access_flags(FILE *out, uint16_t flags);

/**
 * @brief Exibe as flags de acesso específicas de um método.
 *
 * Interpreta os bits de @p flags conforme os modificadores válidos para métodos:
 * ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_SYNCHRONIZED,
 * ACC_BRIDGE, ACC_VARARGS, ACC_NATIVE, ACC_ABSTRACT, ACC_STRICT e ACC_SYNTHETIC.
 *
 * @param out   Ponteiro para o arquivo de saída.
 * @param flags Máscara de bits de access_flags do method_info.
 */
void print_method_access_flags(FILE *out, uint16_t flags);

/**
 * @brief Decodifica e exibe o conteúdo de um atributo específico.
 *
 * Resolve o nome do atributo via constant pool e, para os tipos conhecidos,
 * exibe os campos estruturados: Code (com bytecode disassemblado via
 * view_instructions(), tabela de exceções e sub-atributos), ConstantValue,
 * Exceptions, SourceFile, LineNumberTable e LocalVariableTable.
 * Atributos desconhecidos são exibidos como bytes brutos hexadecimais.
 *
 * @param out  Ponteiro para o arquivo de saída.
 * @param cf   Ponteiro para o ClassFile (necessário para resolver referências do constant pool).
 * @param attr Ponteiro para o attribute_info a ser exibido.
 */
void print_specific_attribute_info(FILE *out, ClassFile *cf, attribute_info *attr);

// ---------------------------------------------------------------------------------------

#endif
