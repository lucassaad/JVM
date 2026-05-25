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

// Traduz as máscaras de bits das Access Flags de um FIELD para texto descritivo
void print_field_access_flags(uint16_t flags) {
    printf("0x%04X [ ", flags);
    if (flags & 0x0001) printf("public ");
    if (flags & 0x0002) printf("private ");
    if (flags & 0x0004) printf("protected ");
    if (flags & 0x0008) printf("static ");
    if (flags & 0x0010) printf("final ");
    if (flags & 0x0040) printf("volatile ");
    if (flags & 0x0080) printf("transient ");
    if (flags & 0x1000) printf("synthetic ");
    if (flags & 0x4000) printf("enum ");
    printf("]");
}

void print_method_access_flags(uint16_t flags) {
    printf("0x%04X [ ", flags);
    if (flags & 0x0001) printf("public ");
    if (flags & 0x0002) printf("private ");
    if (flags & 0x0004) printf("protected ");
    if (flags & 0x0008) printf("static ");
    if (flags & 0x0010) printf("final ");
    if (flags & 0x0020) printf("synchronized ");
    if (flags & 0x0040) printf("bridge ");
    if (flags & 0x0080) printf("varargs ");
    if (flags & 0x0100) printf("native ");
    if (flags & 0x0400) printf("abstract ");
    if (flags & 0x0800) printf("strictfp ");
    if (flags & 0x1000) printf("synthetic ");
    printf("]");
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
    
    // Implementar demais campos
    printf("=======================================================\n");
}

