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

    print_general_information(cf);

    // Limpeza (temporária, até implementarem o deep free no utils.c)
    free(cf);

    if (fclose(file) != 0) {
        perror("Erro ao fechar o file");
        return 1;
    }



    return 0;
}