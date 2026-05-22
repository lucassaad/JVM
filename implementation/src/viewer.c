#include <stdio.h>
#include "viewer.h"
#include "class_file.h"
#include "constant_pool.h"

// Traduz a versão Major
const char* get_java_version_string(uint16_t major_version) {
    switch (major_version) {
        case 45: return "1.1";
        case 46: return "1.2";
        case 47: return "1.3";
        case 48: return "1.4";
        case 49: return "Java 5";
        case 50: return "Java 6";
        case 51: return "Java 7";
        case 52: return "Java 8";
        case 53: return "Java 9";
        case 54: return "Java 10";
        case 55: return "Java 11";
        case 56: return "Java 12";
        case 57: return "Java 13";
        case 58: return "Java 14";
        case 59: return "Java 15";
        case 60: return "Java 16";
        case 61: return "Java 17";
        case 62: return "Java 18";
        case 63: return "Java 19";
        case 64: return "Java 20";
        case 65: return "Java 21";
        case 66: return "Java 22";
        case 67: return "Java 23";
        case 68: return "Java 24";
        case 69: return "Java 25";
        default: return "Versão Desconhecida";
    }
}

void print_utf8_info(void *entry_void) {
    CONSTANT_Utf8_info *entry = (CONSTANT_Utf8_info *) entry_void;
    printf("tag    :  %d\n", entry->tag);
    printf("length :  %d\n", entry->length);
    printf("string :  %.*s\n", entry->length, entry->bytes);

    return;
}

void print_general_information(ClassFile *cf) {
    printf("=======================================================\n");
    printf("                  Informação Geral                     \n");
    printf("=======================================================\n");
    
    printf("Magic Number          : 0x%08X\n", cf->magic);
    printf("Minor Version         : %d\n", cf->minor_version);
    printf("Major Version         : %d [%s]\n", cf->major_version, get_java_version_string(cf->major_version));
    printf("Constant Pool Count   : %d\n", cf->constant_pool_count);
    printf("Access flags          : 0x%04x\n", cf->access_flags);
    printf("This Class            : cp_info #%d\n", cf->this_class);
    printf("Super Class           : cp_info #%d\n", cf->super_class);
    printf("Interfaces Count      : %d\n", cf->interfaces_count);
    printf("Fields Count          : %d\n", cf->fields_count);
    printf("Methods Count         : %d\n", cf->methods_count);

    // Display 'constant_utf8_info'
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *constant_info = cf->constant_pool[i];
        if (constant_info->tag == 1) {
            printf("CONSTANT_Utf8_info - constant_pool[%d]\n", i);
            print_utf8_info(constant_info->info);
        }
        if (constant_info->tag == 5 || constant_info->tag == 6) i++;    
    }
    
    // Implementar demais campos
    printf("=======================================================\n");
}

