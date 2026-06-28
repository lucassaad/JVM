/**
 * @file utils.h
 * @brief Utilitários de conversão de bytes e liberação de memória do ClassFile.
 *
 * Fornece funções auxiliares usadas transversalmente pelo leitor e pelo interpretador:
 * conversão de big-endian para little-endian (necessária pois o formato `.class` é
 * big-endian e a plataforma alvo é little-endian) e liberação recursiva de toda
 * a memória alocada para um ClassFile.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "class_file.h"

/**
 * @brief Inverte a ordem dos bytes de um valor de 16 bits (big-endian → little-endian).
 *
 * Usada após a leitura de campos u2 do arquivo `.class` para converter
 * do formato big-endian da JVM para a endianness nativa da plataforma.
 *
 * @param val Valor de 16 bits em big-endian lido do arquivo `.class`.
 * @return Valor de 16 bits com a ordem dos bytes invertida.
 */
uint16_t byteswap_u2(uint16_t val);

/**
 * @brief Inverte a ordem dos bytes de um valor de 32 bits (big-endian → little-endian).
 *
 * Usada após a leitura de campos u4 do arquivo `.class` para converter
 * do formato big-endian da JVM para a endianness nativa da plataforma.
 *
 * @param val Valor de 32 bits em big-endian lido do arquivo `.class`.
 * @return Valor de 32 bits com a ordem dos bytes invertida.
 */
uint32_t byteswap_u4(uint32_t val);

/**
 * @brief Libera toda a memória alocada dinamicamente para um ClassFile.
 *
 * Percorre recursivamente a hierarquia completa do ClassFile e libera
 * cada estrutura interna na ordem correta para evitar vazamentos de memória:
 * - Entradas do constant pool (cada struct CONSTANT_*_info alocada individualmente).
 * - Atributos de campos, métodos e do ClassFile (payload e struct attribute_info).
 *   Para o atributo Code, libera também o array de bytecode e os sub-atributos.
 * - Arrays de fields, methods, interfaces e attributes do ClassFile.
 * - A própria struct ClassFile não é liberada (responsabilidade do chamador).
 *
 * Entradas do tipo Long e Double são tratadas com atenção ao salto de dois
 * slots consecutivos no constant pool.
 *
 * @param cf Ponteiro para o ClassFile cujos recursos internos serão liberados.
 */
void deep_free(ClassFile *cf);

#endif
