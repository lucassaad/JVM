#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "class_file.h"
#include "reader.h"
#include "viewer.h"
#include "attributes.h"
#include "instruction_viewer.h"
#include "constant_pool.h"

#include "method_area.h"
#include "interpreter.h" 
#include "frame.h"

int main(int argc, char *argv[]) {
    
    // Tratamento de Argumentos (Linha de Comando)
    if (argc != 3) {
        printf("Usage: %s [-l | -i] <arquivo.class>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *file_name = argv[2];

    int is_leitor = 0;
    int is_interpreter = 0;

    if (strcmp(mode, "-l") == 0 || strcmp(mode, "--leitor") == 0) {
        is_leitor = 1;
    } else if (strcmp(mode, "-i") == 0 || strcmp(mode, "--interpretador") == 0) {
        is_interpreter = 1;
    } else {
        printf("Usage: %s [-l | -i] <arquivo.class>\n", argv[0]);
        return 1;
    }

    // Leitura Binária e Validação (Comum a ambos os modos)
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Erro ao abrir file");
        return 1;
    }

    ClassFile *cf = malloc(sizeof(ClassFile));
    if (cf == NULL) {
        perror("Erro ao alocar ClassFile");
        fclose(file);
        return 1;
    }

    if (read_classfile(cf, file) != 0) {
        perror("Erro ao ler o .class");
        free(cf);
        fclose(file);
        return 1;
    }
    
    if (check_constant_pool_references(cf) != 0) {
        printf("Erro nas referencias da constant pool\n");
    }

    if (fclose(file) != 0) {
        perror("Erro ao fechar o file");
    }


    // Inicializa a Method Area e registra a classe principal
    method_area_init();
    method_area_register(cf);

    // Bifurcação de Lógica (Leitor vs Interpretador)
    if (is_leitor) {
        printf("Iniciando modo Leitor/Exibidor...\n");
        
        FILE *arquivo_txt = fopen("saida_exibidor.txt", "w");
        if (arquivo_txt == NULL) {
            printf("Erro ao criar o arquivo txt!\n");
        } else {
            print_general_information(stdout, cf);
            print_general_information(arquivo_txt, cf);

            print_constant_pool(stdout, cf);
            print_constant_pool(arquivo_txt, cf);

            print_interfaces(stdout, cf);
            print_interfaces(arquivo_txt, cf);

            print_fields(stdout, cf);
            print_fields(arquivo_txt, cf);
            
            print_methods(stdout, cf);
            print_methods(arquivo_txt, cf);

            print_attributes(stdout, cf);
            print_attributes(arquivo_txt, cf);

            fclose(arquivo_txt); 
        }
        
    } else if (is_interpreter) {
        printf("Iniciando modo Interpretador (Motor de Execucao)...\n");
        
        // Sobrescreve/Cria o arquivo de auditoria unificado
        FILE *initial_log = fopen("execucao_jvm.txt", "w");
        if (initial_log == NULL) {
            fprintf(stderr, "Erro ao criar arquivo de dump 'execucao_jvm.txt'\n");
            free(cf);
            return 1;
        }

        // Procura o método main
        method_info *main_method = method_area_find_method(cf, "main", "([Ljava/lang/String;)V", 0x0001 | 0x0008);
        if (main_method == NULL) {
            fprintf(stderr, "Erro: Metodo 'main' nao encontrado na classe.\n");
            fprintf(initial_log, "FALHA: Metodo 'main' de entrada nao localizado.\n");
            fclose(initial_log);
            free(cf);
            return 1;
        }

        // Procura o atributo Code
        Code_attribute *code = method_area_get_code(cf, main_method);
        if (code == NULL) {
            fprintf(stderr, "Erro: atributo Code do metodo main nao encontrado.\n");
            fprintf(initial_log, "FALHA: Atributo 'Code' do entrypoint main ausente.\n");
            fclose(initial_log);
            free(cf);
            return 1;
        }

        // Registra metadados iniciais exclusivamente no arquivo de texto
        fprintf(initial_log, "=======================================================\n");
        fprintf(initial_log, "             DUMP DE EXECUCAO DA JVM                   \n");
        fprintf(initial_log, "=======================================================\n");
        fprintf(initial_log, "Classe Inicial Alvo : %s\n", argv[2]);
        fprintf(initial_log, "Metodo Ponto de Entrada: main([Ljava/lang/String;)V\n");
        fprintf(initial_log, "Configuracao do Frame Inicial:\n");
        fprintf(initial_log, "  -> max_stack   = %d\n", code->max_stack);
        fprintf(initial_log, "  -> max_locals  = %d\n", code->max_locals);
        fprintf(initial_log, "  -> code_length = %d bytes\n", code->code_length);
        fprintf(initial_log, "=======================================================\n\n");
        fclose(initial_log); // Fecha temporariamente para o execute_engine reabrir em append

        // Cria o frame principal
        Frame *frame = frame_create(code, cf->constant_pool, cf->constant_pool_count);
        if (frame == NULL) {
            fprintf(stderr, "Erro ao alocar Frame inicial da JVM.\n");
            return 1;
        }

        // Inicializa a pilha de execução
        JVMStack stack;
        jvm_stack_init(&stack, 1024 * 1024); // Limite de 1MB

        uint32_t frame_size = jvm_stack_frame_size(code->max_locals, code->max_stack);
        JVMStackStatus status = jvm_stack_push(&stack, frame, frame_size);

        if (status != JVM_STACK_OK) {
            fprintf(stderr, "Erro crítico de estouro ao empilhar Frame na Stack.\n");
            frame_destroy(frame);
            free(cf);
            return 1;
        }

        printf("Execucao iniciada. Todo o rastro tecnico sera enviado para 'execucao_jvm.txt'\n\n");

        // Executa o motor em modo silencioso para logs técnicos
        execute_engine(&stack, cf);
        
        printf("\nExecucao concluida com sucesso. Arquivo 'execucao_jvm.txt' atualizado.\n");
    }

    free(cf->constant_pool);
    free(cf);

    return 0;
}