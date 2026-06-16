#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frame.h"
#include "jvm_stack.h"
#include "constant_pool.h"

// ---------------------------------------------------------------------------------------
// Criação e Destruição do Frame
// ---------------------------------------------------------------------------------------

Frame* frame_create(Code_attribute *code_attr, cp_info **constant_pool, uint16_t constant_pool_count) {

    // Uso o code_attr para saber os valores de max_locals e max_stack
    if (code_attr == NULL) {
        fprintf(stderr, "frame_create: Code_attribute nulo\n");
        return NULL;
    }

    Frame *frame = malloc(sizeof(Frame));
    if (frame == NULL) {
        perror("frame_create: erro ao alocar Frame");
        return NULL;
    }

    frame->max_locals = code_attr->max_locals;
    frame->max_stack  = code_attr->max_stack;

    // Stack Pointer do frame começa em 0, indicando que esse é o próximo índice livre na Pilha de Operandos
    frame->sp = 0;

    // Vetor de Variáveis Locais: array dinâmico de slots de 32 bits
    frame->local_vars = calloc(frame->max_locals, sizeof(uint32_t));

    // Condição para quando há erro no armazenamento do Vetor de Variáveis Locais
    if (frame->local_vars == NULL && frame->max_locals > 0) {
        perror("frame_create: erro ao alocar local_vars");
        free(frame);
        return NULL;
    }

    // Pilha de Operandos: array dinâmico de slots de 32 bits
    frame->operand_stack = calloc(frame->max_stack, sizeof(uint32_t));

    // Condição para quando há erro no armazenamento da Pilha de Operandos
    if (frame->operand_stack == NULL && frame->max_stack > 0) {
        perror("frame_create: erro ao alocar operand_stack");
        free(frame->local_vars);
        free(frame);
        return NULL;
    }

    frame->constant_pool = constant_pool;
    frame->constant_pool_count = constant_pool_count;

    frame->code = code_attr->code;
    frame->code_length = code_attr->code_length;

    // Program Counter local ao frame, devendo iniciar em 0 para ler o código do método desde o início
    frame->pc = 0;

    // Valor inicial do frame antigo é NULL para abordar o caso de ser o primeiro frame (inicial)
    frame->previous_frame = NULL;

    return frame;
}

void frame_destroy(Frame *frame) {
    if (frame == NULL) return;

    free(frame->local_vars);
    free(frame->operand_stack);
    free(frame);
}

// ---------------------------------------------------------------------------------------
// Acesso ao Vetor de Variáveis Locais
// ---------------------------------------------------------------------------------------

void frame_set_local_int(Frame *frame, uint16_t index, int32_t value) {

    // Variável "raw" criada a fim de guardar a sequência exata de bits, evitando erros de interpretação
    uint32_t raw;

    // Função "memcpy" copia os bits EXATOS de "value" para "raw" (sem cast ou interpretação de sinal)
    memcpy(&raw, &value, sizeof(uint32_t));
    frame->local_vars[index] = raw;
}

int32_t frame_get_local_int(Frame *frame, uint16_t index) {
    int32_t value;
    uint32_t raw = frame->local_vars[index];
    memcpy(&value, &raw, sizeof(int32_t));
    return value;
}

void frame_set_local_float(Frame *frame, uint16_t index, float value) {
    uint32_t raw;
    memcpy(&raw, &value, sizeof(uint32_t));
    frame->local_vars[index] = raw;
}

float frame_get_local_float(Frame *frame, uint16_t index) {
    float value;
    uint32_t raw = frame->local_vars[index];
    memcpy(&value, &raw, sizeof(float));
    return value;
}

