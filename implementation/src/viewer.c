#include <stdio.h>
#include "viewer.h"
#include "class_file.h"
#include "constant_pool.h"
#include "instruction_viewer.h"
#include <string.h>

// Traduz a versão Major
const char* get_java_version_string(uint16_t major_version) {
    switch (major_version) {
        case 45: return "Java 1.1";
        case 46: return "Java 1.2";
        case 47: return "Java 1.3";
        case 48: return "Java 1.4";
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
        case 70: return "Java 26";
        default: return "Versão Desconhecida";
    }
}
const char* get_class_access_flags_string(uint16_t flags) {
    static char buffer[128];
    buffer[0] = '\0'; 

    if (flags & 0x0001) strcat(buffer, "public ");
    if (flags & 0x0010) strcat(buffer, "final ");
    if (flags & 0x0020) strcat(buffer, "super ");
    if (flags & 0x0200) strcat(buffer, "interface ");
    if (flags & 0x0400) strcat(buffer, "abstract ");
    if (flags & 0x1000) strcat(buffer, "synthetic ");
    if (flags & 0x2000) strcat(buffer, "annotation ");
    if (flags & 0x4000) strcat(buffer, "enum ");
    if (flags & 0x8000) strcat(buffer, "module ");

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ' ') {
        buffer[len - 1] = '\0';
    }

    return buffer;
}

const char* get_class_name_string(ClassFile *cf, uint16_t class_index) {
    if (class_index == 0 || class_index >= cf->constant_pool_count) {
        return "Nenhum";
    }

    cp_info *cp_entry = cf->constant_pool[class_index];
    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cp_entry->info;

    uint16_t name_index = class_info->name_index;
    cp_info *utf8_entry = cf->constant_pool[name_index];
    CONSTANT_Utf8_info *utf8_info = (CONSTANT_Utf8_info *)utf8_entry->info;

    static char name_buffer[512];
    
    uint16_t len = utf8_info->length;
    if (len > 511) len = 511;

    memcpy(name_buffer, utf8_info->bytes, len);
    name_buffer[len] = '\0';

    return name_buffer;
}

// Traduz as mascaras de bits das Access Flags de um FIELD para texto descritivo
void print_field_access_flags(FILE *out, uint16_t flags) {
    fprintf(out,"0x%04X [ ", flags);
    if (flags & 0x0001) fprintf(out,"public ");
    if (flags & 0x0002) fprintf(out,"private ");
    if (flags & 0x0004) fprintf(out,"protected ");
    if (flags & 0x0008) fprintf(out,"static ");
    if (flags & 0x0010) fprintf(out,"final ");
    if (flags & 0x0040) fprintf(out,"volatile ");
    if (flags & 0x0080) fprintf(out,"transient ");
    if (flags & 0x1000) fprintf(out,"synthetic ");
    if (flags & 0x4000) fprintf(out,"enum ");
    fprintf(out,"]");
}

void print_method_access_flags(FILE *out, uint16_t flags) {
    fprintf(out,"0x%04X [ ", flags);
    if (flags & 0x0001) fprintf(out,"public ");
    if (flags & 0x0002) fprintf(out,"private ");
    if (flags & 0x0004) fprintf(out,"protected ");
    if (flags & 0x0008) fprintf(out,"static ");
    if (flags & 0x0010) fprintf(out,"final ");
    if (flags & 0x0020) fprintf(out,"synchronized ");
    if (flags & 0x0040) fprintf(out,"bridge ");
    if (flags & 0x0080) fprintf(out,"varargs ");
    if (flags & 0x0100) fprintf(out,"native ");
    if (flags & 0x0400) fprintf(out,"abstract ");
    if (flags & 0x0800) fprintf(out,"strictfp ");
    if (flags & 0x1000) fprintf(out,"synthetic ");
    fprintf(out,"]");
}


void print_utf8_info(void *entry_void) {
    CONSTANT_Utf8_info *entry = (CONSTANT_Utf8_info *) entry_void;
    printf("tag    :  %d\n", entry->tag);
    printf("length :  %d\n", entry->length);
    printf("string :  %.*s\n", entry->length, entry->bytes);

    return;
}

