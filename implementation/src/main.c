#include <stdio.h>
#include <stdlib.h>
#include "class_file.h"
#include "reader.h"
#include "viewer.h"

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
    if (check_constant_pool_references(cf) != 0) printf("erro ASJD AKSDJ ");
    

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



    return 0;
}