// long/double ocupam 2 slots consecutivos em big-endian order:
// slot[index]   = 32 bits mais significativos
// slot[index+1] = 32 bits menos significativos
void frame_set_local_long(Frame *frame, uint16_t index, int64_t value) {
    uint64_t raw;
    memcpy(&raw, &value, sizeof(uint64_t));

    // Deslocamento dos bits da parte mais significativa para armazená-la em 32 bits
    // (o cast para 32 bits "apaga" os 32 bits mais significativos, que serão 0 após o deslocamento)
    frame->local_vars[index]     = (uint32_t)(raw >> 32); // Parte mais significativa

    // Operação AND lógica bit a bit com 0xFFFFFFFF unsigned para isolar os 32 bits menos significativos
    frame->local_vars[index + 1] = (uint32_t)(raw & 0xFFFFFFFFu); // Parte menos significativa
}

int64_t frame_get_local_long(Frame *frame, uint16_t index) {

    // Operação OR lógica para gerar o número de 64 bits
    uint64_t raw = ((uint64_t)frame->local_vars[index] << 32) | (uint64_t)frame->local_vars[index + 1];
    int64_t value;
    memcpy(&value, &raw, sizeof(int64_t));
    return value;
}

void frame_set_local_double(Frame *frame, uint16_t index, double value) {
    uint64_t raw;
    memcpy(&raw, &value, sizeof(uint64_t));

    // Deslocamento dos bits da parte mais significativa para armazená-la em 32 bits
    // (o cast para 32 bits "apaga" os 32 bits mais significativos, que serão 0 após o deslocamento)
    frame->local_vars[index]     = (uint32_t)(raw >> 32); // Parte mais significativa

    // Operação AND lógica bit a bit com 0xFFFFFFFF unsigned para isolar os 32 bits menos significativos
    frame->local_vars[index + 1] = (uint32_t)(raw & 0xFFFFFFFFu); // Parte menos significativa
}

double frame_get_local_double(Frame *frame, uint16_t index) {

    // Operação OR lógica para gerar o número de 64 bits
    uint64_t raw = ((uint64_t)frame->local_vars[index] << 32) | (uint64_t)frame->local_vars[index + 1];
    double value;
    memcpy(&value, &raw, sizeof(double));
    return value;
}

void frame_set_local_ref(Frame *frame, uint16_t index, uint32_t ref) {
    frame->local_vars[index] = ref;
}

uint32_t frame_get_local_ref(Frame *frame, uint16_t index) {
    return frame->local_vars[index];
}

// ---------------------------------------------------------------------------------------
// Pilha de Operandos
// ---------------------------------------------------------------------------------------

void frame_push_raw(Frame *frame, uint32_t raw) {

    // Verificação do tamanho da Pilha de Operandos, impedindo que ultrapasse o valor de (max_stack - 1)
    if (frame->sp >= frame->max_stack) {
        fprintf(stderr, "frame_push_raw: overflow da pilha de operandos\n");
        exit(1);
    }

    // Armazena o dado no índice apontado pelo Stack Pointer e incrementa SP depois (próximo espaço livre)
    frame->operand_stack[frame->sp++] = raw;
}

uint32_t frame_pop_raw(Frame *frame) {

    // Verificação para saber se ainda há elementos na Pilha de Operandos (SP > 0)
    if (frame->sp == 0) {
        fprintf(stderr, "frame_pop_raw: underflow da pilha de operandos\n");
        exit(1);
    }

    // Retorna o próximo espaço livre da Pilha de Operandos (SP - 1)
    // O valor apagado passa a ser visto como "lixo de memória"
    return frame->operand_stack[--frame->sp];
}

void frame_push_int(Frame *frame, int32_t value) {
    uint32_t raw;

    // Valor do inteiro (32 bits) é colocado na variável raw (sem sinal) usando memcpy
    // para prevenir cast e interpretação de sinal
    memcpy(&raw, &value, sizeof(uint32_t));

    // Chama a função de push para números "crus" (raw), colocando os 32 bits do inteiro
    // na Pilha de Operandos
    frame_push_raw(frame, raw);
}

int32_t frame_pop_int(Frame *frame) {

    // Armazena os 32 bits do número inteiro após retirada ("pop") deles da pilha
    uint32_t raw = frame_pop_raw(frame);

    // Cria variável value com sinal para armazenar o inteiro, considerando seu sinal
    int32_t value;
    memcpy(&value, &raw, sizeof(int32_t));
    return value;
}

