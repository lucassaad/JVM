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