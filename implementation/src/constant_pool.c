#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "constant_pool.h"
#include "class_file.h"

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


int check_constant_pool_references(ClassFile *cf) {
    for (int i = 1; i < cf->constant_pool_count; i++) {
        // Pega o elemento na posicao [i] 
        cp_info *entry = (cp_info *) cf->constant_pool[i];
        if (entry == NULL) continue;

        cp_tags tag = entry->tag;
        switch (tag) 
        {
            case CONSTANT_Class: {
                CONSTANT_Class_info *info = (CONSTANT_Class_info *) entry->info;
            // Valida 'name_index'
                // 0 < name_index <= constant_pool_count
                if (info->name_index < 1 || info->name_index > cf->constant_pool_count) {
                    perror("'name_index' da CONSTANT_Class_info faz referencia a um indice invalido");
                    return 1;
                }
                // constant_pool[name_index] -> CONSTANT_Utf8_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->name_index];
                if (referenced_entry->tag != CONSTANT_Utf8) {
                    perror("'name_index' da CONSTANT_Class_info não faz referencia a CONTANT_Utf8");
                    return 1;
                }
                break;
            }

            case CONSTANT_Fieldref: {
                CONSTANT_Fieldref_info *info = (CONSTANT_Fieldref_info *) entry->info;
        
            // Valida 'class_index': mey be either a class or inteface type
                // 0 < class_index < constant_pool_count
                if (info->class_index < 1 || info->class_index > cf->constant_pool_count) {
                    perror("'class_index' da CONSTANT_Fieldref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[class_index] -> CONSTANT_Class_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->class_index];
                if (referenced_entry->tag != CONSTANT_Class) {
                    perror("'class_index' da CONSTANT_Fieldref_info não faz referencia a CONSTANT_Class");
                    return 1;
                }
            // Valida 'name_and_type_index': descriptor must be a field descriptor 
                // 0 < name_and_type_index < constant_pool_count
                if (info->name_and_type_index < 1 || info->name_and_type_index > cf->constant_pool_count) {
                    perror("'name_and_type_index' da CONSTANT_Fieldref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[name_and_type_index] -> CONSTANT_Class_info
                referenced_entry = (cp_info *) cf->constant_pool[info->name_and_type_index];
                if (referenced_entry->tag != CONSTANT_NameAndType) {
                    perror("'name_and_type_index' da CONSTANT_Fieldref_info não faz referencia a CONSTANT_NameAndType");
                    return 1;
                }
                break;
            }

            case CONSTANT_Methodref: {
                CONSTANT_Methodref_info *info = (CONSTANT_Methodref_info *) entry->info;
        
            // Valida 'class_index': should be a class
                // 0 < class_index < constant_pool_count
                if (info->class_index < 1 || info->class_index > cf->constant_pool_count) {
                    perror("'class_index' da CONSTANT_Methodref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[class_index] -> CONSTANT_Class_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->class_index];
                if (referenced_entry->tag != CONSTANT_Class) {
                    perror("'class_index' da CONSTANT_Methodref_info não faz referencia a CONSTANT_Class");
                    return 1;
                }
            // Valida 'name_and_type_index': descriptor must be a method descriptor 
                // 0 < name_and_type_index < constant_pool_count
                if (info->name_and_type_index < 1 || info->name_and_type_index > cf->constant_pool_count) {
                    perror("'name_and_type_index' da CONSTANT_Methodref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[name_and_type_index] -> CONSTANT_Class_info
                referenced_entry = (cp_info *) cf->constant_pool[info->name_and_type_index];
                if (referenced_entry->tag != CONSTANT_NameAndType) {
                    perror("'name_and_type_index' da CONSTANT_Methodref_info não faz referencia a CONSTANT_NameAndType");
                    return 1;
                }
                break;
            }

            case CONSTANT_InterfaceMethodref: {
                CONSTANT_InterfaceMethodref_info *info = (CONSTANT_InterfaceMethodref_info *) entry->info;
        
            // Valida 'class_index': should be an interface
                // 0 < class_index < constant_pool_count
                if (info->class_index < 1 || info->class_index > cf->constant_pool_count) {
                    perror("'class_index' da CONSTANT_InterfaceMethodref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[class_index] -> CONSTANT_Class_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->class_index];
                if (referenced_entry->tag != CONSTANT_Class) {
                    perror("'class_index' da CONSTANT_InterfaceMethodref_info não faz referencia a CONSTANT_Class");
                    return 1;
                }
            // Valida 'name_and_type_index'
                // 0 < name_and_type_index < constant_pool_count
                if (info->name_and_type_index < 1 || info->name_and_type_index > cf->constant_pool_count) {
                    perror("'name_and_type_index' da CONSTANT_InterfaceMethodref_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[name_and_type_index] -> CONSTANT_Class_info
                referenced_entry = (cp_info *) cf->constant_pool[info->name_and_type_index];
                if (referenced_entry->tag != CONSTANT_NameAndType) {
                    perror("'name_and_type_index' da CONSTANT_InterfaceMethodref_info não faz referencia a CONSTANT_NameAndType");
                    return 1;
                }
                break;
            }

            case CONSTANT_String: {
                CONSTANT_String_info *info = (CONSTANT_String_info *) entry->info;
        
            // Valida 'string_index': should be an interface
                // 0 < string_index < constant_pool_count
                if (info->string_index < 1 || info->string_index > cf->constant_pool_count) {
                    perror("'string_index' da CONSTANT_String_info faz referencia a um indice invalido");
                    return 1;
                }            
                // constant_pool[string_index] -> CONSTANT_Utf8_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->string_index];
                if (referenced_entry->tag != CONSTANT_Utf8) {
                    perror("'string_index' da CONSTANT_String_info não faz referencia a CONSTANT_Utf8");
                    return 1;
                }
                break;
            }

            // nao referenciam outra entrada
            case CONSTANT_Integer: break;
            case CONSTANT_Float: break;
            case CONSTANT_Long: break;
            case CONSTANT_Double: break;
            
            case CONSTANT_NameAndType:{
                CONSTANT_NameAndType_info *info = (CONSTANT_NameAndType_info *) entry;
            // Valida 'name_index'
                // 0 < name_index <= constant_pool_count
                if (info->name_index < 1 || info->name_index > cf->constant_pool_count) {
                    perror("'name_index' da CONSTANT_NameAndType faz referencia a um indice invalido");
                    return 1;
                }
                // constant_pool[name_index] -> CONSTANT_Utf8_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->name_index];
                if (referenced_entry->tag != CONSTANT_Utf8) {
                    perror("'name_index' da CONSTANT_NameAndType não faz referencia a CONTANT_Utf8");
                    return 1;
                }
                // Valida 'descriptor_index'
                // 0 < descriptor_index <= constant_pool_count
                if (info->descriptor_index < 1 || info->descriptor_index > cf->constant_pool_count) {
                    perror("'descriptor_index' da CONSTANT_NameAndType faz referencia a um indice invalido");
                    return 1;
                }
                // constant_pool[descriptor_index] -> CONSTANT_Utf8_info
                referenced_entry = (cp_info *) cf->constant_pool[info->descriptor_index];
                if (referenced_entry->tag != CONSTANT_Utf8) {
                    perror("'descriptor_index' da CONSTANT_NameAndType não faz referencia a CONTANT_Utf8");
                    return 1;
                }
                break;
            }
            
            case CONSTANT_Utf8: {
                // valida 'bytes'
                // no byte may have value 0 and may not lie in the range 0xf0 - 0xff
                CONSTANT_Utf8_info *info = (CONSTANT_Utf8_info *) entry;
                for (int j = 0; j < info->length; j++) {
                    if (info->bytes[j] == 0) {
                        perror("byte com o valor 0");
                        return 1;
                    }
                    if(info->bytes[j] >= 0xf0) {
                        perror("byte na faixa 0xf0 - 0x00");
                        return 1;
                    }
                }
                break;
            }
            
            case CONSTANT_MethodHandle: {
                CONSTANT_MethodHandle_info *info = (CONSTANT_MethodHandle_info *) entry;
            // valida 'reference_kind'
                if (info->reference_kind < 1 || info->reference_kind > 9) {
                        perror("valor invalido de 'reference_kind'");
                        return 1;
                    }
            // valida 'reference_index'
                // 0 < reference_index <= constant_pool_count
                if (info->reference_index < 1 || info->reference_index > cf->constant_pool_count) {
                    perror("'reference_index' da CONSTANT_MethodHandle_info faz referencia a um indice invalido");
                    return 1;
                }
                switch (info->reference_index) {
                    case 1:     // REF_getField
                    case 2:     // REF_getStatic
                    case 3:     // REF_putField
                    case 4: {   // REF_putStatic
                        cp_info *referenced_info = (cp_info *) cf->constant_pool[info->reference_index];
                        if (referenced_info->tag != CONSTANT_Fieldref) {
                            perror("'reference_index' da CONSTANT_MethodHandle nao faz referencia a CONSTANT_Fieldref");
                            return 1;
                        }
                        break;
                    }
                    case 5:     // REF_invokeVirtual
                    case 8: {   // REF_newInvokeSpecial
                        cp_info *referenced_info = (cp_info *) cf->constant_pool[info->reference_index];
                        if (referenced_info->tag != CONSTANT_Methodref) {
                            perror("'reference_index' da CONSTANT_MethodHandle nao faz referencia a CONSTANT_Methodref");
                            return 1;
                        }
                        break;
                    }
                    case 6:     // REF_invokeStatic
                    case 7: {   // REF_invokeSpecial
                        // Version less than 52.0
                        cp_info *referenced_info = (cp_info *) cf->constant_pool[info->reference_index];
                        if (cf->major_version < 52) {
                            if (referenced_info->tag != CONSTANT_Methodref) {
                                perror("'reference_index' da CONSTANT_MethodHandle nao faz referencia a CONSTANT_Methodref");
                                return 1;
                            }
                        }
                        else {
                            if (referenced_info->tag != CONSTANT_Methodref && referenced_info->tag != CONSTANT_InterfaceMethodref) {
                                perror("'reference_index' da CONSTANT_MethodHandle nao faz referencia a classe certa");
                                return 1;
                            }
                        }
                        break;
                    }
                    case 9: {   // REF_invokeInterface
                        cp_info *referenced_info = (cp_info *) cf->constant_pool[info->reference_index];
                        if (referenced_info->tag != CONSTANT_InterfaceMethodref) {
                                perror("'reference_index' da CONSTANT_MethodHandle nao faz referencia CONSTANT_InterfaceMethodref");
                                return 1;
                            }
                        break;
                    }
                } 
                break;      
            }

            case CONSTANT_MethodType: {
                CONSTANT_MethodType_info *info = (CONSTANT_MethodType_info *) entry;
            // valida 'descriptor_index'
                // 0 < descriptor_index <= constant_pool_count
                if (info->descriptor_index < 1 || info->descriptor_index > cf->constant_pool_count) {
                    perror("'descriptor_index' da CONSTANT_MethodType faz referencia a um indice invalido");
                    return 1;
                }
                // constant_pool[descriptor_index] -> CONSTANT_Utf8_info
                cp_info *referenced_entry = (cp_info *) cf->constant_pool[info->descriptor_index];
                if (referenced_entry->tag != CONSTANT_Utf8) {
                    perror("'descriptor_index' da CONSTANT_MethodType não faz referencia a CONTANT_Utf8");
                    return 1;
                }
                break;
            }

            case CONSTANT_Dynamic:
            case CONSTANT_InvokeDynamic:
                // todo
                break;
            case CONSTANT_Module:
                //todo
                break;
            case CONSTANT_Package:
                // todo    
                break;
        }  

    }
    return 0;
}