void frame_push_float(Frame *frame, float value) {
    uint32_t raw;

    // Valor do float (32 bits) é colocado na variável raw (sem sinal) usando memcpy
    // para prevenir cast e interpretação de sinal
    memcpy(&raw, &value, sizeof(uint32_t));

    // Chama a função de push para números "crus" (raw), colocando os 32 bits do float
    // na Pilha de Operandos
    frame_push_raw(frame, raw);
}

float frame_pop_float(Frame *frame) {

    // Armazena os 32 bits do float após retirada ("pop") deles da pilha
    uint32_t raw = frame_pop_raw(frame);

    // Cria variável value com sinal para armazenar o float, considerando seu sinal
    float value;
    memcpy(&value, &raw, sizeof(float));
    return value;
}

// Funções para Objetos e Arrays
void frame_push_ref(Frame *frame, uint32_t ref) {
    frame_push_raw(frame, ref);
}

uint32_t frame_pop_ref(Frame *frame) {
    return frame_pop_raw(frame);
}

// Categoria 2 (long/double): empilha 2 slots, em big-endian
// (parte mais significativa fica "abaixo" na pilha, ou seja, é empilhada primeiro).
void frame_push_long(Frame *frame, int64_t value) {
    uint64_t raw;

    // Usa memcpy para atribuir à variável "raw" o valor de "value", prevenindo cast ou interpretação de sinal
    // (apenas copia os bits da forma como eles são)
    memcpy(&raw, &value, sizeof(uint64_t));

    // Deslocamentos para isolar a parte mais significativa (high) e menos significativa (low)
    // e caber no slot (na Pilha de Operandos, slot é de 32 bits) 
    frame_push_raw(frame, (uint32_t)(raw >> 32));         // high (empilhada primeiro)
    frame_push_raw(frame, (uint32_t)(raw & 0xFFFFFFFFu)); // low
}

int64_t frame_pop_long(Frame *frame) {

    // O que está mais perto do topo da pilha é a parte menos significativa
    uint32_t low  = frame_pop_raw(frame);
    uint32_t high = frame_pop_raw(frame);

    // Reconstrução do número de 64 bits que foi empilhado anteriormente
    uint64_t raw = ((uint64_t)high << 32) | (uint64_t)low;
    int64_t value;

    // Armazena na variável "value" o valor do long, considerando o sinal
    memcpy(&value, &raw, sizeof(int64_t));
    return value;
}

void frame_push_double(Frame *frame, double value) {
    uint64_t raw;

    // Usa memcpy para atribuir à variável "raw" o valor de "value", prevenindo cast ou interpretação de sinal
    // (apenas copia os bits da forma como eles são)
    memcpy(&raw, &value, sizeof(uint64_t));

    // Deslocamentos para isolar a parte mais significativa (high) e menos significativa (low)
    // e caber no slot (na Pilha de Operandos, slot é de 32 bits)
    frame_push_raw(frame, (uint32_t)(raw >> 32));         // high (empilhada primeiro)
    frame_push_raw(frame, (uint32_t)(raw & 0xFFFFFFFFu)); // low
}

double frame_pop_double(Frame *frame) {

    // O que está mais perto do topo da pilha é a parte menos significativa
    uint32_t low  = frame_pop_raw(frame);
    uint32_t high = frame_pop_raw(frame);

    // Reconstrução do número de 64 bits que foi empilhado anteriormente
    uint64_t raw = ((uint64_t)high << 32) | (uint64_t)low;
    double value;

    // Armazena na variável "value" o valor do double, considerando o sinal
    memcpy(&value, &raw, sizeof(double));
    return value;
}

// ---------------------------------------------------------------------------------------
// Funções de Ciclo de Vida do Frame: Push/Pop em chamadas de método
// ---------------------------------------------------------------------------------------

