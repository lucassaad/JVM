#include <stdlib.h>
#include "frame.h"

// Função para instanciar o frame e seus componentes internos
Frame* create_frame(uint16_t max_locals, uint16_t max_stack, cp_info **cp, Frame *previous) {
    Frame *frame = (Frame*) malloc(sizeof(Frame));
    
    // Tratamento primário para OutOfMemoryError (será expandido na Issue #70)
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

// Limpeza de memória crucial para quando a função Pop for implementada (Issue #71)
void free_frame(Frame *frame) {
    if (frame != NULL) {
        free(frame->local_variables);
        free(frame->operand_stack);
        free(frame);
    }
}