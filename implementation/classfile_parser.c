#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "constant_pool.h"



// Constant Pool entry format
typedef struct {
    uint8_t tag;
    uint8_t info[];
} cp_info;


// ClassFile structure 
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info **constant_pool;
} ClassFile;


int main() {
    FILE *arquivo;
    const char *file_name = "Main.class";

    arquivo = fopen(file_name, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo1");
        return 1;
    }
    
    ClassFile *cf = malloc(sizeof(ClassFile));
    if (cf == NULL){
        perror("Erro ao alocar ClassFile");
        return 1;
    }

    // MAGIC
    if (fread(&cf->magic, sizeof(cf->magic), 1, arquivo) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = ntohl(cf->magic);
    printf("magic: 0x%08X\n", cf->magic);

    // MINOR_VERSION
    if (fread(&cf->minor_version, sizeof(cf->minor_version), 1, arquivo) != 1) {
        perror("Erro ao ler 'minor_version'");
        return 1;
    }
    cf->minor_version = ntohs(cf->minor_version);
    printf("minor_version: 0x%04X\n", cf->minor_version);

    // MAJOR_VERSION
    if (fread(&cf->major_version, sizeof(cf->major_version), 1, arquivo) != 1) {
        perror("Erro ao ler 'major_version'");
        return 1;
    }
    cf->major_version = ntohs(cf->major_version);
    printf("major_version: 0x%04X\n", cf->major_version);

    // CONSTANT_POOL_COUNT
    if (fread(&cf->constant_pool_count, sizeof(cf->constant_pool_count), 1, arquivo) != 1) {
        perror("Erro ao ler 'constant_pool_count'");
        return 1;
    }
    cf->constant_pool_count = ntohs(cf->constant_pool_count);
    printf("constant_pool_count: 0x%04X\n", cf->constant_pool_count);
    
    
    for (int i = 0; i < cf->constant_pool_count; i++) {
        uint8_t tag;
        if (fread(&tag, sizeof(tag), 1, arquivo) != 1) {
            perror("Erro na leitura da tag");
            return 1;
        }
        // mapear tag para a informação
        // alocar memoria para o tipo da informacao 
        // mapear a entrada para a constant_pool
    }

    free(cf);

    if (fclose(arquivo) != 0) {
        perror("Erro ao fechar o arquivo");
        return 1;
    }


    return 0;
}


void* constant_pool_reader(cp_tags tag) {
    switch (tag) {
        case CONSTANT_Class: {
                    
            CONSTANT_Class_info *entry = malloc(sizeof(CONSTANT_Class_info));
            entry->tag = tag;

            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return;
            }
            entry->name_index = ntohs(entry->name_index);

            return entry;
        }

        case CONSTANT_Fieldref: {
            CONSTANT_Fieldref_info *entry = malloc(sizeof(CONSTANT_Fieldref_info));
            entry->tag = tag;
            
            // Read 'class_index':
            if (fread(&entry->class_index, sizeof(entry->class_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->class_index = ntohs(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = ntohs(entry->name_and_type_index);

            return entry;    
        }

        case CONSTANT_Methodref: {
            CONSTANT_Methodref_info *entry = malloc(sizeof(CONSTANT_Methodref_info));
            entry->tag = tag;
            
            // Read 'class_index':
            if (fread(&entry->class_index, sizeof(entry->class_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->class_index = ntohs(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = ntohs(entry->name_and_type_index);

            return entry;    
        }
        
        case CONSTANT_InterfaceMethodref: {
            CONSTANT_InterfaceMethodref_info *entry = malloc(sizeof(CONSTANT_InterfaceMethodref_info));
            entry->tag = tag;
            
            // Read 'class_index':
            if (fread(&entry->class_index, sizeof(entry->class_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->class_index = ntohs(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = ntohs(entry->name_and_type_index);
    
            return entry;    
        }
        
        case CONSTANT_String: {
            CONSTANT_String_info *entry = malloc(sizeof(CONSTANT_String_info));
            entry->tag = tag;
            
            // Read name_and_type_index:
            if (fread(&entry->string_index, sizeof(entry->string_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->string_index = ntohs(entry->string_index);
            
            return entry;
        }
        
        
        case CONSTANT_Integer: {
            CONSTANT_Integer_info *entry = malloc(sizeof(CONSTANT_Integer_info));
            entry->tag = tag;
            
            // Read name_and_type_index:
            if (fread(&entry->bytes, sizeof(entry->bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bytes = ntohl(entry->bytes);
            
            return entry;
        }
        
        case CONSTANT_Float: {
            CONSTANT_Float_info *entry = malloc(sizeof(CONSTANT_Float_info));
            entry->tag = tag;
            
            // Read name_and_type_index:
            if (fread(&entry->bytes, sizeof(entry->bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bytes = ntohl(entry->bytes);
            
            return entry;
        }
        
        case CONSTANT_Long: {
            CONSTANT_Long_info *entry = malloc(sizeof(CONSTANT_Long_info));
            entry->tag = tag;
            
            // Read name_and_type_index:
            if (fread(&entry->high_bytes, sizeof(entry->high_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->high_bytes = ntohl(entry->high_bytes);
            
            if (fread(&entry->low_bytes, sizeof(entry->low_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->low_bytes = ntohl(entry->low_bytes);

            return entry;
        }
        
        case CONSTANT_Double: {
            CONSTANT_Double_info *entry = malloc(sizeof(CONSTANT_Double_info));
            entry->tag = tag;
            
            // Read name_and_type_index:
            if (fread(&entry->high_bytes, sizeof(entry->high_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->high_bytes = ntohl(entry->high_bytes);
            
            if (fread(&entry->low_bytes, sizeof(entry->low_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->low_bytes = ntohl(entry->low_bytes);
            
            return entry;
        }

        case CONSTANT_NameAndType: {
            CONSTANT_NameAndType_info *entry = malloc(sizeof(CONSTANT_NameAndType_info));
            entry->tag = tag;
            
            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = ntohs(entry->name_index);
            
            if (fread(&entry->descriptor_index, sizeof(entry->descriptor_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->descriptor_index = ntohs(entry->descriptor_index);
            
            return entry;
        }
        
        case CONSTANT_Utf8: {
            break;
        }
        
        case CONSTANT_MethodHandle: {
            CONSTANT_MethodHandle_info *entry = malloc(sizeof(CONSTANT_MethodHandle_info));
            entry->tag = tag;
            
            if (fread(&entry->reference_kind, sizeof(entry->reference_kind), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            
            if (fread(&entry->reference_index, sizeof(entry->reference_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->reference_index = ntohs(entry->reference_index);

            return entry;
        }
        
        case CONSTANT_MethodType: {
            CONSTANT_MethodType_info *entry = malloc(sizeof(CONSTANT_MethodType_info));
            entry->tag = tag;
            
            if (fread(&entry->descriptor_index, sizeof(entry->descriptor_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->descriptor_index = ntohs(entry->descriptor_index);

            return entry;
        }
        
        case CONSTANT_Dynamic: {
            CONSTANT_Dynamic_info *entry = malloc(sizeof(CONSTANT_Dynamic_info));
            entry->tag = tag;
            
            if (fread(&entry->bootstrap_method_attr_index, sizeof(entry->bootstrap_method_attr_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bootstrap_method_attr_index = ntohs(entry->bootstrap_method_attr_index);
            
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = ntohs(entry->name_and_type_index);

            return entry;
        }

        case CONSTANT_InvokeDynamic: {
            CONSTANT_InvokeDynamic_info *entry = malloc(sizeof(CONSTANT_InvokeDynamic_info));
            entry->tag = tag;
            
            if (fread(&entry->bootstrap_method_attr_index, sizeof(entry->bootstrap_method_attr_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bootstrap_method_attr_index = ntohs(entry->bootstrap_method_attr_index);
            
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = ntohs(entry->name_and_type_index);

            return entry;
        }

        case CONSTANT_Package: {
            CONSTANT_Package_info *entry = malloc(sizeof(CONSTANT_Package_info));
            entry->tag = tag;
            
            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = ntohs(entry->name_index);
            
            return entry;
        }


    }
}