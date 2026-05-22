#include <stdio.h>
#include <stdlib.h>
#include "class_file.h"
#include "reader.h"
#include "viewer.h"
#include "code_attribute.h"
#include <string.h>

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
        fclose(file);
        return 1;
    }

    // Executa a leitura
    read_classfile(cf, file);

    print_general_information(cf);

    // Limpeza (temporária, até implementarem o deep free no utils.c)
    // 1. libera cada entrada da constant pool
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *entry = cf->constant_pool[i];

        if (entry->tag == 5 || entry->tag == 6) i++;
        free(entry->info);
        free(entry);
    }

    // 2. libera o array da constant pool
    free(cf->constant_pool);

    // 3. libera a ClassFile
    free(cf);
    

    if (fclose(file) != 0) {
        perror("Erro ao fechar o file");
        return 1;
    }
    printf("\n===== CODE ATTRIBUTES =====\n");

    for (int i = 0; i < cf->methods_count; i++) {

        method_info *method = &cf->methods[i];

        for (int j = 0; j < method->attributes_count; j++) {

            attribute_info *attr = &method->attributes[j];

            uint16_t name_index = attr->attribute_name_index;

            cp_info *cp_entry = cf->constant_pool[name_index];

            CONSTANT_Utf8_info *utf8 =
                (CONSTANT_Utf8_info*) cp_entry->info;

            if (
                utf8->length == 4 &&
                strncmp((char*)utf8->bytes, "Code", 4) == 0
            ) {

                printf("\nMethod %d - Code Attribute\n", i);

                Code_attribute *code =
                    (Code_attribute*) attr->info;

                print_code_attribute(code);
            }
        }
    }


    return 0;
}