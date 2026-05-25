#include <stdio.h>
#include "viewer.h"
#include "class_file.h"
#include "constant_pool.h"
#include <string.h>

// Traduz a versão Major
const char* get_java_version_string(uint16_t major_version) {
    switch (major_version) {
        case 45: return "Java 1.1 [Javac 1.1]";
        case 46: return "Java 1.2 [Javac 1.2]";
        case 47: return "Java 1.3 [Javac 1.3]";
        case 48: return "Java 1.4 [Javac 1.4]";
        case 49: return "Java 5 [Javac 1.5]";
        case 50: return "Java 6 [Javac 1.6]";
        case 51: return "Java 7 [Javac 1.7]";
        case 52: return "Java 8 [Javac 1.8]"; 
        case 53: return "Java 9 [Javac 9]";
        case 54: return "Java 10 [Javac 10]";
        case 55: return "Java 11 [Javac 11]";
        case 56: return "Java 12 [Javac 12]";
        case 57: return "Java 13 [Javac 13]";
        case 58: return "Java 14 [Javac 14]";
        case 59: return "Java 15 [Javac 15]";
        case 60: return "Java 16 [Javac 16]";
        case 61: return "Java 17 [Javac 17]";
        case 62: return "Java 18 [Javac 18]";
        case 63: return "Java 19 [Javac 19]";
        case 64: return "Java 20 [Javac 20]";
        case 65: return "Java 21 [Javac 21]";
        case 66: return "Java 22 [Javac 22]";
        case 67: return "Java 23 [Javac 23]";
        case 68: return "Java 24 [Javac 24]";
        case 69: return "Java 25 [Javac 25]";
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
    
    // Implementar demais campos
    fprintf(out, "=======================================================\n");
}