void print_constant_pool(ClassFile *cf) {
    printf("=== Constant Pool (%d entradas) ===\n", cf->constant_pool_count - 1);

    for (int i = 1; i < cf->constant_pool_count; i++) {
        // Pega o elemento atual
        cp_info *entry = cf->constant_pool[i];

        // Se for NULL, pula
        if (entry == NULL) continue;

        switch (entry->tag) {
            case CONSTANT_Utf8: {
                CONSTANT_Utf8_info *info = (CONSTANT_Utf8_info *)entry;

                printf("  #%d = Utf8\t\t%.*s\n", i, info->length, info->bytes);
                break;
            }
            case CONSTANT_Class: {
                CONSTANT_Class_info *info = (CONSTANT_Class_info *)entry;
                // Resolve a referência para mostrar o nome real
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index];

                printf("  #%d = Class\t\t#%d\t// %.*s\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Methodref: {
                CONSTANT_Methodref_info *info = (CONSTANT_Methodref_info *)entry;

                // Resolve onde está a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index];
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index];

                // Resolve onde está o nome e o tipo do método
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index];
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index];

                printf("  #%d = Methodref\t\t#%d.#%d\t\t// %.*s.%.*s\n", i, info->class_index, info->name_and_type_index, class_name->length, class_name->bytes, nt_name->length, nt_name->bytes);
                break;
            }
            case CONSTANT_Fieldref: {
                CONSTANT_Fieldref_info *info = (CONSTANT_Fieldref_info *)entry;
                // Resolve a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index];
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index];

                // Resolve o nome e tipo
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index];
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index];

                printf("  #%d = Fieldref\t\t#%d.#%d\t\t// %.*s.%.*s\n", i, info->class_index, info->name_and_type_index, class_name->length, class_name->bytes, nt_name->length, nt_name->bytes);
                break;
            }
            case CONSTANT_String: {
                CONSTANT_String_info *info = (CONSTANT_String_info *)entry;
                CONSTANT_Utf8_info *str = (CONSTANT_Utf8_info *)cf->constant_pool[info->string_index];

                printf("  #%d = String\t\t#%d\t// %.*s\n", i, info->string_index, str->length, str->bytes);
                break;
            }
            case CONSTANT_NameAndType: {
                CONSTANT_NameAndType_info *info = (CONSTANT_NameAndType_info *)entry;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index];
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index];
                
                printf("  #%d = NameAndType\t#%d:#%d\t\t// %.*s:%.*s\n", i, info->name_index, info->descriptor_index, name->length, name->bytes, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_Integer: {
                CONSTANT_Integer_info *info = (CONSTANT_Integer_info *)entry;

                printf("  #%d = Integer\t\t%d\n", i, info->bytes);
                break;
            }
            case CONSTANT_Float: {
                CONSTANT_Float_info *info = (CONSTANT_Float_info *)entry;

                printf("  #%d = Float\t\t%f\n", i, *(float *)&info->bytes);
                break;
            }
            case CONSTANT_Long: {
                CONSTANT_Long_info *info = (CONSTANT_Long_info *)entry;
                long long valor = ((long long)info->high_bytes << 32) | info->low_bytes;

                printf("  #%d = Long\t\t%lld\n", i, valor);
                i++; // Long ocupa dois slots
                break;
            }
            case CONSTANT_Double: {
                CONSTANT_Double_info *info = (CONSTANT_Double_info *)entry;
                long long bits = ((long long)info->high_bytes << 32) | info->low_bytes;

                printf("  #%d = Double\t\t%f\n", i, *(double *)&bits);
                i++; // Double ocupa dois slots
                break;
            }
            case CONSTANT_InterfaceMethodref: {
                CONSTANT_InterfaceMethodref_info *info = (CONSTANT_InterfaceMethodref_info *)entry;
                // Resolve a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index];
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index];
                
                // Resolve o nome e tipo
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index];
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index];

                printf("  #%d = InterfaceMethodref\t#%d.#%d\t\t// %.*s.%.*s\n", i, info->class_index, info->name_and_type_index, class_name->length, class_name->bytes, nt_name->length, nt_name->bytes);
                break;
            }
            case CONSTANT_MethodHandle: {
                CONSTANT_MethodHandle_info *info = (CONSTANT_MethodHandle_info *)entry;

                printf("  #%d = MethodHandle\tKind: %d, Ref: #%d\n", i, info->reference_kind, info->reference_index);
                break;
            }
            case CONSTANT_MethodType: {
                CONSTANT_MethodType_info *info = (CONSTANT_MethodType_info *)entry;
                // Resolve o descriptor que aponta para um Utf8
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index];

                printf("  #%d = MethodType\t#%d\t// %.*s\n", i, info->descriptor_index, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_InvokeDynamic: {
                CONSTANT_InvokeDynamic_info *info = (CONSTANT_InvokeDynamic_info *)entry;
                // Resolve o NameAndType associado
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index];
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index];
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index];
                
                printf("  #%d = InvokeDynamic\t#%d.#%d\t// %.*s:%.*s\n", i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Dynamic: {
                CONSTANT_Dynamic_info *info = (CONSTANT_Dynamic_info *)entry;
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index];

                // Busca o nome e o tipo
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index];
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index];

                printf("  #%d = Dynamic\t\t#%d.#%d\t// %.*s:%.*s\n", i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Module: {
                CONSTANT_Module_info *info = (CONSTANT_Module_info *)entry;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index];
                printf("  #%d = Module\t#%d\t// %.*s\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Package: {
                CONSTANT_Package_info *info = (CONSTANT_Package_info *)entry;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index];
                printf("  #%d = Package\t#%d\t// %.*s\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            default:
                printf("  #%d = (tag %d não implementada)\n", i, entry->tag);
                break;
        }
    }
}

void print_fields(ClassFile *cf) {
    printf("=== Fields (%d entradas) ===\n", cf->fields_count);

    if (cf->fields_count == 0) {
        printf("  (nenhum field)\n");
        return;
    }

    // O vetor de fields começa no índice 0
    for (int i = 0; i < cf->fields_count; i++) {
        
        // Pega o field atual do laço
        field_info *field = &cf->fields[i];

        // Vai na Constant Pool buscar o Nome da variável
        CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[field->name_index];
        
        // Vai na Constant Pool buscar o Tipo (Descriptor) da variável
        CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[field->descriptor_index];

        // Exibe de forma formatada e limpa
        printf("[%02d] Nome: %.*s\n", i, name->length, name->bytes);
        printf("     Tipo: %.*s\n", desc->length, desc->bytes);
        printf("     Flags de Acesso: 0x%04X\n", field->access_flags);
        printf("     Atributos: %d\n\n", field->attributes_count);
    }
}