void print_class_attributes(ClassFile *cf)
{
    printf("\n===== CLASS ATTRIBUTES =====\n");

    for (int i = 0; i < cf->attributes_count; i++) {

        attribute_info *attr =
            &cf->attributes[i];

        uint16_t name_index =
            attr->attribute_name_index;

        cp_info *cp =
            cf->constant_pool[name_index];

        if (cp == NULL)
            continue;

        if (cp->tag != CONSTANT_Utf8)
            continue;

        CONSTANT_Utf8_info *utf8 =
            (CONSTANT_Utf8_info*) cp->info;

        printf("\nAttribute %d\n", i);

        printf(
            "Name: %.*s\n",
            utf8->length,
            utf8->bytes
        );

        printf(
            "Length: %u\n",
            attr->attribute_length
        );
    }
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
                CONSTANT_Utf8_info *info = (CONSTANT_Utf8_info *)entry->info;

                printf("[%02d] CONSTANT_Utf8_info\n\tLength of byte array: %d\n\tLength of string: %d\n\tString: <%.*s>\n", 
                       i, info->length, info->length, info->length, info->bytes);
                break;
            }
            case CONSTANT_Class: {
                CONSTANT_Class_info *info = (CONSTANT_Class_info *)entry->info;
                // Resolve a referência para mostrar o nome real
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;

                printf("[%02d] CONSTANT_Class_info\n\tClass Name: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Methodref: {
                CONSTANT_Methodref_info *info = (CONSTANT_Methodref_info *)entry->info;

                // Resolve onde está a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index]->info;
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index]->info;

                // Resolve onde está o nome, o tipo e o descriptor do método
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index]->info;
                CONSTANT_Utf8_info *nt_desc = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->descriptor_index]->info;

                printf("[%02d] CONSTANT_Methodref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->class_index, class_name->length, class_name->bytes, info->name_and_type_index, nt_name->length, nt_name->bytes, nt_desc->length, nt_desc->bytes);
                break;
            }
            case CONSTANT_Fieldref: {
                CONSTANT_Fieldref_info *info = (CONSTANT_Fieldref_info *)entry->info;
                // Resolve a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index]->info;
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index]->info;

                // Resolve o nome, tipo e descriptor
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index]->info;
                CONSTANT_Utf8_info *nt_desc = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->descriptor_index]->info;

                printf("[%02d] CONSTANT_Fieldref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->class_index, class_name->length, class_name->bytes, info->name_and_type_index, nt_name->length, nt_name->bytes, nt_desc->length, nt_desc->bytes);
                break;
            }
            case CONSTANT_String: {
                CONSTANT_String_info *info = (CONSTANT_String_info *)entry->info;
                CONSTANT_Utf8_info *str = (CONSTANT_Utf8_info *)cf->constant_pool[info->string_index]->info;

                printf("[%02d] CONSTANT_String_info\n\tcp_info #%d <%.*s>\n", i, info->string_index, str->length, str->bytes);
                break;
            }
            case CONSTANT_NameAndType: {
                CONSTANT_NameAndType_info *info = (CONSTANT_NameAndType_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index]->info;
                
                printf("[%02d] CONSTANT_NameAndType_info\n\tName: cp_info #%d <%.*s>\n\tDescriptor: cp_info #%d <%.*s>\n", 
                       i, info->name_index, name->length, name->bytes, info->descriptor_index, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_Integer: {
                CONSTANT_Integer_info *info = (CONSTANT_Integer_info *)entry->info;

                printf("[%02d] CONSTANT_Integer_info\n\tBytes: 0x%08X\n\tInteger: %d\n", 
                       i, info->bytes, info->bytes);
                break;
            }
            case CONSTANT_Float: {
                CONSTANT_Float_info *info = (CONSTANT_Float_info *)entry->info;

                printf("[%02d] CONSTANT_Float_info\n\tBytes: 0x%08X\n\tFloat: %f\n", 
                       i, info->bytes, *(float *)&info->bytes);
                break;
            }
            case CONSTANT_Long: {
                CONSTANT_Long_info *info = (CONSTANT_Long_info *)entry->info;
                long long valor = ((long long)info->high_bytes << 32) | info->low_bytes;

                printf("[%02d] CONSTANT_Long_info\n\tHigh bytes: 0x%08X\n\tLow bytes: 0x%08X\n\tLong: %lld\n", 
                       i, info->high_bytes, info->low_bytes, valor);
                printf("[%02d] (large numeric continued)\n", i + 1);
                i++; // Long ocupa dois slots
                break;
            }
            case CONSTANT_Double: {
                CONSTANT_Double_info *info = (CONSTANT_Double_info *)entry->info;
                long long bits = ((long long)info->high_bytes << 32) | info->low_bytes;

                printf("[%02d] CONSTANT_Double_info\n\tHigh bytes: 0x%08X\n\tLow bytes: 0x%08X\n\tDouble: %f\n", 
                       i, info->high_bytes, info->low_bytes, *(double *)&bits);
                printf("[%02d] (large numeric continued)\n", i + 1);
                i++; // Double ocupa dois slots
                break;
            }
            case CONSTANT_InterfaceMethodref: {
                CONSTANT_InterfaceMethodref_info *info = (CONSTANT_InterfaceMethodref_info *)entry->info;
                // Resolve a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index]->info;
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index]->info;
                
                // Resolve o nome, tipo e descriptor
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index]->info;
                CONSTANT_Utf8_info *nt_desc = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->descriptor_index]->info;

                printf("[%02d] CONSTANT_InterfaceMethodref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->class_index, class_name->length, class_name->bytes, info->name_and_type_index, nt_name->length, nt_name->bytes, nt_desc->length, nt_desc->bytes);
                break;
            }
            case CONSTANT_MethodHandle: {
                CONSTANT_MethodHandle_info *info = (CONSTANT_MethodHandle_info *)entry->info;

                printf("[%02d] CONSTANT_MethodHandle_info\n\tReference kind: %d\n\tReference index: cp_info #%d\n", 
                       i, info->reference_kind, info->reference_index);
                break;
            }
            case CONSTANT_MethodType: {
                CONSTANT_MethodType_info *info = (CONSTANT_MethodType_info *)entry->info;
                // Resolve o descriptor que aponta para um Utf8
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index]->info;

                printf("[%02d] CONSTANT_MethodType_info\n\tDescriptor: cp_info #%d <%.*s>\n", 
                       i, info->descriptor_index, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_InvokeDynamic: {
                CONSTANT_InvokeDynamic_info *info = (CONSTANT_InvokeDynamic_info *)entry->info;
                // Resolve o NameAndType associado
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index]->info;
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index]->info;
                
                printf("[%02d] CONSTANT_InvokeDynamic_info\n\tBootstrap method attr: %d\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Dynamic: {
                CONSTANT_Dynamic_info *info = (CONSTANT_Dynamic_info *)entry->info;
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;

                // Busca o nome e o tipo
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index]->info;
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index]->info;

                printf("[%02d] CONSTANT_Dynamic_info\n\tBootstrap method attr: %d\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Module: {
                CONSTANT_Module_info *info = (CONSTANT_Module_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                printf("[%02d] CONSTANT_Module_info\n\tName: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Package: {
                CONSTANT_Package_info *info = (CONSTANT_Package_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                printf("[%02d] CONSTANT_Package_info\n\tName: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            default:
                printf("[%02d] (tag %d não implementada)\n", i, entry->tag);
                break;
        }
    }
}

void print_fields(ClassFile *cf) {
    printf("\n=== Fields (%d entradas) ===\n", cf->fields_count);

    if (cf->fields_count == 0) {
        printf("  (nenhum field)\n");
        return;
    }

    // O vetor de fields começa no índice 0
    for (int i = 0; i < cf->fields_count; i++) {
        
        field_info *field = &cf->fields[i];

        CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[field->name_index]->info;
        CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[field->descriptor_index]->info;

        // Título do Field
        printf("[%d] %.*s\n", i, name->length, name->bytes);
        
        // Detalhes
        printf("    Name:           cp_info #%d <%.*s>\n", field->name_index, name->length, name->bytes);
        printf("    Descriptor:     cp_info #%d <%.*s>\n", field->descriptor_index, desc->length, desc->bytes);

        printf("    Access flags:   ");
        print_field_access_flags(field->access_flags);
        printf("\n");
        printf("    Attributes count: %d\n", field->attributes_count);

        for (int j = 0; j < field->attributes_count; j++) {
            attribute_info *attr = &field->attributes[j];

            // Busca o nome do atributo na Constant Pool
            CONSTANT_Utf8_info *attr_name = (CONSTANT_Utf8_info *)cf->constant_pool[attr->attribute_name_index]->info;

            // Imprime cada atributo
            printf("       -> Attribute [%d]: %.*s\n", j, attr_name->length, attr_name->bytes);
            printf("           Attribute name index: cp_info #%d <%.*s>\n", attr->attribute_name_index, attr_name->length, attr_name->bytes);
            printf("           Attribute length:     %u\n", attr->attribute_length);
            printf("\n");
        }
        
        printf("\n"); // Pula uma linha no final para separar o próximo field
    }
}

void print_methods(ClassFile *cf) {
    printf("\n=== Methods (%d entradas) ===\n", cf->methods_count);

    if (cf->methods_count == 0) {
        printf("  (nenhum method)\n");
        return;
    }

    // O vetor de methods começa no índice 0
    for (int i = 0; i < cf->methods_count; i++) {
        
        method_info *method = &cf->methods[i];

        CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[method->name_index]->info;
        CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[method->descriptor_index]->info;

        // Título do Method
        printf("[%d] %.*s\n", i, name->length, name->bytes);
        
        // Detalhes
        printf("    Name:           cp_info #%d <%.*s>\n", method->name_index, name->length, name->bytes);
        printf("    Descriptor:     cp_info #%d <%.*s>\n", method->descriptor_index, desc->length, desc->bytes);

        printf("    Access flags:   ");
        print_method_access_flags(method->access_flags);
        printf("\n");

        printf("    Attributes count: %d\n", method->attributes_count);

        for (int j = 0; j < method->attributes_count; j++) {
            attribute_info *attr = &method->attributes[j];

            // Busca o nome do atributo na Constant Pool
            CONSTANT_Utf8_info *attr_name = (CONSTANT_Utf8_info *)cf->constant_pool[attr->attribute_name_index]->info;

            // Imprime o sumário de cada atributo
            printf("       -> Attribute [%d]: %.*s\n", j, attr_name->length, attr_name->bytes);
            printf("           Attribute name index: cp_info #%d <%.*s>\n", attr->attribute_name_index, attr_name->length, attr_name->bytes);
            printf("           Attribute length:     %u\n", attr->attribute_length);
            printf("\n");
        }
        
        printf("\n"); // Pula uma linha no final para separar o próximo method
    }
}