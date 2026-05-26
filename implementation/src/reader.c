#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "utils.h"
#include "attributes.h"
#include "constant_pool.h"
#include "attributes.h"

// Função auxiliar para ler um array de atributos genérico
attribute_info* read_attributes_array(ClassFile *cf, uint16_t count, FILE *file) {
    if (count == 0) return NULL;

    attribute_info *attrs = malloc(sizeof(attribute_info) * count);
    if (attrs == NULL) {
        perror("Erro ao alocar memória para array de atributos");
        return NULL;
    }

    for (int j = 0; j < count; j++) {
        fread(&attrs[j].attribute_name_index, sizeof(uint16_t), 1, file);
        attrs[j].attribute_name_index = byteswap_u2(attrs[j].attribute_name_index);
        
        fread(&attrs[j].attribute_length, sizeof(uint32_t), 1, file);
        attrs[j].attribute_length = byteswap_u4(attrs[j].attribute_length);
        
        cp_info *cp_entry = cf->constant_pool[attrs[j].attribute_name_index];
        CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)cp_entry->info;

        char *attr_name = malloc(utf8->length + 1);
        memcpy(attr_name, utf8->bytes, utf8->length);
        attr_name[utf8->length] = '\0';

        attrs[j].info = read_specific_attribute_info(cf, file, attr_name, attrs[j].attribute_length);

        free(attr_name); 
    }
    return attrs;
}

// Função para validar o this_class
int validate_this_class(ClassFile *cf) {
    if (cf->this_class <= 0 || cf->this_class >= cf->constant_pool_count) {
        printf("Erro de Validação: this_class index (%d) fora dos limites da constant pool.\n", cf->this_class);
        return 0; 
    }

    if (cf->constant_pool[cf->this_class]->tag != CONSTANT_Class) {
        printf("Erro de Validação: this_class não aponta para um CONSTANT_Class (tag %d). Tag encontrada: %d\n", 
                CONSTANT_Class, cf->constant_pool[cf->this_class]->tag);
        return 0; 
    }

    return 1; 
}

// Função para validar o super_class
int validate_super_class(ClassFile *cf) {
    if (cf->super_class == 0) {
        if ((cf->access_flags & 0x0200) != 0) {
            printf("Erro de Validação: O arquivo é uma Interface, portanto super_class não pode ser 0.\n");
            return 0; 
        }
        return 1; 
    }

    if (cf->super_class >= cf->constant_pool_count) {
        printf("Erro de Validação: super_class index (%d) fora dos limites da constant pool.\n", cf->super_class);
        return 0; 
    }

    if (cf->constant_pool[cf->super_class]->tag != CONSTANT_Class) {
        printf("Erro de Validação: super_class não aponta para um CONSTANT_Class (tag %d). Tag encontrada: %d\n", 
                CONSTANT_Class, cf->constant_pool[cf->super_class]->tag);
        return 0; 
    }

    return 1; 
}

// Função para validar interfaces
int validate_interfaces(ClassFile *cf) {
    for (int i = 0; i < cf->interfaces_count; i++) {
        uint16_t interface_idx = cf->interfaces[i];

        if (interface_idx <= 0 || interface_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: A interface na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, interface_idx);
            return 0; 
        }

        if (cf->constant_pool[interface_idx]->tag != CONSTANT_Class) {
            printf("Erro de Validação: A interface na posição %d não aponta para um CONSTANT_Class (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Class, cf->constant_pool[interface_idx]->tag);
            return 0; 
        }
    }

    return 1; 
}

// Função para validar fields
int validate_fields(ClassFile *cf) {
    for (int i = 0; i < cf->fields_count; i++) {
        uint16_t name_idx = cf->fields[i].name_index;
        uint16_t desc_idx = cf->fields[i].descriptor_index;

        if (name_idx == 0 || name_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: O name_index do field na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, name_idx);
            return 0; 
        }
        
        if (cf->constant_pool[name_idx]->tag != CONSTANT_Utf8) {
            printf("Erro de Validação: O name_index do field %d não aponta para um CONSTANT_Utf8 (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Utf8, cf->constant_pool[name_idx]->tag);
            return 0; 
        }

        if (desc_idx == 0 || desc_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: O descriptor_index do field na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, desc_idx);
            return 0; 
        }
        
        if (cf->constant_pool[desc_idx]->tag != CONSTANT_Utf8) {
            printf("Erro de Validação: O descriptor_index do field %d não aponta para um CONSTANT_Utf8 (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Utf8, cf->constant_pool[desc_idx]->tag);
            return 0; 
        }
    }

    return 1; 
}

