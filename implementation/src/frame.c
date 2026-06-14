#include <stdio.h>
#include <stdlib.h>
#include "frame.h"

// Função para instanciar o frame e seus componentes internos
Frame* create_frame(uint16_t max_locals, uint16_t max_stack, cp_info **cp, Frame *previous) {
    
    // Validação caso o cp for NULL
    if (cp == NULL) {
        fprintf(stderr, "[Erro] Tentativa de criar um Frame sem Constant Pool referenciado.\n");
        return NULL;
    }
    
    Frame *frame = (Frame*) malloc(sizeof(Frame));
    
    if (frame == NULL) {
        return NULL; 
    }

    // Aloca os arrays dinamicamente baseados nos valores extraídos do atributo Code
    frame->local_variables = (uint32_t*) calloc(max_locals, sizeof(uint32_t));
    frame->operand_stack = (uint32_t*) calloc(max_stack, sizeof(uint32_t));

    // Validação para garantir que todos os campos foram alocados
    if ((max_locals > 0 && frame->local_variables == NULL) || 
        (max_stack > 0 && frame->operand_stack == NULL)) {
        free(frame->local_variables);
        free(frame->operand_stack);
        free(frame);
        return NULL;
    }

    // Inicializa as propriedades do frame
    frame->sp = -1; // Pilha de operandos começa vazia
    frame->max_locals = max_locals;
    frame->max_stack = max_stack;
    
    // Configura os ponteiros de contexto
    frame->constant_pool = cp;
    frame->previous_frame = previous;

    return frame;
}

// Função para apagar o frame e liberar a memória utilizada por ele
void free_frame(Frame *frame) {
    if (frame != NULL) {
        free(frame->local_variables);
        free(frame->operand_stack);
        free(frame);
    }
}

// Inicializa a pilha da JVM
JVMStack* create_jvm_stack(uint32_t max_size_bytes) {
    JVMStack *stack = (JVMStack*) malloc(sizeof(JVMStack));
    // Se retornar NULL, não há espaço em memória para alocar a pilha da JVM (OutOfMemoryError)
    if (stack == NULL) {
        fprintf(stderr, "[Erro] OutOfMemoryError: Falha ao alocar a JVM Stack nativa.\n");
        return NULL;
    }

    stack->current_frame = NULL;              // Frame atual é NULL na inicialização da pilha da JVM
    stack->max_size_bytes = max_size_bytes;
    stack->current_size_bytes = 0;
    stack->frame_count = 0;
    return stack;
}

// Calcula o tamanho do frame a ser adicionado na pilha da JVM
static uint32_t calculate_frame_size(uint16_t max_locals, uint16_t max_stack) {
    return sizeof(Frame) + (max_locals * sizeof(uint32_t)) + (max_stack * sizeof(uint32_t));
}

// Operação de push na pilha da JVM para quando há adição de novo frame
int push_frame(JVMStack *stack, uint16_t max_locals, uint16_t max_stack, cp_info **cp) {

    // Validação caso a pilha da JVM não tiver sido inicializada
    if (stack == NULL) {
        fprintf(stderr, "[Erro] Tentativa de realizar push em uma JVM Stack não inicializada (NULL).\n");
        return -3; // Código de erro específico para stack inexistente
    }
    
    uint32_t frame_size = calculate_frame_size(max_locals, max_stack);

    // Verificação de StackOverflowError
    if (stack->current_size_bytes + frame_size > stack->max_size_bytes) {
        fprintf(stderr, "[X] java.lang.StackOverflowError: Limite de %u bytes da pilha excedido.\n", stack->max_size_bytes);
        return -1; // Código de erro para estouro de pilha
    }

    // Tenta alocar o novo frame
    Frame *new_frame = create_frame(max_locals, max_stack, cp, stack->current_frame);
    
    // Verificação de OutOfMemoryError (Se o malloc/calloc falhar)
    if (new_frame == NULL) {
        fprintf(stderr, "[X] java.lang.OutOfMemoryError: Memória nativa insuficiente para alocação do Frame.\n");
        return -2; // Código de erro para falta de memória do SO
    }

    // Atualiza os estados da pilha da JVM se a alocação do novo frame foi bem-sucedida
    stack->current_frame = new_frame;
    stack->current_size_bytes += frame_size;
    stack->frame_count++;

    return 0;
}

// Operação de pop na pilha da JVM, eliminando o frame corrente (topo da pilha)
void pop_frame(JVMStack *stack) {
    if (stack == NULL || stack->current_frame == NULL) {
        return; // Pilha já vazia
    }

    // O frame que será removido será o do topo da pilha
    Frame *frame_to_remove = stack->current_frame;
    uint32_t frame_size = calculate_frame_size(frame_to_remove->max_locals, frame_to_remove->max_stack);

    // Ajusta o topo da pilha para apontar para o frame anterior
    stack->current_frame = frame_to_remove->previous_frame;
    stack->current_size_bytes -= frame_size;
    stack->frame_count--;

    // Libera a memória alocada
    free_frame(frame_to_remove);
}

// Liberação da pilha ao encerrar a JVM
void free_jvm_stack(JVMStack *stack) {
    if (stack != NULL) {
        while (stack->current_frame != NULL) {
            pop_frame(stack);
        }
        free(stack);
    }
}