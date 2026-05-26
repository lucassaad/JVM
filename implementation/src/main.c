#include <stdio.h>
#include <stdlib.h>
#include "class_file.h"
#include "reader.h"
#include "viewer.h"
#include "code_attribute.h"
#include "instruction_reader.h"
#include "constant_pool.h"
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *file;
    const char *file_name;

    // Se o usuário passou um arquivo por argumento, usa ele. Se não, usa o Main.class padrão.
    if (argc > 1) {
        file_name = argv[1];
    } else {
        file_name = "Main.class";
    }

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

    // Executa a leitura binária do arquivo .class
    if (read_classfile(cf, file) != 0) {
        perror("Erro ao ler o .class");
        return 1;
    }
    
    if (check_constant_pool_references(cf) != 0)
        printf("erro nas referencias da constant pool");

    // Fecha o arquivo pois os dados já estão na memória (estrutura cf)
    if (fclose(file) != 0) {
        perror("Erro ao fechar o file");
        free(cf);
        return 1;
    }

    // Abre o arquivo .txt
    FILE *arquivo_txt = fopen("saida_exibidor.txt", "w");
    if (arquivo_txt == NULL) {
        printf("Erro ao criar o arquivo txt!\n");
        return 1;
    }

    // Exibe as informações Gerais
    print_general_information(stdout, cf);
    print_general_information(arquivo_txt, cf);

    // Exibe a Pool de Constantes
    print_constant_pool(stdout, cf);
    print_constant_pool(arquivo_txt, cf);

    // Exibe as Interfaces
    print_interfaces(stdout, cf);
    print_interfaces(arquivo_txt, cf);

    // Exibe fields e methods
    print_fields(stdout, cf);
    print_fields(arquivo_txt, cf);
    print_methods(stdout, cf);
    print_methods(arquivo_txt, cf);

    print_class_attributes(stdout, cf);
    print_class_attributes(arquivo_txt, cf);


    // EXIBIÇÃO DO CODE ATTRIBUTE (Movido para antes dos frees!)
    printf("\n===== CODE ATTRIBUTES =====\n");

    for (int i = 0; i < cf->methods_count; i++) {
        method_info *method = &cf->methods[i];

        for (int j = 0; j < method->attributes_count; j++) {
            attribute_info *attr = &method->attributes[j];
            uint16_t name_index = attr->attribute_name_index;
            cp_info *cp_entry = cf->constant_pool[name_index];
            CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info*) cp_entry->info;

            if (utf8->length == 4 && strncmp((char*)utf8->bytes, "Code", 4) == 0) {
                printf("\nMethod %d - Code Attribute\n", i);
                Code_attribute *code = (Code_attribute*) attr->info;

                // Chama a função modificada que faz o disassembly 
                // Se você alterou para receber cf, use: print_code_attribute(code, cf);
                // Se manteve a assinatura antiga, use: print_code_attribute(code);
                print_code_attribute(code); 
                read_instructions(cf, code);
            }
        }
    }

    // =======================================================
    // LIMPEZA DA MEMÓRIA (Agora no final absoluto do programa)
    // =======================================================
    
    // 1. Libera cada entrada da constant pool
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *entry = cf->constant_pool[i];
        if (entry != NULL) {
            if (entry->tag == 5 || entry->tag == 6) i++;
            free(entry->info);
            free(entry);
        }
    }

    fclose(arquivo_txt);

    // 2. Libera o array da constant pool
    free(cf->constant_pool);

    // 3. Libera a ClassFile principal
    free(cf);

    return 0;
}