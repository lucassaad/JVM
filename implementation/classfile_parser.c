#include <string.h>
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

void* constant_pool_reader(cp_tags tag, FILE *file);
int read_classfile (ClassFile *cf, FILE *file);


int main() {
    FILE *file;
    const char *file_name = "Main.class";

    file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Erro ao abrir file");
        return 1;
    }
    
    ClassFile *cf = malloc(sizeof(ClassFile));
    if (cf == NULL){
        perror("Erro ao alocar ClassFile");
        return 1;
    }

    read_classfile(cf, file);

    free(cf);

    if (fclose(file) != 0) {
        perror("Erro ao fechar o file");
        return 1;
    }


    return 0;
}


void* constant_pool_reader(cp_tags tag, FILE *file) {
    switch (tag) {
        case CONSTANT_Class: {
                    
            CONSTANT_Class_info *entry = malloc(sizeof(CONSTANT_Class_info));
            entry->tag = tag;

            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
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
            uint16_t length;
            if (fread(&length, sizeof(length), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            length = ntohs(length);
            
            uint8_t bytes[length];
            if (fread(&bytes, sizeof(bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }

            CONSTANT_Utf8_info *entry = malloc(sizeof(CONSTANT_Utf8_info) + length);
            entry->tag = tag;
            entry->length = length;
            memcpy(entry->bytes, bytes, length);

            return entry;    
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


int read_classfile (ClassFile *cf, FILE *file) {
    // Read 'magic'
    if (fread(&cf->magic, sizeof(cf->magic), 1, file) != 1) {
        perror("Erro ao ler 'magic'");
        return 1;
    }
    cf->magic = ntohl(cf->magic);


    // Read 'minor_version'
    if (fread(&cf->minor_version, sizeof(cf->minor_version), 1, file) != 1) {
        perror("Erro ao ler 'minor_version'");
        return 1;
    }
    cf->minor_version = ntohs(cf->minor_version);


    // Read 'major_version'
    if (fread(&cf->major_version, sizeof(cf->major_version), 1, file) != 1) {
        perror("Erro ao ler 'major_version'");
        return 1;
    }
    cf->major_version = ntohs(cf->major_version);


    // CONSTANT_POOL_COUNT
    if (fread(&cf->constant_pool_count, sizeof(cf->constant_pool_count), 1, file) != 1) {
        perror("Erro ao ler 'constant_pool_count'");
        return 1;
    }
    cf->constant_pool_count = ntohs(cf->constant_pool_count);
    printf("constant_pool_count: 0x%04X\n", cf->constant_pool_count);
    
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

        cf->constant_pool[i] = entry;

        if (tag == CONSTANT_Long || tag == CONSTANT_Double) i++; 
    }

    return 0;
}