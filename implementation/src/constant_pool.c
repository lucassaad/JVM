#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "constant_pool.h"

void* constant_pool_reader(cp_tags tag, FILE *file) {
    switch (tag) {
        case CONSTANT_Class: {
                    
            CONSTANT_Class_info *entry = malloc(sizeof(CONSTANT_Class_info));
            entry->tag = tag;

            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = byteswap_u2(entry->name_index);

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
            entry->class_index = byteswap_u2(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = byteswap_u2(entry->name_and_type_index);

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
            entry->class_index = byteswap_u2(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = byteswap_u2(entry->name_and_type_index);

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
            entry->class_index = byteswap_u2(entry->class_index);
            
            // Read name_and_type_index:
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = byteswap_u2(entry->name_and_type_index);
    
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
            entry->string_index = byteswap_u2(entry->string_index);
            
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
            entry->bytes = byteswap_u4(entry->bytes);
            
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
            entry->bytes = byteswap_u4(entry->bytes);
            
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
            entry->high_bytes = byteswap_u4(entry->high_bytes);
            
            if (fread(&entry->low_bytes, sizeof(entry->low_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->low_bytes = byteswap_u4(entry->low_bytes);

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
            entry->high_bytes = byteswap_u4(entry->high_bytes);
            
            if (fread(&entry->low_bytes, sizeof(entry->low_bytes), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->low_bytes = byteswap_u4(entry->low_bytes);
            
            return entry;
        }

        case CONSTANT_NameAndType: {
            CONSTANT_NameAndType_info *entry = malloc(sizeof(CONSTANT_NameAndType_info));
            entry->tag = tag;
            
            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = byteswap_u2(entry->name_index);
            
            if (fread(&entry->descriptor_index, sizeof(entry->descriptor_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->descriptor_index = byteswap_u2(entry->descriptor_index);
            
            return entry;
        }
        
        case CONSTANT_Utf8: {
            uint16_t length;
            if (fread(&length, sizeof(length), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            length = byteswap_u2(length);
            
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
            entry->reference_index = byteswap_u2(entry->reference_index);

            return entry;
        }
        
        case CONSTANT_MethodType: {
            CONSTANT_MethodType_info *entry = malloc(sizeof(CONSTANT_MethodType_info));
            entry->tag = tag;
            
            if (fread(&entry->descriptor_index, sizeof(entry->descriptor_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->descriptor_index = byteswap_u2(entry->descriptor_index);

            return entry;
        }
        
        case CONSTANT_Dynamic: {
            CONSTANT_Dynamic_info *entry = malloc(sizeof(CONSTANT_Dynamic_info));
            entry->tag = tag;
            
            if (fread(&entry->bootstrap_method_attr_index, sizeof(entry->bootstrap_method_attr_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bootstrap_method_attr_index = byteswap_u2(entry->bootstrap_method_attr_index);
            
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = byteswap_u2(entry->name_and_type_index);

            return entry;
        }

        case CONSTANT_InvokeDynamic: {
            CONSTANT_InvokeDynamic_info *entry = malloc(sizeof(CONSTANT_InvokeDynamic_info));
            entry->tag = tag;
            
            if (fread(&entry->bootstrap_method_attr_index, sizeof(entry->bootstrap_method_attr_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->bootstrap_method_attr_index = byteswap_u2(entry->bootstrap_method_attr_index);
            
            if (fread(&entry->name_and_type_index, sizeof(entry->name_and_type_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_and_type_index = byteswap_u2(entry->name_and_type_index);

            return entry;
        }

        case CONSTANT_Package: {
            CONSTANT_Package_info *entry = malloc(sizeof(CONSTANT_Package_info));
            entry->tag = tag;
            
            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = byteswap_u2(entry->name_index);
            
            return entry;
        }

        case CONSTANT_Module: {
            CONSTANT_Module_info *entry = malloc(sizeof(CONSTANT_Module_info));
            entry->tag = tag;

            if (fread(&entry->name_index, sizeof(entry->name_index), 1, file) != 1) {
                perror("Error while reading constant_pool");
                return NULL;
            }
            entry->name_index = byteswap_u2(entry->name_index);

            return entry;
        }

        default:
            return NULL;
    }
}