void print_general_information(FILE *out, ClassFile *cf) {
    fprintf(out, "=======================================================\n");
    fprintf(out, "                  Informacao Geral                     \n");
    fprintf(out, "=======================================================\n");
    
    fprintf(out, "Magic Number          : 0x%08X\n", cf->magic);
    fprintf(out, "Minor Version         : %d\n", cf->minor_version);
    fprintf(out, "Major Version         : %d [%s]\n", cf->major_version, get_java_version_string(cf->major_version));
    fprintf(out, "Constant Pool Count   : %d\n", cf->constant_pool_count);
    fprintf(out, "Access flags          : 0x%04X [%s]\n", cf->access_flags, get_class_access_flags_string(cf->access_flags));
    fprintf(out, "This Class            : cp_info #%d [%s]\n", cf->this_class, get_class_name_string(cf, cf->this_class));
    fprintf(out, "Super Class           : cp_info #%d [%s]\n", cf->super_class, get_class_name_string(cf, cf->super_class));
    fprintf(out, "Interfaces Count      : %d\n", cf->interfaces_count);
    fprintf(out, "Fields Count          : %d\n", cf->fields_count);
    fprintf(out, "Methods Count         : %d\n", cf->methods_count);
    fprintf(out, "Attributes Count      : %d\n", cf->attributes_count);
}

