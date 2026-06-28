/**
 * @file utils.c
 * @brief Utilitários de conversão big-endian e liberação recursiva de ClassFile.
 *
 * byteswap_u2 e byteswap_u4 realizam a conversão de big-endian (formato `.class`)
 * para little-endian (x86/x64) por manipulação de bits, sem depender de funções
 * de sistema (ntohs/ntohl), garantindo portabilidade.
 *
 * deep_free() percorre e libera toda a hierarquia de memória de um ClassFile
 * na ordem inversa de dependência:
 *   1. Constant pool: itera de i=1 (índice 0 é reservado); ao encontrar tag 5
 *      (CONSTANT_Long) ou 6 (CONSTANT_Double) incrementa i para pular o slot
 *      fantasma NULL que ocupa o índice seguinte. Libera entry->info e entry.
 *   2. Interfaces: vetor plano de uint16_t, free simples.
 *   3. Fields: para cada field, libera o vetor de atributos (info de cada um)
 *      antes de liberar o vetor de fields.
 *   4. Methods: para cada método, distingue o atributo "Code" dos demais — Code
 *      possui alocação interna extra (code->code, o array de bytecode) que deve
 *      ser liberado antes do próprio Code_attribute. Atributos desconhecidos são
 *      tratados como buffer bruto (free direto). Por fim libera o vetor de atributos
 *      e o vetor de methods.
 *   5. Atributos da classe (ex: SourceFile): info liberado individualmente, depois
 *      o vetor.
 *   6. A própria struct ClassFile é liberada por último.
 *
 * O ponteiro cf em si é liberado — o chamador não deve acessá-lo após deep_free().
 */

#include <stdint.h>
#include <stdlib.h>
#include "utils.h"
#include "class_file.h"
#include "attributes.h"
#include "constant_pool.h"
#include <string.h>

// Inverte 2 bytes (u2 / uint16_t)
uint16_t byteswap_u2(uint16_t val) {
    return (val >> 8) | (val << 8);
}

// Inverte 4 bytes (u4 / uint32_t)
uint32_t byteswap_u4(uint32_t val) {
    return ((val >> 24) & 0x000000FF) |
           ((val >> 8)  & 0x0000FF00) |
           ((val << 8)  & 0x00FF0000) |
           ((val << 24) & 0xFF000000);
}

void deep_free(ClassFile *cf) {
    if (cf == NULL) return;

    // 1. Constant pool
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *entry = cf->constant_pool[i];
        if (entry != NULL) {
            if (entry->tag == 5 || entry->tag == 6) i++;
            free(entry->info);
            free(entry);
        }
    }
    free(cf->constant_pool);

    // 2. Interfaces
    free(cf->interfaces);

    // 3. Fields e seus atributos
    for (int i = 0; i < cf->fields_count; i++) {
        for (int j = 0; j < cf->fields[i].attributes_count; j++) {
            free(cf->fields[i].attributes[j].info);
        }
        free(cf->fields[i].attributes);
    }
    free(cf->fields);

    // 4. Methods e seus atributos
    for (int i = 0; i < cf->methods_count; i++) {
        for (int j = 0; j < cf->methods[i].attributes_count; j++) {
            attribute_info *attr = &cf->methods[i].attributes[j];
            
            if (attr->info == NULL) continue;

            // Verifica se é o atributo "Code"
            cp_info *cp_entry = cf->constant_pool[attr->attribute_name_index];
            CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info*) cp_entry->info;

            if (utf8->length == 4 && strncmp((char*)utf8->bytes, "Code", 4) == 0) {
                // É Code — libera o bytecode interno primeiro
                Code_attribute *code = (Code_attribute*) attr->info;
                free(code->code);
                free(code);
            } else {
                // Outros atributos são bytes crus
                free(attr->info);
            }
        }
        free(cf->methods[i].attributes);
    }
    free(cf->methods);

    // 5. Atributos da classe (ex: SourceFile)
    for (int i = 0; i < cf->attributes_count; i++) {
        free(cf->attributes[i].info);
    }
    free(cf->attributes);

    // 6. ClassFile
    free(cf);
}