// Função para validar methods
int validate_methods(ClassFile *cf) {
    // Percorre cada method que foi lido
    for (int i = 0; i < cf->methods_count; i++) {
        uint16_t name_idx = cf->methods[i].name_index;
        uint16_t desc_idx = cf->methods[i].descriptor_index;

        if (name_idx == 0 || name_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: O name_index do method na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, name_idx);
            return 0; 
        }
        
        if (cf->constant_pool[name_idx]->tag != CONSTANT_Utf8) {
            printf("Erro de Validação: O name_index do method %d não aponta para um CONSTANT_Utf8 (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Utf8, cf->constant_pool[name_idx]->tag);
            return 0;
        }

        if (desc_idx == 0 || desc_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: O descriptor_index do method na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, desc_idx);
            return 0; 
        }
        
        if (cf->constant_pool[desc_idx]->tag != CONSTANT_Utf8) {
            printf("Erro de Validação: O descriptor_index do method %d não aponta para um CONSTANT_Utf8 (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Utf8, cf->constant_pool[desc_idx]->tag);
            return 0;
        }
    }

    return 1; 
}

// Função para validar attributes
int validate_attributes(ClassFile *cf) {
    for (int i = 0; i < cf->attributes_count; i++) {
        uint16_t name_idx = cf->attributes[i].attribute_name_index;

        if (name_idx == 0 || name_idx >= cf->constant_pool_count) {
            printf("Erro de Validação: O attribute_name_index do atributo na posição %d aponta para um índice inválido (%d) na constant pool.\n", 
                   i, name_idx);
            return 0; 
        }
        
        if (cf->constant_pool[name_idx]->tag != CONSTANT_Utf8) {
            printf("Erro de Validação: O attribute_name_index do atributo %d não aponta para um CONSTANT_Utf8 (tag %d). Tag encontrada: %d\n", 
                   i, CONSTANT_Utf8, cf->constant_pool[name_idx]->tag);
            return 0; 
        }
    }

    return 1; 
}

int read_classfile(ClassFile *cf, FILE *file) {

    // Read 'magic'
    if (fread(&cf->magic, sizeof(cf->magic), 1, file) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = byteswap_u4(cf->magic);
    if (cf->magic != 0xCAFEBABE) {
        perror("Magic invalido, o arquivo nao e compativel com .class");
    }

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

        if (entry == NULL) {
            perror("Error while reading constant_pool");
            return 1;
        }

        cp_info *cp_entry = malloc(sizeof(cp_info));

        cp_entry->tag = tag;
        cp_entry->info = entry;

        cf->constant_pool[i] = cp_entry;

        if (tag == CONSTANT_Long || tag == CONSTANT_Double) {
            i++; 
            cf->constant_pool[i] = NULL;
        }
            
    }

    // Read 'access_flags'
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

    // Valida 'this_class'
    if (!validate_this_class(cf)) {
        return 1; 
    }
    
    // Read 'super_class'
    if (fread(&cf->super_class, sizeof(cf->super_class), 1, file) != 1) {
        perror("Erro ao ler 'super_class'");
        return 1;
    }

    cf->super_class = byteswap_u2(cf->super_class);

    // Valida 'super_class'
    if (!validate_super_class(cf)) {
        return 1; 
    }

    // Read 'interfaces_count'
    if (fread(&cf->interfaces_count, sizeof(cf->interfaces_count), 1, file) != 1) {
        perror("Erro ao ler 'interfaces_count'");
        return 1;
    }

    cf->interfaces_count = byteswap_u2(cf->interfaces_count);

    if (cf->interfaces_count > 0) {

        cf->interfaces = malloc(sizeof(uint16_t) * cf->interfaces_count);

        if (cf->interfaces == NULL) {
            perror("Erro ao alocar memória para interfaces");
            return 1;
        }

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

    // Valida 'interfaces'
    if (!validate_interfaces(cf)) {
        return 1; 
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
            cf->fields[i].attributes = read_attributes_array(cf, cf->fields[i].attributes_count, file);
        }

    } else {

        cf->fields = NULL;
    }

    // Valida 'fields'
    if (!validate_fields(cf)) {
        return 1; 
    }

    // Read 'methods_count'
    if (fread(&cf->methods_count, sizeof(cf->methods_count), 1, file) != 1) {
        perror("Erro ao ler 'methods_count'");
        return 1;
    }

    cf->methods_count = byteswap_u2(cf->methods_count);

    // Read methods
    if (cf->methods_count > 0) {

        cf->methods = malloc(sizeof(method_info) * cf->methods_count);

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
            cf->methods[i].attributes = read_attributes_array(cf, cf->methods[i].attributes_count, file);
        }
    } else {
        cf->methods = NULL;
    }

    // Valida 'methods'
    if (!validate_methods(cf)) {
        return 1; 
    }

    if (fread(&cf->attributes_count, sizeof(cf->attributes_count), 1, file) != 1) {
        perror("Erro ao ler 'attributes_count' global");
        return 1;
    }
    cf->attributes_count = byteswap_u2(cf->attributes_count);

    cf->attributes = read_attributes_array(cf, cf->attributes_count, file);

    // Valida 'attributes'
    if (!validate_attributes(cf)) {
        return 1; 
    }
    return 0;
}