void print_constant_pool(FILE *out, ClassFile *cf) {
    fprintf(out, "=======================================================\n");
    fprintf(out, "               CONSTANT POOL (%d Entradas)             \n", cf->constant_pool_count - 1);
    fprintf(out, "=======================================================\n");

    for (int i = 1; i < cf->constant_pool_count; i++) {
        // Pega o elemento atual
        cp_info *entry = cf->constant_pool[i];

        // Se for NULL, pula
        if (entry == NULL) continue;

        switch (entry->tag) {
            case CONSTANT_Utf8: {
                CONSTANT_Utf8_info *info = (CONSTANT_Utf8_info *)entry->info;

                fprintf(out,"[%02d] CONSTANT_Utf8_info\n\tLength of byte array: %d\n\tString: <%.*s>\n", 
                       i, info->length, info->length, info->bytes);
                break;
            }
            case CONSTANT_Class: {
                CONSTANT_Class_info *info = (CONSTANT_Class_info *)entry->info;
                // Resolve a referência para mostrar o nome real
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;

                fprintf(out,"[%02d] CONSTANT_Class_info\n\tClass Name: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Methodref: {
                CONSTANT_Methodref_info *info = (CONSTANT_Methodref_info *)entry->info;

                // Resolve onde esta a classe
                CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cf->constant_pool[info->class_index]->info;
                CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)cf->constant_pool[class_info->name_index]->info;

                // Resolve onde esta o nome, o tipo e o descriptor do metodo
                CONSTANT_NameAndType_info *nt_info = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *nt_name = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->name_index]->info;
                CONSTANT_Utf8_info *nt_desc = (CONSTANT_Utf8_info *)cf->constant_pool[nt_info->descriptor_index]->info;

                fprintf(out,"[%02d] CONSTANT_Methodref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
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

                fprintf(out,"[%02d] CONSTANT_Fieldref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->class_index, class_name->length, class_name->bytes, info->name_and_type_index, nt_name->length, nt_name->bytes, nt_desc->length, nt_desc->bytes);
                break;
            }
            case CONSTANT_String: {
                CONSTANT_String_info *info = (CONSTANT_String_info *)entry->info;
                CONSTANT_Utf8_info *str = (CONSTANT_Utf8_info *)cf->constant_pool[info->string_index]->info;

                fprintf(out,"[%02d] CONSTANT_String_info\n\tcp_info #%d <%.*s>\n", i, info->string_index, str->length, str->bytes);
                break;
            }
            case CONSTANT_NameAndType: {
                CONSTANT_NameAndType_info *info = (CONSTANT_NameAndType_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index]->info;
                
                fprintf(out,"[%02d] CONSTANT_NameAndType_info\n\tName: cp_info #%d <%.*s>\n\tDescriptor: cp_info #%d <%.*s>\n", 
                       i, info->name_index, name->length, name->bytes, info->descriptor_index, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_Integer: {
                CONSTANT_Integer_info *info = (CONSTANT_Integer_info *)entry->info;

                fprintf(out,"[%02d] CONSTANT_Integer_info\n\tBytes: 0x%08X\n\tInteger: %d\n", 
                       i, info->bytes, info->bytes);
                break;
            }
            case CONSTANT_Float: {
                CONSTANT_Float_info *info = (CONSTANT_Float_info *)entry->info;

                fprintf(out,"[%02d] CONSTANT_Float_info\n\tBytes: 0x%08X\n\tFloat: %f\n", 
                       i, info->bytes, *(float *)&info->bytes);
                break;
            }
            case CONSTANT_Long: {
                CONSTANT_Long_info *info = (CONSTANT_Long_info *)entry->info;
                long long valor = ((long long)info->high_bytes << 32) | info->low_bytes;

                fprintf(out,"[%02d] CONSTANT_Long_info\n\tHigh bytes: 0x%08X\n\tLow bytes: 0x%08X\n\tLong: %lld\n", 
                       i, info->high_bytes, info->low_bytes, valor);
                fprintf(out,"[%02d] (large numeric continued)\n", i + 1);
                i++; // Long ocupa dois slots
                break;
            }
            case CONSTANT_Double: {
                CONSTANT_Double_info *info = (CONSTANT_Double_info *)entry->info;
                long long bits = ((long long)info->high_bytes << 32) | info->low_bytes;

                fprintf(out,"[%02d] CONSTANT_Double_info\n\tHigh bytes: 0x%08X\n\tLow bytes: 0x%08X\n\tDouble: %f\n", 
                       i, info->high_bytes, info->low_bytes, *(double *)&bits);
                fprintf(out,"[%02d] (large numeric continued)\n", i + 1);
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

                fprintf(out,"[%02d] CONSTANT_InterfaceMethodref_info\n\tClass name: cp_info #%d <%.*s>\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->class_index, class_name->length, class_name->bytes, info->name_and_type_index, nt_name->length, nt_name->bytes, nt_desc->length, nt_desc->bytes);
                break;
            }
            case CONSTANT_MethodHandle: {
                CONSTANT_MethodHandle_info *info = (CONSTANT_MethodHandle_info *)entry->info;

                fprintf(out,"[%02d] CONSTANT_MethodHandle_info\n\tReference kind: %d\n\tReference index: cp_info #%d\n", 
                       i, info->reference_kind, info->reference_index);
                break;
            }
            case CONSTANT_MethodType: {
                CONSTANT_MethodType_info *info = (CONSTANT_MethodType_info *)entry->info;
                // Resolve o descriptor que aponta para um Utf8
                CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[info->descriptor_index]->info;

                fprintf(out,"[%02d] CONSTANT_MethodType_info\n\tDescriptor: cp_info #%d <%.*s>\n", 
                       i, info->descriptor_index, desc->length, desc->bytes);
                break;
            }
            case CONSTANT_InvokeDynamic: {
                CONSTANT_InvokeDynamic_info *info = (CONSTANT_InvokeDynamic_info *)entry->info;
                // Resolve o NameAndType associado
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index]->info;
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index]->info;
                
                fprintf(out,"[%02d] CONSTANT_InvokeDynamic_info\n\tBootstrap method attr: %d\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Dynamic: {
                CONSTANT_Dynamic_info *info = (CONSTANT_Dynamic_info *)entry->info;
                CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)cf->constant_pool[info->name_and_type_index]->info;

                // Busca o nome e o tipo
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[nt->name_index]->info;
                CONSTANT_Utf8_info *tipo = (CONSTANT_Utf8_info *)cf->constant_pool[nt->descriptor_index]->info;

                fprintf(out,"[%02d] CONSTANT_Dynamic_info\n\tBootstrap method attr: %d\n\tName and type: cp_info #%d <%.*s : %.*s>\n", 
                       i, info->bootstrap_method_attr_index, info->name_and_type_index, name->length, name->bytes, tipo->length, tipo->bytes);
                break;
            }
            case CONSTANT_Module: {
                CONSTANT_Module_info *info = (CONSTANT_Module_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                fprintf(out,"[%02d] CONSTANT_Module_info\n\tName: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            case CONSTANT_Package: {
                CONSTANT_Package_info *info = (CONSTANT_Package_info *)entry->info;
                CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[info->name_index]->info;
                fprintf(out,"[%02d] CONSTANT_Package_info\n\tName: cp_info #%d <%.*s>\n", i, info->name_index, name->length, name->bytes);
                break;
            }
            default:
                fprintf(out,"[%02d] (tag %d não implementada)\n", i, entry->tag);
                break;
        }
    }
}

void print_interfaces(FILE *out, ClassFile *cf) {
    fprintf(out, "=======================================================\n");
    fprintf(out, "               INTERFACES (%d Entradas)                \n", cf->interfaces_count);
    fprintf(out, "=======================================================\n");

    if (cf->interfaces_count == 0) {
        fprintf(out, "  (nenhuma interface)\n\n");
        return;
    }

    for (int i = 0; i < cf->interfaces_count; i++) {
        uint16_t interface_cp_index = cf->interfaces[i];

        cp_info *cp_entry = cf->constant_pool[interface_cp_index];
        
        if (cp_entry->tag != CONSTANT_Class) {
            fprintf(out, "[%d] cp_info #%d <Erro: Tag nao e CONSTANT_Class>\n", i, interface_cp_index);
            continue;
        }

        CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)cp_entry->info;

        uint16_t name_index = class_info->name_index;
        cp_info *utf8_entry = cf->constant_pool[name_index];
        CONSTANT_Utf8_info *utf8_info = (CONSTANT_Utf8_info *)utf8_entry->info;

        fprintf(out, "[%d] Interface: cp_info #%d\n", i, interface_cp_index);
        fprintf(out, "    Class name index : cp_info #%d <%.*s>\n", 
                name_index, utf8_info->length, utf8_info->bytes);
        fprintf(out, "    Resolved name    : %.*s\n\n", 
                utf8_info->length, utf8_info->bytes);
    }
}

void print_fields(FILE *out, ClassFile *cf) {
    fprintf(out, "=======================================================\n");
    fprintf(out, "               FIELDS (%d Entradas)                    \n", cf->fields_count);
    fprintf(out, "=======================================================\n");

    if (cf->fields_count == 0) {
        fprintf(out,"  (nenhum field)\n");
        return;
    }

    // O vetor de fields começa no Indice 0
    for (int i = 0; i < cf->fields_count; i++) {
        
        field_info *field = &cf->fields[i];

        CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[field->name_index]->info;
        CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[field->descriptor_index]->info;

        // TItulo do Field
        fprintf(out,"[%d] %.*s\n", i, name->length, name->bytes);
        
        // Detalhes
        fprintf(out,"    Name:           cp_info #%d <%.*s>\n", field->name_index, name->length, name->bytes);
        fprintf(out,"    Descriptor:     cp_info #%d <%.*s>\n", field->descriptor_index, desc->length, desc->bytes);

        fprintf(out,"    Access flags:   ");
        print_field_access_flags(out, field->access_flags);
        fprintf(out,"\n");
        fprintf(out,"    Attributes count: %d\n", field->attributes_count);

        for (int j = 0; j < field->attributes_count; j++) {
            attribute_info *attr = &field->attributes[j];

            // Busca o nome do atributo na Constant Pool
            CONSTANT_Utf8_info *attr_name = (CONSTANT_Utf8_info *)cf->constant_pool[attr->attribute_name_index]->info;

            // Imprime cada atributo
            fprintf(out,"       -> Attribute [%d]: %.*s\n", j, attr_name->length, attr_name->bytes);
            fprintf(out,"           Attribute name index: cp_info #%d <%.*s>\n", attr->attribute_name_index, attr_name->length, attr_name->bytes);
            fprintf(out,"           Attribute length:     %u\n", attr->attribute_length);
            fprintf(out,"\n");
        }
        
        fprintf(out,"\n"); // Pula uma linha no final para separar o próximo field
    }
}

void print_methods(FILE *out, ClassFile *cf) {
    fprintf(out, "=======================================================\n");
    fprintf(out, "               METHODS (%d Entradas)                   \n", cf->methods_count);
    fprintf(out, "=======================================================\n");

    if (cf->methods_count == 0) {
        fprintf(out,"  (nenhum method)\n");
        return;
    }

    // O vetor de methods começa no Indice 0
    for (int i = 0; i < cf->methods_count; i++) {
        
        method_info *method = &cf->methods[i];

        CONSTANT_Utf8_info *name = (CONSTANT_Utf8_info *)cf->constant_pool[method->name_index]->info;
        CONSTANT_Utf8_info *desc = (CONSTANT_Utf8_info *)cf->constant_pool[method->descriptor_index]->info;

        // TItulo do Method
        fprintf(out,"[%d] %.*s\n", i, name->length, name->bytes);
        
        // Detalhes
        fprintf(out,"    Name:           cp_info #%d <%.*s>\n", method->name_index, name->length, name->bytes);
        fprintf(out,"    Descriptor:     cp_info #%d <%.*s>\n", method->descriptor_index, desc->length, desc->bytes);

        fprintf(out,"    Access flags:   ");
        print_method_access_flags(out, method->access_flags);
        fprintf(out,"\n");

        fprintf(out,"    Attributes count: %d\n", method->attributes_count);

        for (int j = 0; j < method->attributes_count; j++) {
            attribute_info *attr = &method->attributes[j];

            // Busca o nome do atributo na Constant Pool
            CONSTANT_Utf8_info *attr_name = (CONSTANT_Utf8_info *)cf->constant_pool[attr->attribute_name_index]->info;

            // Imprime o sumario de cada atributo
            fprintf(out,"       Attribute [%d]: %.*s\n", j, attr_name->length, attr_name->bytes);
            fprintf(out,"           Attribute name index: cp_info #%d <%.*s>\n", attr->attribute_name_index, attr_name->length, attr_name->bytes);
            fprintf(out,"           Attribute length:     %u\n", attr->attribute_length);
            fprintf(out,"\n");
        }
        
        fprintf(out,"\n"); // Pula uma linha no final para separar o próximo method
    }
}

//--------------------------------------------------------------------------
void print_specific_attribute_info(FILE *out, ClassFile *cf, attribute_info *attr) {
    cp_info *cp = cf->constant_pool[attr->attribute_name_index];
    CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)cp->info;
    
    fprintf(out, "    Informacoes Genericas:\n");
    fprintf(out, "        Indice do nome do atributo: cp_info #%d <%.*s>\n", attr->attribute_name_index, utf8->length, utf8->bytes);
    fprintf(out, "        Tamanho do atributo:        %u\n", attr->attribute_length);
    
    fprintf(out, "    Informacoes Especificas:\n");
    
    if (utf8->length == 10 && strncmp((char*)utf8->bytes, "SourceFile", 10) == 0) {
        SourceFile_attribute *sf = (SourceFile_attribute *)attr->info;
        CONSTANT_Utf8_info *sf_utf8 = (CONSTANT_Utf8_info *)cf->constant_pool[sf->sourcefile_index]->info;
        fprintf(out, "        Indice do arquivo fonte:    cp_info #%d <%.*s>\n", sf->sourcefile_index, sf_utf8->length, sf_utf8->bytes);
    } 
    else if (utf8->length == 13 && strncmp((char*)utf8->bytes, "ConstantValue", 13) == 0) {
        ConstantValue_attribute *cv = (ConstantValue_attribute *)attr->info;
        fprintf(out, "        Indice do valor constante:  cp_info #%d\n", cv->constantvalue_index);
    }
    else if (utf8->length == 10 && strncmp((char*)utf8->bytes, "Exceptions", 10) == 0) {
        Exceptions_attribute *exc = (Exceptions_attribute *)attr->info;
        fprintf(out, "        Numero de excecoes:         %u\n", exc->number_of_exceptions);
        for (int e = 0; e < exc->number_of_exceptions; e++) {
            uint16_t idx = exc->exception_index_table[e];
            CONSTANT_Class_info *cinfo = (CONSTANT_Class_info*)cf->constant_pool[idx]->info;
            CONSTANT_Utf8_info *cname = (CONSTANT_Utf8_info*)cf->constant_pool[cinfo->name_index]->info;
            fprintf(out, "        - Exceção: cp_info #%d <%.*s>\n", idx, cname->length, cname->bytes);
        }
    }
    else if (utf8->length == 12 && strncmp((char*)utf8->bytes, "InnerClasses", 12) == 0) {
        InnerClasses_attribute *ic = (InnerClasses_attribute *)attr->info;
        fprintf(out, "        Numero de classes: %u\n", ic->number_of_classes);
        for (int c = 0; c < ic->number_of_classes; c++) {
            InnerClass_info *cl = &ic->classes[c];
            fprintf(out, "        - Indice de Info da Classe Interna:  cp_info #%d\n", cl->inner_class_info_index);
            fprintf(out, "          Indice de Info da Classe Externa:  cp_info #%d\n", cl->outer_class_info_index);
            fprintf(out, "          Indice do Nome Interno:            cp_info #%d\n", cl->inner_name_index);
            fprintf(out, "          Flags de Acesso da Classe Interna: 0x%04X\n", cl->inner_class_access_flags);
        }
    }
    else if (utf8->length == 15 && strncmp((char*)utf8->bytes, "LineNumberTable", 15) == 0) {
        LineNumberTable_attribute *lnt = (LineNumberTable_attribute *)attr->info;
        fprintf(out, "        Tamanho da Tabela de Numeros de Linha: %u\n", lnt->line_number_table_length);
        for (int l = 0; l < lnt->line_number_table_length; l++) {
            fprintf(out, "        - PC Inicial: %u, Numero da Linha: %u\n", lnt->line_number_table[l].start_pc, lnt->line_number_table[l].line_number);
        }
    }
    else if (utf8->length == 18 && strncmp((char*)utf8->bytes, "LocalVariableTable", 18) == 0) {
        LocalVariableTable_attribute *lvt = (LocalVariableTable_attribute *)attr->info;
        fprintf(out, "        Tamanho da Tabela de Variaveis Locais: %u\n", lvt->local_variable_table_length);
        for (int l = 0; l < lvt->local_variable_table_length; l++) {
            fprintf(out, "        - PC Inicial: %u, Tamanho: %u, Indice do Nome: %u, Indice do Descritor: %u, Indice do Slot: %u\n", 
                    lvt->local_variable_table[l].start_pc, lvt->local_variable_table[l].length, 
                    lvt->local_variable_table[l].name_index, lvt->local_variable_table[l].descriptor_index, 
                    lvt->local_variable_table[l].index);
        }
    }
    else if (utf8->length == 4 && strncmp((char*)utf8->bytes, "Code", 4) == 0) {
        Code_attribute *code = (Code_attribute *)attr->info;
        fprintf(out, "        max_stack:    %u\n", code->max_stack);
        fprintf(out, "        max_locals:    %u\n", code->max_locals);
        fprintf(out, "        code_length:   %u\n", code->code_length);
        
        fprintf(out, "        Dump Hexadecimal: ");
        for (uint32_t k = 0; k < code->code_length; k++) {
            fprintf(out, "%02X ", code->code[k]);
        }
        fprintf(out, "\n\n        Desmontagem (Disassembly):\n");
        view_instructions(out, cf, code);

        if (code->exception_table_length > 0) {
            fprintf(out, "\n        Tabela de Excecoes:\n");
            fprintf(out, "        Nr.\tPC Inicial\tPC Final\tPC Handler\tTipo Catch\n");
            for (int e = 0; e < code->exception_table_length; e++) {
                ExceptionTable_info *exc = &code->exception_table[e];
                fprintf(out, "        %d\t%u\t\t%u\t%u\t\t%u", e, exc->start_pc, exc->end_pc, exc->handler_pc, exc->catch_type);
                if (exc->catch_type != 0) {
                    CONSTANT_Class_info *cinfo = (CONSTANT_Class_info*)cf->constant_pool[exc->catch_type]->info;
                    CONSTANT_Utf8_info *cname = (CONSTANT_Utf8_info*)cf->constant_pool[cinfo->name_index]->info;
                    fprintf(out, " <%.*s>\n", cname->length, cname->bytes);
                } else {
                    fprintf(out, " <Qualquer>\n");
                }
            }
        }

        if (code->attributes_count > 0) {
            fprintf(out, "\n        Sub-Atributos de Code (%d Entradas):\n", code->attributes_count);
            for (int a = 0; a < code->attributes_count; a++) {
                attribute_info *sub_attr = &code->attributes[a];
                CONSTANT_Utf8_info *sub_utf8 = (CONSTANT_Utf8_info*)cf->constant_pool[sub_attr->attribute_name_index]->info;
                fprintf(out, "        [%02d] Sub-Atributo %.*s:\n", a, sub_utf8->length, sub_utf8->bytes);
                
                // Chamando a própria função para ler o atributo filho
                print_specific_attribute_info(out, cf, sub_attr); 
            }
        }
    } 
    else if (utf8->length == 10 && strncmp((char*)utf8->bytes, "Deprecated", 10) == 0) {
        fprintf(out, "        (Atributo Deprecated - sem dados adicionais)\n");
    } 
    else {
        if (attr->attribute_length > 0 && attr->info != NULL) {
            fprintf(out, "        Dump Hexadecimal da Info: ");
            uint32_t limit = attr->attribute_length > 30 ? 30 : attr->attribute_length;
            for (uint32_t k = 0; k < limit; k++) {
                fprintf(out, "%02X ", ((uint8_t*)attr->info)[k]);
            }
            if (attr->attribute_length > 30) fprintf(out, "... (+%d bytes)", attr->attribute_length - 30);
            fprintf(out, "\n");
        } else {
            fprintf(out, "        (Sem dados adicionais)\n");
        }
    }
}

void print_attributes(FILE *out, ClassFile *cf) {
    int total_attributes = cf->attributes_count;
    
    for (int i = 0; i < cf->fields_count; i++) {
        total_attributes += cf->fields[i].attributes_count;
    }
    
    for (int i = 0; i < cf->methods_count; i++) {
        total_attributes += cf->methods[i].attributes_count;
    }

    fprintf(out, "=======================================================\n");
    fprintf(out, "               ATTRIBUTES (%d Entradas)                \n", total_attributes);
    fprintf(out, "=======================================================\n");

    int global_attr_idx = 0; 

    for (int i = 0; i < cf->attributes_count; i++) {
        attribute_info *attr = &cf->attributes[i];
        CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info*) cf->constant_pool[attr->attribute_name_index]->info;
        
        fprintf(out, "[%02d] Class Attribute: %.*s\n", global_attr_idx++, utf8->length, utf8->bytes);
        print_specific_attribute_info(out, cf, attr);
        fprintf(out, "\n");
    }

    for (int i = 0; i < cf->fields_count; i++) {
        CONSTANT_Utf8_info *fname = (CONSTANT_Utf8_info *)cf->constant_pool[cf->fields[i].name_index]->info;
        
        for (int j = 0; j < cf->fields[i].attributes_count; j++) {
            attribute_info *attr = &cf->fields[i].attributes[j];
            CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info*) cf->constant_pool[attr->attribute_name_index]->info;
            
            fprintf(out, "[%02d] Field <%.*s> Attribute: %.*s\n", global_attr_idx++, fname->length, fname->bytes, utf8->length, utf8->bytes);
            print_specific_attribute_info(out, cf, attr);
            fprintf(out, "\n");
        }
    }

    for (int i = 0; i < cf->methods_count; i++) {
        CONSTANT_Utf8_info *mname = (CONSTANT_Utf8_info *)cf->constant_pool[cf->methods[i].name_index]->info;
        
        for (int j = 0; j < cf->methods[i].attributes_count; j++) {
            attribute_info *attr = &cf->methods[i].attributes[j];
            CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info*) cf->constant_pool[attr->attribute_name_index]->info;
            
            fprintf(out, "[%02d] Method <%.*s> Attribute: %.*s\n", global_attr_idx++, mname->length, mname->bytes, utf8->length, utf8->bytes);
            print_specific_attribute_info(out, cf, attr);
            fprintf(out, "\n");
        }
    }

    if (global_attr_idx == 0) {
        fprintf(out, "  (nenhum atributo encontrado no arquivo .class)\n");
    }
}