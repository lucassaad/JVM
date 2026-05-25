#include <stdio.h>
#include <stdlib.h>
#include "class_file.h"
#include "reader.h"
#include "viewer.h"
#include "constant_pool.h"
#include "utils.h"

int main()
{
    FILE *file;
    printf("ok\n");
    const char *file_name = "Main.class";

    file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("Erro ao abrir file");
        return 1;
    }

    ClassFile *cf = malloc(sizeof(ClassFile));
    if (cf == NULL)
    {
        perror("Erro ao alocar ClassFile");
        fclose(file);
        return 1;
    }

    // Executa a leitura
    read_classfile(cf, file);
    if (check_constant_pool_references(cf) != 0)
        printf("erro ASJD AKSDJ ");

    print_general_information(cf);

    deep_free(cf);

    if (fclose(file) != 0)
    {
        perror("Erro ao fechar o file");
        return 1;
    }

    return 0;
}