uint16_t descriptor_count_arg_slots(const char *method_descriptor, int is_instance_method) {

    // Inicialização da variável "slots": se for método de instância, já precisará de 
    // pelo menos 1 slot. Caso contrário, "slots" começa em zero
    uint16_t slots = is_instance_method ? 1 : 0; // slot 0 = "this" para métodos de instância

    if (method_descriptor == NULL) return slots;

    // method_descriptor é o ponteiro para o início da string que descreve o método
    const char *p = method_descriptor;

    // O descriptor de método tem o formato "(ParameterDescriptors)ReturnDescriptor"
    if (*p != '(') return slots; // formato inválido (não começa com par6entese aberto "("), retorna o mínimo

    // Se *p == '('
    p++; // pula '('

    // Laço while para enquanto não chegar ao final do descritor de método
    // Inclusão da condição com caractere nulo (\0) para evitar acesso inadequado à memória, caso
    // descritor esteja corrompido de alguma forma (não finalizando com ")")
    while (*p != ')' && *p != '\0') {
        switch (*p) {
            case 'B': // byte
            case 'C': // char
            case 'F': // float
            case 'I': // int
            case 'S': // short
            case 'Z': // boolean
                slots += 1; // Ocupam apenas 1 slot (32 bits)
                p++;        // Incrementa "p" para seguir com a leitura do descritor
                break;      // Sai do laço do switch e segue para analisar o resto do descritor

            case 'D': // double
            case 'J': // long
                slots += 2; // Ocupam 2 slots (64 bits)
                p++;        // Incrementa "p" para seguir com a leitura do descritor
                break;

            case 'L': // referência à classe
                slots += 1; // Ocupa apenas 1 slot (32 bits), que será a instância da classe referenciada
                while (*p != ';' && *p != '\0') p++; // Pular para o final da referência ou do descritor (\0)
                if (*p == ';') p++; // A referência a uma classe termina com ";"
                break;

            case '[': // array: pode ter múltiplos '[' antes do tipo do elemento
                slots += 1; // arrays sempre ocupam 1 slot (referência), independente do tipo base
                            // formam apenas 1 argumento do método, ocupando apenas 1 slot
                while (*p == '[') p++;

                // Se for um array de instâncias de uma classe
                if (*p == 'L') {
                    while (*p != ';' && *p != '\0') p++; // Pula para o final da referência ou do descritor (\0)
                    if (*p == ';') p++;

                // Se ainda não chegou ao final do descritor (quando *p == \0)
                } else if (*p != '\0') {
                    p++; // tipo primitivo do array (B, C, D, F, I, J, S, Z)
                }
                break;

            default:
                // Caractere inesperado: avança para evitar loop infinito
                p++;
                break;
        }
    }

    // Retorna a quantidade de slots que slots que será necessária alocar no Vetor de Variáveis Locais
    // do novo frame
    return slots;
}

