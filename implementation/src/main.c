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
        
        // Código para o Interpretador

        // Procura o método main
        method_info *main_method = method_area_find_method(cf, "main", "([Ljava/lang/String;)V", 0x0001 | 0x0008);
        if (main_method == NULL) {
            fprintf(stderr, "Erro: Metodo 'main' nao encontrado na classe.\n");
            free(cf);
            return 1;
        }
        // prints para debug
        printf("main encontrado\n");


        // Procura o atributo Code
        Code_attribute *code = method_area_get_code(cf, main_method);
        if (code == NULL) {
            fprintf(stderr, "Erro: atributo Code nao encontrado.\n");
            free(cf);
            return 1;
        }
        // prints para debug
        printf("Code encontrado\n");
        printf("max_stack = %d\n", code->max_stack);
        printf("max_locals = %d\n", code->max_locals);
        printf("code_length = %d\n", code->code_length);
        printf("primeiro opcode = 0x%02X\n", code->code[0]);

        // Cria o primeiro Frame
        Frame *frame = frame_create(code, cf->constant_pool, cf->constant_pool_count);

        if (frame == NULL) {
            fprintf(stderr, "Erro ao criar Frame.\n");
            free(cf);
            return 1;
        }
        // prints para debug
        printf("Frame criado\n");
        printf("PC = %u\n", frame->pc);
        printf("Primeiro opcode pelo frame = 0x%02X\n", frame->code[0]);

        // Inicializa a JVM Stack
        JVMStack stack;

        jvm_stack_init(&stack, 1024 * 1024); // 1 MB

        uint32_t frame_size = jvm_stack_frame_size(code->max_locals, code->max_stack);

        // Empilha o primeiro Frame
        JVMStackStatus status = jvm_stack_push(&stack, frame, frame_size);

        //prints para debug
        printf("Frames = %d\n", stack.frame_count);
        printf("Current frame = %p\n", (void*)stack.current_frame); 

        if (status != JVM_STACK_OK) {
            fprintf(stderr, "Erro ao inicializar JVM Stack.\n");
            frame_destroy(frame);
            return 1;
        }

        // Inicia o Fetch-Decode-Execute
        execute_engine(&stack, cf);


    }

    // =======================================================
    // 4. LIMPEZA DA MEMÓRIA (Executada em ambos os fluxos)
    // =======================================================
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *entry = cf->constant_pool[i];
        if (entry != NULL) {
            if (entry->tag == 5 || entry->tag == 6) i++; 
            free(entry->info);
            free(entry);
        }
    }

    free(cf->constant_pool);
    free(cf);

    return 0;
}