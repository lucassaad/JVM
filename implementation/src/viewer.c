#include <stdio.h>
#include "viewer.h"
#include "class_file.h"

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

void print_class_name(Class *cf, uint16_t class_index) {
    if (class_index == 0 || class_index >= cf->constant_pool_count) {
        printf("Índice de classe inválido: %d\n", class_index);
        return;
    }

    cp_info *cp_entry = cf->constant_poll[class_index];
    if (cp_entry != NULL && cp_entry->tag == 7) {
        uint16_t name_index = *(uint16_t *)cp_entry->info;
        print_resolved_utf8(cf, name_index);
    } else {
        printf("Entrada de classe inválida no constant pool: índice %d\n", class_index
}

void print_class_access_flags(uint16_t access_flags) {
    printf("Access Flags: 0x%04x\n", access_flags);
    if (access_flags & 0x0001) printf(" - public\n");
    if (access_flags & 0x0010) printf(" - final\n");
    if (access_flags & 0x0020) printf(" - super\n");
    if (access_flags & 0x0200) printf(" - interface\n");
    if (access_flags & 0x0400) printf(" - abstract\n");
    if (access_flags & 0x1000) printf(" - synthetic\n");
    if (access_flags & 0x2000) printf(" - annotation\n");
    if (access_flags & 0x4000) printf(" - enum\n");
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