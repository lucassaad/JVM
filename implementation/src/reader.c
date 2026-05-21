#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "utils.h"


int read_classfile(ClassFile *cf, FILE *file) {
    // Read 'magic'
    if (fread(&cf->magic, sizeof(cf->magic), 1, file) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = byteswap_u4(cf->magic);

    
    // Read 'minor_version'
    if (fread(&cf->minor_version, sizeof(cf->minor_version), 1, file) != 1) {
        perror("Erro ao ler 'minor_version'");
        return 1;
    }
    cf->minor_version = byteswap_u2(cf->minor_version);


    // Read 'major_version'
    if (fread(&cf->major_version, sizeof(cf->major_version), 1, file) != 1) {
        perror("Erro ao ler 'major_version'");
        return 1;
    }
    cf->major_version = byteswap_u2(cf->major_version);


    // CONSTANT_POOL_COUNT
    if (fread(&cf->constant_pool_count, sizeof(cf->constant_pool_count), 1, file) != 1) {
        perror("Erro ao ler 'constant_pool_count'");
        return 1;
    }
    cf->constant_pool_count = byteswap_u2(cf->constant_pool_count);
    
    cf->constant_pool = malloc(sizeof(cp_info*) * cf->constant_pool_count);
    for (int i = 1; i < cf->constant_pool_count; i++) {
        uint8_t tag_byte;
        if (fread(&tag_byte, sizeof(tag_byte), 1, file) != 1) {
            perror("Erro na leitura da tag");
            return 1;
        }
        cp_tags tag = (cp_tags) tag_byte;
        
        void *entry = constant_pool_reader(tag, file);
        if (entry == NULL){
            perror("Error while reading constant_pool");
            return 1;
        }

        cp_info *cp_entry = malloc(sizeof(cp_info));
        cp_entry->tag = tag;
        cp_entry->info = entry;
        
        cf->constant_pool[i] = cp_entry;
        
        if (tag == CONSTANT_Long || tag == CONSTANT_Double) i++; 
    }
    
    
    // Read 'access_flag'
    if (fread(&cf->access_flags, sizeof(cf->access_flags), 1, file) != 1) {
        perror("Erro ao ler 'access_flags'");
        return 1;
    }
    cf->access_flags = byteswap_u2(cf->access_flags);

    // Read 'this_class'
    if (fread(&cf->this_class, sizeof(cf->this_class), 1, file) != 1) {
        perror("Erro ao ler 'this_class'");
        return 1;
    }
    cf->this_class = byteswap_u2(cf->this_class);
    
    
    // Read 'super_class'
    if (fread(&cf->super_class, sizeof(cf->super_class), 1, file) != 1) {
        perror("Erro ao ler 'super_class'");
        return 1;
    }
    cf->super_class = byteswap_u2(cf->super_class);

    // Lê o índice e a quantidade de interfaces
    if (fread(&cf->interfaces_count, sizeof(cf->interfaces_count), 1, file) != 1) {
        perror("Erro ao ler 'interfaces_count'");
        return 1;
    }
    cf->interfaces_count = byteswap_u2(cf->interfaces_count);

    if (cf->interfaces_count > 0) {
        // Aloca o vetor com base na quantidade lida
        cf->interfaces = malloc(sizeof(uint16_t) * cf->interfaces_count);
        if (cf->interfaces == NULL) {
            perror("Erro ao alocar memória para interfaces");
            return 1;
        }

        // Lê os índices de 2 em 2 bytes
        for (int i = 0; i < cf->interfaces_count; i++) {
            if (fread(&cf->interfaces[i], sizeof(uint16_t), 1, file) != 1) {
                perror("Erro ao ler índice da interface");
                return 1;
            }
            cf->interfaces[i] = byteswap_u2(cf->interfaces[i]); 
        }
    } else {
        // Se a classe não implementar nenhuma interface
        cf->interfaces = NULL;
    }

    // Read 'field_count'
    if (fread(&cf->fields_count, sizeof(cf->fields_count), 1, file) != 1) {
        perror("Erro ao ler 'fields_count'");
        return 1;
    }
    cf->fields_count = byteswap_u2(cf->fields_count);
    
    // Read 'field_info'
    if (cf->fields_count > 0) {
        cf->fields = malloc(sizeof(field_info) * cf->fields_count);
        if (cf->fields == NULL) {
            perror("Erro ao alocar memória para fields");
            return 1;
        }
        
        for (int i = 0; i < cf->fields_count; i++) {
            fread(&cf->fields[i].access_flags, sizeof(uint16_t), 1, file);
            cf->fields[i].access_flags = byteswap_u2(cf->fields[i].access_flags);
            
            fread(&cf->fields[i].name_index, sizeof(uint16_t), 1, file);
            cf->fields[i].name_index = byteswap_u2(cf->fields[i].name_index);
            
            fread(&cf->fields[i].descriptor_index, sizeof(uint16_t), 1, file);
            cf->fields[i].descriptor_index = byteswap_u2(cf->fields[i].descriptor_index);
            
            fread(&cf->fields[i].attributes_count, sizeof(uint16_t), 1, file);
            cf->fields[i].attributes_count = byteswap_u2(cf->fields[i].attributes_count);
            
            // Leitura dos atributos do field
            if (cf->fields[i].attributes_count > 0) {
                cf->fields[i].attributes = malloc(sizeof(attribute_info) * cf->fields[i].attributes_count);
                
                for (int j = 0; j < cf->fields[i].attributes_count; j++) {
                    fread(&cf->fields[i].attributes[j].attribute_name_index, sizeof(uint16_t), 1, file);
                    cf->fields[i].attributes[j].attribute_name_index = byteswap_u2(cf->fields[i].attributes[j].attribute_name_index);
                    
                    fread(&cf->fields[i].attributes[j].attribute_length, sizeof(uint32_t), 1, file);
                    cf->fields[i].attributes[j].attribute_length = byteswap_u4(cf->fields[i].attributes[j].attribute_length);
                    
                    if (cf->fields[i].attributes[j].attribute_length > 0) {
                        cf->fields[i].attributes[j].info = malloc(cf->fields[i].attributes[j].attribute_length);
                        fread(cf->fields[i].attributes[j].info, 1, cf->fields[i].attributes[j].attribute_length, file);
                    } else {
                        cf->fields[i].attributes[j].info = NULL;
                    }
                }
            } else {
                cf->fields[i].attributes = NULL;
            }
        }
    } else {
        cf->fields = NULL;
    }
    
    // Read 'methods_count'
    if (fread(&cf->methods_count, sizeof(cf->methods_count), 1, file) != 1) {
        perror("Erro ao ler 'methods_count'");
        return 1;
    }
    cf->methods_count = byteswap_u2(cf->methods_count);

    // Read 'method[methods_count]': method_info
    if (cf->methods_count > 0) {
        cf->methods = malloc(sizeof(field_info) * cf->methods_count);
        if (cf->methods == NULL) {
            perror("Erro ao alocar memória para methods");
            return 1;
        }

        for (int i = 0; i < cf->methods_count; i++) {
            fread(&cf->methods[i].access_flags, sizeof(uint16_t), 1, file);
            cf->methods[i].access_flags = byteswap_u2(cf->methods[i].access_flags);
            
            fread(&cf->methods[i].name_index, sizeof(uint16_t), 1, file);
            cf->methods[i].name_index = byteswap_u2(cf->methods[i].name_index);
            
            fread(&cf->methods[i].descriptor_index, sizeof(uint16_t), 1, file);
            cf->methods[i].descriptor_index = byteswap_u2(cf->methods[i].descriptor_index);
            
            fread(&cf->methods[i].attributes_count, sizeof(uint16_t), 1, file);
            cf->methods[i].attributes_count = byteswap_u2(cf->methods[i].attributes_count);
            
            // Leitura dos atributos do method
            if (cf->methods[i].attributes_count > 0) {
                cf->methods[i].attributes = malloc(sizeof(attribute_info) * cf->methods[i].attributes_count);
                
                for (int j = 0; j < cf->methods[i].attributes_count; j++) {
                    fread(&cf->methods[i].attributes[j].attribute_name_index, sizeof(uint16_t), 1, file);
                    cf->methods[i].attributes[j].attribute_name_index = byteswap_u2(cf->methods[i].attributes[j].attribute_name_index);
                    
                    fread(&cf->methods[i].attributes[j].attribute_length, sizeof(uint32_t), 1, file);
                    cf->methods[i].attributes[j].attribute_length = byteswap_u4(cf->methods[i].attributes[j].attribute_length);
                    
                    if (cf->methods[i].attributes[j].attribute_length > 0) {
                        cf->methods[i].attributes[j].info = malloc(cf->methods[i].attributes[j].attribute_length);
                        fread(cf->methods[i].attributes[j].info, 1, cf->methods[i].attributes[j].attribute_length, file);
                    } else {
                        cf->methods[i].attributes[j].info = NULL;
                    }
                }
            } else {
                cf->methods[i].attributes = NULL;
            }
        }
    } else {
        cf->methods = NULL;
    }
    return 0;
}