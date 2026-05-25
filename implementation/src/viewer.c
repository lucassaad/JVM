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