JVMStackStatus frame_push_method(JVMStack *stack, Code_attribute *code_attr,
                                  cp_info **constant_pool, uint16_t constant_pool_count,
                                  uint16_t num_arg_slots) {

    // Ponteiro para armazenar endereço do frame chamador (antigo frame corrente)
    Frame *caller = stack->current_frame;

    // Cria o novo frame, extraindo max_locals e max_stack de code_attr
    Frame *new_frame = frame_create(code_attr, constant_pool, constant_pool_count);

    // Se não há espaço na Pilha da JVM para a criação do frame
    if (new_frame == NULL) {
        return JVM_STACK_OUT_OF_MEMORY;
    }

    // Move os argumentos do topo da Pilha de Operandos do chamador para o
    // início do vetor de Variáveis Locais do novo frame, preservando a ordem.
    if (caller != NULL && num_arg_slots > 0) {

        // O Stack Pointer indica a próxima posição LIVRE (topo = SP - 1)
        // Se o SP for menor que o número de argumentos esperado, não há argumentos suficientes
        if (caller->sp < num_arg_slots) {
            fprintf(stderr, "frame_push_method: pilha de operandos do chamador "
                            "nao possui argumentos suficientes (sp=%u, esperado=%u)\n",
                            caller->sp, num_arg_slots);
            frame_destroy(new_frame); // Destrói o frame
            return JVM_STACK_INVALID_STATE; // Dispara erro para estado inválido
        }

        // Se o número de slots necessários (argumentos) é maior do que o tamanho máximo do
        // Vetor de Variáveis Locais do novo frame, há inconsistência
        if (num_arg_slots > new_frame->max_locals) {
            fprintf(stderr, "frame_push_method: max_locals (%u) insuficiente para "
                            "os argumentos (%u)\n", new_frame->max_locals, num_arg_slots);
            frame_destroy(new_frame); // Destrói o frame
            return JVM_STACK_OVERFLOW_ERROR; // Dispara erro de Overflow
        }

        // Os argumentos estão nas últimas 'num_arg_slots' posições da pilha do
        // chamador, na mesma ordem em que devem aparecer no vetor de variáveis
        // locais (índice 0 = primeiro argumento ou "this" para métodos de instância).
        uint16_t base = caller->sp - num_arg_slots;
        for (uint16_t i = 0; i < num_arg_slots; i++) {
            // Aloca os argumentos no Vetor de Variáveis Locais do novo frame
            new_frame->local_vars[i] = caller->operand_stack[base + i];
        }

        // Remove os argumentos consumidos da Pilha de Operandos do chamador
        caller->sp -= num_arg_slots;
    }

    // Empilha o novo frame na JVM Stack (faz previous_frame = caller)
    uint32_t frame_size = jvm_stack_frame_size(new_frame->max_locals, new_frame->max_stack);
    JVMStackStatus status = jvm_stack_push(stack, new_frame, frame_size);

    // Se o status for diferente de "OK", ocorreu algo errado (Overflow ou OutOfMemory)
    if (status != JVM_STACK_OK) {
        frame_destroy(new_frame);
        return status;
    }

    return JVM_STACK_OK;
}

int frame_pop_method(JVMStack *stack, ReturnType return_type) {
    Frame *current = stack->current_frame;
    if (current == NULL) {
        return 0;
    }

    // Lê o valor de retorno (se houver) do topo da Pilha de Operandos do
    // frame que está terminando, ANTES de destruí-lo.
    int has_return = (return_type != RETURN_VOID);

    // Criação de variáveis "placeholders"
    int32_t  ret_int    = 0;
    float    ret_float  = 0.0f;
    int64_t  ret_long   = 0;
    double   ret_double = 0.0;
    uint32_t ret_ref    = 0;

    // Se há retorno no método do antigo frame corrente, faz o pop adequado de acordo com o tipo
    if (has_return) {
        switch (return_type) {
            case RETURN_INT:    ret_int    = frame_pop_int(current);    break;
            case RETURN_FLOAT:  ret_float  = frame_pop_float(current);  break;
            case RETURN_LONG:   ret_long   = frame_pop_long(current);   break;
            case RETURN_DOUBLE: ret_double = frame_pop_double(current); break;
            case RETURN_REF:    ret_ref    = frame_pop_ref(current);    break;
            default: break;
        }
    }

    // Destrói o frame atual e restaura o frame anterior como corrente
    uint32_t frame_size = jvm_stack_frame_size(current->max_locals, current->max_stack);
    Frame *popped = jvm_stack_pop(stack, frame_size);
    frame_destroy(popped);

    Frame *new_current = stack->current_frame;

    // Empilha o valor de retorno na pilha de operandos do frame chamador,
    // fazendo o push adequado para o tipo de dado retornado
    if (has_return && new_current != NULL) {
        switch (return_type) {
            case RETURN_INT:    frame_push_int(new_current, ret_int);       break;
            case RETURN_FLOAT:  frame_push_float(new_current, ret_float);   break;
            case RETURN_LONG:   frame_push_long(new_current, ret_long);     break;
            case RETURN_DOUBLE: frame_push_double(new_current, ret_double); break;
            case RETURN_REF:    frame_push_ref(new_current, ret_ref);       break;
            default: break;
        }
    }

    // Verifica se a Pilha da JVM ficou vazia (new_current == NULL) ou não
    return (new_current != NULL) ? 1 : 0;
}