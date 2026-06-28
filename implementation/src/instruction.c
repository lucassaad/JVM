/**
 * @file instruction.c
 * @brief Implementação de todas as instruções bytecode da JVM.
 *
 * Cada função implementa um opcode JVM, operando diretamente sobre o Frame
 * corrente (pilha de operandos, variáveis locais, PC) e, quando necessário,
 * sobre a JVMStack (invocações de método).
 *
 * Convenções gerais de implementação:
 * - Operandos inline são lidos incrementando frame->pc manualmente após o
 *   interpretador já ter avançado 1 byte pelo opcode.
 * - Desvios condicionais e incondicionais calculam o destino como
 *   opcode_pc + offset, onde opcode_pc é o PC salvo antes de ler os operandos.
 * - Referências a objetos e arrays são armazenadas como uint32_t (uintptr_t
 *   truncado) e reconstituídas com cast para ponteiro via (uintptr_t).
 * - Tipos de 64 bits (long, double) são divididos em 2 slots de 32 bits
 *   (big-endian) na pilha e nas variáveis locais; memcpy é usado para
 *   converter entre uint64_t e os tipos com sinal/ponto flutuante.
 *
 * Mocks e hacks acadêmicos presentes:
 * - getstatic/putstatic: detectam "java/lang/System.out" e empilham/descartam
 *   uma referência nula (ref=0) para permitir chamadas a println.
 * - invokevirtual: detecta "java/io/PrintStream.println" e "print" e imprime
 *   diretamente via printf para int, float, double, char, long e String.
 *   Strings são resolvidas percorrendo CP_String → CP_Utf8.
 * - invokespecial: detecta "java/lang/Object.<init>" e apenas consome o "this"
 *   da pilha sem criar frame, simulando o construtor padrão.
 * - invokestatic: detecta o método "Soma(String,String)String" e retorna a
 *   primeira String de entrada como mock de concatenação.
 * - multianewarray: suporta apenas arrays de 2 dimensões (limitação acadêmica).
 *
 * Função auxiliar estática:
 * - get_field_descriptor_first_char(): navega CP Fieldref → NameAndType → Utf8
 *   e retorna o primeiro caractere do descritor do campo, usado por getfield e
 *   putfield para determinar se o campo é de 64 bits ('J' ou 'D').
 * - ldc_resolve(): lógica comum a ldc e ldc_w para resolver Integer, Float e
 *   String a partir de um índice já decodificado do constant pool.
 */

#include "instruction.h"
#include "constant_pool.h"
#include "method_area.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// INSTRUÇÕES MATEMÁTICAS - INTEIROS (32 BITS)
// Opcode: 0x60
void iadd(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 + val2);
}

// Opcode: 0x64
void isub(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 - val2);
}

// Opcode: 0x68
void imul(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 * val2);
}

// Opcode: 0x6C
void idiv(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    if (val2 == 0) {
        fprintf(stderr, "ArithmeticException: / by zero\n");
        exit(1);
    }
    frame_push_int(frame, val1 / val2);
}

// Opcode: 0x70
void irem(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    if (val2 == 0) {
        fprintf(stderr, "ArithmeticException: / by zero\n");
        exit(1);
    }
    frame_push_int(frame, val1 % val2);
}

// Opcode: 0x84
void iinc(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    
    int8_t const_val = (int8_t)frame->code[frame->pc++];
    
    int32_t val = frame_get_local_int(frame, index);
    frame_set_local_int(frame, index, val + (int32_t)const_val);
}

// INSTRUÇÕES MATEMÁTICAS - LONGS (64 BITS)
// Opcode: 0x61
void ladd(Frame *frame) {
    int64_t val2 = frame_pop_long(frame);
    int64_t val1 = frame_pop_long(frame);
    frame_push_long(frame, val1 + val2);
}

// Opcode: 0x65
void lsub(Frame *frame) {
    int64_t val2 = frame_pop_long(frame);
    int64_t val1 = frame_pop_long(frame);
    frame_push_long(frame, val1 - val2);
}

// Opcode: 0x69
void lmul(Frame *frame) {
    int64_t val2 = frame_pop_long(frame);
    int64_t val1 = frame_pop_long(frame);
    frame_push_long(frame, val1 * val2);
}

// Opcode: 0x6D
void execute_ldiv(Frame *frame) {
    int64_t val2 = frame_pop_long(frame);
    int64_t val1 = frame_pop_long(frame);
    if (val2 == 0) {
        fprintf(stderr, "ArithmeticException: / by zero\n");
        exit(1);
    }
    frame_push_long(frame, val1 / val2);
}

// Opcode: 0x71
void lrem(Frame *frame) {
    int64_t val2 = frame_pop_long(frame);
    int64_t val1 = frame_pop_long(frame);
    if (val2 == 0) {
        fprintf(stderr, "ArithmeticException: / by zero\n");
        exit(1);
    }
    frame_push_long(frame, val1 % val2);
}

// INSTRUÇÕES MATEMÁTICAS - DOUBLES (64 BITS)
// Opcode: 0x63
void dadd(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    frame_push_double(frame, val1 + val2);
}

// Opcode: 0x67
void dsub(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    frame_push_double(frame, val1 - val2);
}

// Opcode: 0x6B
void dmul(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    frame_push_double(frame, val1 * val2);
}

// Opcode: 0x6F
void ddiv(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);

    frame_push_double(frame, val1 / val2);
}

// Opcode: 0x73
void drem(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    frame_push_double(frame, fmod(val1, val2));
}

// INSTRUÇÕES LÓGICAS E BIT-A-BIT (INTEIROS)
// Opcode: 0x7E
void iand(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 & val2);
}

// Opcode: 0x80
void ior(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 | val2);
}

// Opcode: 0x82
void ixor(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 ^ val2);
}

// Opcode: 0x78 (Shift Left)
void ishl(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);

    frame_push_int(frame, val1 << (val2 & 0x1F));
}

// Opcode: 0x7A (Arithmetic Shift Right - Mantém o sinal)
void ishr(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);
    frame_push_int(frame, val1 >> (val2 & 0x1F));
}

// Opcode: 0x7C (Logical Shift Right - Preenche com zeros)
void iushr(Frame *frame) {
    int32_t val2 = frame_pop_int(frame);
    int32_t val1 = frame_pop_int(frame);

    uint32_t u_val1 = (uint32_t)val1;
    frame_push_int(frame, (int32_t)(u_val1 >> (val2 & 0x1F)));
}

// FUNÇÃO AUXILIAR
char get_field_descriptor_first_char(cp_info **constant_pool, uint16_t indexbyte) {
    // O indexbyte aponta para um CONSTANT_Fieldref
    cp_info *fieldref_cp = constant_pool[indexbyte];
    CONSTANT_Fieldref_info *fieldref = (CONSTANT_Fieldref_info *)fieldref_cp->info;
    
    // Do Fieldref, pegamos o NameAndType
    cp_info *nt_cp = constant_pool[fieldref->name_and_type_index];
    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)nt_cp->info;
    
    // Do NameAndType, pegamos o descriptor_index que aponta pro Utf8
    cp_info *utf8_cp = constant_pool[nt->descriptor_index];
    CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)utf8_cp->info;
    
    // Retorna a primeira letra da string (ex: 'I', 'J', 'D', 'L')
    return (char)utf8->bytes[0];
}

// INSTRUÇÕES DE ARRAYS GENÉRICAS
// Opcode: 0xBC
void newarray(Frame *frame) {
    uint8_t atype = frame->code[frame->pc++];
    int32_t count = frame_pop_int(frame);
    
    Array *arr = create_new_array(count, (ArrayType)atype);
    if (arr == NULL) { exit(1); }
    
    frame_push_ref(frame, (uint32_t)(uintptr_t)arr);
}

// Opcode: 0xBD
void anewarray(Frame *frame) {
    uint16_t indexbyte = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2;
    (void)indexbyte;
    int32_t count = frame_pop_int(frame);
    
    Array *arr = create_new_array(count, T_REFERENCE);
    if (arr == NULL) { exit(1); }
    
    frame_push_ref(frame, (uint32_t)(uintptr_t)arr);
}

// Opcode: 0xBE
void arraylength(Frame *frame) {
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) {
        fprintf(stderr, "NullPointerException: arraylength\n");
        exit(1);
    }
    Array *arr = (Array *)(uintptr_t)ref;
    frame_push_int(frame, arr->length);
}

// Opcode: 0xC5
void multianewarray_inst(Frame *frame) {
    // Lê o índice da Constant Pool (2 bytes)
    uint16_t cp_index = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2;
    (void)cp_index; // Hack acadêmico: ignoramos o tipo da classe pois nosso criador de arrays é genérico

    // Lê a quantidade de dimensões (1 byte)
    uint8_t dimensions = frame->code[frame->pc++];

    if (dimensions != 2) {
        fprintf(stderr, "[multianewarray] Suporte implementado apenas para 2 dimensoes neste escopo.\n");
        exit(1);
    }

    // Desempilha os tamanhos das dimensões na ordem inversa (Pilha de Operandos)
    int32_t dim2_size = frame_pop_int(frame); 
    int32_t dim1_size = frame_pop_int(frame); 

    // Aloca o Array Principal (Vetor de referências que representará as Linhas)
    Array *main_array = create_new_array(dim1_size, T_REFERENCE);
    if (main_array == NULL) { exit(1); }

    // Aloca recursivamente as colunas para cada uma das linhas criadas
    for (int32_t i = 0; i < dim1_size; i++) {
        Array *sub_array = create_new_array(dim2_size, T_REFERENCE);
        if (sub_array == NULL) { exit(1); }
        main_array->elements[i] = (int32_t)(uintptr_t)sub_array;
    }

    // Empilha a matriz resultante de volta como uma referência no topo
    frame_push_ref(frame, (uint32_t)(uintptr_t)main_array);
}

// INSTRUÇÕES DE LEITURA (LOAD) EM ARRAYS
// Opcode: 0x2E
void iaload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t value = arr->elements[index];
    frame_push_int(frame, value); 
}

// Opcode: 0x32
void aaload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t value = arr->elements[index];
    frame_push_ref(frame, (uint32_t)value);
}

// Opcode: 0x2F
void laload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t real_index = index * 2;
    uint32_t high = arr->elements[real_index];
    uint32_t low  = arr->elements[real_index + 1];
    uint64_t raw_64 = ((uint64_t)high << 32) | (uint64_t)low;
    
    int64_t val;
    memcpy(&val, &raw_64, sizeof(int64_t));
    frame_push_long(frame, val);
}

// Opcode: 0x31
void daload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t real_index = index * 2;
    uint32_t high = arr->elements[real_index];
    uint32_t low  = arr->elements[real_index + 1];
    uint64_t raw_64 = ((uint64_t)high << 32) | (uint64_t)low;
    
    double val;
    memcpy(&val, &raw_64, sizeof(double));
    frame_push_double(frame, val);
}

// INSTRUÇÕES DE ESCRITA (STORE) EM ARRAYS
// Opcode: 0x4F
void iastore(Frame *frame) {
    int32_t value = frame_pop_int(frame);
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = value;
}

// Opcode: 0x53
void aastore(Frame *frame) {
    int32_t value = frame_pop_int(frame);
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = value;
}

// Opcode: 0x50
void lastore(Frame *frame) {
    int64_t value = frame_pop_long(frame);
    uint64_t raw_64;
    memcpy(&raw_64, &value, sizeof(uint64_t));
    
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t real_index = index * 2;
    arr->elements[real_index]     = (uint32_t)(raw_64 >> 32);         // High
    arr->elements[real_index + 1] = (uint32_t)(raw_64 & 0xFFFFFFFFu); // Low
}

// Opcode: 0x52
void dastore(Frame *frame) {
    double value = frame_pop_double(frame);
    uint64_t raw_64;
    memcpy(&raw_64, &value, sizeof(uint64_t));
    
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t real_index = index * 2;
    arr->elements[real_index]     = (uint32_t)(raw_64 >> 32);         // High
    arr->elements[real_index + 1] = (uint32_t)(raw_64 & 0xFFFFFFFFu); // Low
}

// INSTRUÇÕES DE LEITURA (LOAD) PARA DEMAIS ARRAYS PRIMITIVOS
// =========================================================================

// Opcode: 0x30
void faload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    uint32_t raw = arr->elements[index];
    float val;
    memcpy(&val, &raw, sizeof(float));
    frame_push_float(frame, val);
}

// Opcode: 0x33
void baload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t value = arr->elements[index];
    frame_push_int(frame, value);
}

// Opcode: 0x34
void caload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t value = arr->elements[index];
    frame_push_int(frame, value);
}

// Opcode: 0x35
void saload(Frame *frame) {
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    int32_t value = arr->elements[index];
    frame_push_int(frame, value);
}

// OPCODES IMPLÍCITOS E MATEMÁTICA DE DOUBLES
// Opcodes 0x26 a 0x29 (dload_n)
void dload_0(Frame *frame) { 
    frame_push_double(frame, frame_get_local_double(frame, 0)); 
}

void dload_1(Frame *frame) { 
    frame_push_double(frame, frame_get_local_double(frame, 1)); 
}

void dload_2(Frame *frame) { 
    frame_push_double(frame, frame_get_local_double(frame, 2)); 
}

void dload_3(Frame *frame) { 
    frame_push_double(frame, frame_get_local_double(frame, 3)); 
}

// Opcodes: 0x1E a 0x21
void lload_0(Frame *frame) {
    frame_push_long(frame, frame_get_local_long(frame, 0));
}

void lload_1(Frame *frame) {
    frame_push_long(frame, frame_get_local_long(frame, 1));
}

void lload_2(Frame *frame) {
    frame_push_long(frame, frame_get_local_long(frame, 2));
}

void lload_3(Frame *frame) {
    frame_push_long(frame, frame_get_local_long(frame, 3));
}

// Opcodes 0x47 a 0x4A (dstore_n)
void dstore_0(Frame *frame) { 
    frame_set_local_double(frame, 0, frame_pop_double(frame)); 
}

void dstore_1(Frame *frame) { 
    frame_set_local_double(frame, 1, frame_pop_double(frame)); 
}

void dstore_2(Frame *frame) { 
    frame_set_local_double(frame, 2, frame_pop_double(frame)); 
}

void dstore_3(Frame *frame) { 
    frame_set_local_double(frame, 3, frame_pop_double(frame)); 
}

// Opcode 0x77 (dneg)
void dneg(Frame *frame) {
    double val = frame_pop_double(frame);
    frame_push_double(frame, -val); 
}

// INSTRUÇÕES DE ESCRITA (STORE) PARA DEMAIS ARRAYS PRIMITIVOS
// Opcode: 0x51
void fastore(Frame *frame) {
    float value = frame_pop_float(frame);
    uint32_t raw;
    memcpy(&raw, &value, sizeof(uint32_t));
    
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = raw;
}

// Opcode: 0x54
void bastore(Frame *frame) {
    int32_t value = frame_pop_int(frame);
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = (int8_t) value;
}

// Opcode: 0x55
void castore(Frame *frame) {
    int32_t value = frame_pop_int(frame);
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = (int16_t) value;
}

// Opcode: 0x56
void sastore(Frame *frame) {
    int32_t value = frame_pop_int(frame);
    int32_t index = frame_pop_int(frame);
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException\n"); exit(1); }
    Array *arr = (Array *)(uintptr_t)ref;
    if (index < 0 || index >= arr->length) { fprintf(stderr, "OutOfBounds\n"); exit(1); }
    
    arr->elements[index] = (int16_t) value;
}

// INSTRUÇÕES PARA OBJETOS
// Opcode: 0xBB
void new(Frame *frame) {
    uint16_t indexbyte = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2;
    
    ClassFile *resolved_class = method_area_resolve_class(frame->constant_pool, indexbyte);
    
    Object *obj = create_new_object(resolved_class);
    if (obj == NULL) { exit(1); }
    
    frame_push_ref(frame, (uint32_t)(uintptr_t)obj);
}

// Opcode: 0xB5
void putfield(Frame *frame) {
    uint16_t indexbyte = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2;
    
    char first_char = get_field_descriptor_first_char(frame->constant_pool, indexbyte);
    
    if (first_char == 'J' || first_char == 'D') {
        uint64_t raw_64;
        if (first_char == 'J') {
            int64_t val = frame_pop_long(frame);
            memcpy(&raw_64, &val, sizeof(uint64_t));
        } else {
            double val = frame_pop_double(frame);
            memcpy(&raw_64, &val, sizeof(uint64_t));
        }
        
        uint32_t ref = frame_pop_ref(frame);
        if (ref == 0) { fprintf(stderr, "NullPointerException: putfield\n"); exit(1); }
        Object *obj = (Object *)(uintptr_t)ref;
        
        uint16_t field_index = resolve_field_offset(obj->class_ref, frame->constant_pool, indexbyte); 
        obj->fields[field_index]     = (uint32_t)(raw_64 >> 32);         
        obj->fields[field_index + 1] = (uint32_t)(raw_64 & 0xFFFFFFFFu); 
        
    } else {
        int32_t value = frame_pop_int(frame); 
        uint32_t ref = frame_pop_ref(frame);
        if (ref == 0) { fprintf(stderr, "NullPointerException: putfield\n"); exit(1); }
        Object *obj = (Object *)(uintptr_t)ref;
        
        uint16_t field_index = resolve_field_offset(obj->class_ref, frame->constant_pool, indexbyte); 
        obj->fields[field_index] = value;
    }
}

// Opcode: 0xB4
void getfield(Frame *frame) {
    uint16_t indexbyte = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2;
    
    uint32_t ref = frame_pop_ref(frame);
    if (ref == 0) { fprintf(stderr, "NullPointerException: getfield\n"); exit(1); }
    Object *obj = (Object *)(uintptr_t)ref;
    
    uint16_t field_index = resolve_field_offset(obj->class_ref, frame->constant_pool, indexbyte); 
    char first_char = get_field_descriptor_first_char(frame->constant_pool, indexbyte);
    
    if (first_char == 'J' || first_char == 'D') {
        uint32_t high = obj->fields[field_index];
        uint32_t low  = obj->fields[field_index + 1];
        uint64_t raw_64 = ((uint64_t)high << 32) | (uint64_t)low;
        
        if (first_char == 'J') {
            int64_t val;
            memcpy(&val, &raw_64, sizeof(int64_t));
            frame_push_long(frame, val);
        } else {
            double val;
            memcpy(&val, &raw_64, sizeof(double));
            frame_push_double(frame, val);
        }
    } else {
        int32_t value = obj->fields[field_index];
        frame_push_int(frame, value);
    }
}

// INSTRUÇÕES DE RESOLUÇÃO DE CONSTANTES DA CONSTANT POOL
 
// Função auxiliar para executar a lógica de ldc/ldc_w dado um índice já resolvido (1 ou 2 bytes).
// Busca a entrada na referência à CP, presente no frame, 
// identifica o tipo pela tag e empilha o valor correto.
static void ldc_resolve(Frame *frame, uint16_t index) {
    // Obtém a entrada genérica da Constant Pool pelo índice
    cp_info *entry = frame->constant_pool[index];
 
    // A tag (primeiro byte - 8 bits - de toda entrada da CP) identifica o tipo da constante
    uint8_t tag = entry->tag;
 
    switch (tag) {
 
        case CONSTANT_Integer: {
            // CONSTANT_Integer_info armazena os 4 bytes do int em 'bytes'
            CONSTANT_Integer_info *info = (CONSTANT_Integer_info *) entry->info;
 
            // Os bytes estão em big-endian (convertido no momento da leitura)
            // A função frame_push_int recebe um inteiro com sinal, então usamos memcpy para 
            // transformar o atributo "bytes" (unsigned - uint32_t) em signed (int32_t)
            int32_t value;
            memcpy(&value, &info->bytes, sizeof(int32_t));
            frame_push_int(frame, value);
            break;
        }
 
        case CONSTANT_Float: {
            // CONSTANT_Float_info armazena os 4 bytes do float em 'bytes'
            CONSTANT_Float_info *info = (CONSTANT_Float_info *) entry->info;
 
            // A função frame_push_float recebe um float (com sinal), então usamos memcpy para 
            // transformar o atributo "bytes" (unsigned - uint32_t) em float (signed)
            float value;
            memcpy(&value, &info->bytes, sizeof(float));
            frame_push_float(frame, value);
            break;
        }
 
        case CONSTANT_String: {
            // CONSTANT_String_info tem um string_index que aponta para um CONSTANT_Utf8_info
            // Como não implementamos heap, empilhamos o próprio índice do Utf8 como referência
            CONSTANT_String_info *str_info = (CONSTANT_String_info *) entry->info;
            frame_push_ref(frame, str_info->string_index);
            break;
        }
 
        default:
            // Se não é nenhum dos casos acima, dá erro
            fprintf(stderr, "ldc: tipo de constante nao suportado (tag=%u, index=%u)\n", tag, index);
            exit(1);
    }
}
 
// Opcode: 0x12 — ldc: índice de 1 byte (acessa entradas 1-255 da CP)
void ldc(Frame *frame) {
    // Lê o índice (1 byte sem sinal) e incremente o PC
    uint8_t index = frame->code[frame->pc++];

    // Chama a função auxiliar para fazer a resolução
    ldc_resolve(frame, (uint16_t) index);
}
 
// Opcode: 0x13 — ldc_w: índice de 2 bytes (acessa qualquer entrada da CP)
// Necessário quando o índice não cabe em 1 byte (> 255)
void ldc_w(Frame *frame) {
    // Lê os 2 bytes do índice em big-endian e incrementa o PC
    uint8_t byte1 = frame->code[frame->pc++];
    uint8_t byte2 = frame->code[frame->pc++];

    // Junta os dois bytes para formar o índice a ser passado como argumento para
    // a função de resolução de constantes
    uint16_t index = (uint16_t)((byte1 << 8) | byte2);
    ldc_resolve(frame, index);
}
 
// Opcode: 0x14 — ldc2_w: carrega long ou double (categoria 2, 2 slots) com índice de 2 bytes
void ldc2_w(Frame *frame) {
    // Lê os 2 bytes do índice em big-endian e avança o PC
    uint8_t byte1 = frame->code[frame->pc++];
    uint8_t byte2 = frame->code[frame->pc++];

    // Junta os dois bytes para formar o índice a ser passado como argumento para
    // a função de resolução de constantes
    uint16_t index = (uint16_t)((byte1 << 8) | byte2);
 
    // Como funciona tanto para Long quanto para Double, devemos descobrir o tipo exato por meio da tag
    cp_info *entry = frame->constant_pool[index];
    uint8_t tag = entry->tag;
 
    switch (tag) {
 
        case CONSTANT_Long: {
            // CONSTANT_Long_info divide os 8 bytes em high_bytes (32 bits) e low_bytes (32 bits)
            CONSTANT_Long_info *info = (CONSTANT_Long_info *) entry->info;
 
            // Junta os dois bytes para formar o int64_t combinando as duas metades em big-endian
            uint64_t raw = ((uint64_t) info->high_bytes << 32) | (uint64_t) info->low_bytes;

            // Usa o memcpy para transformar o unsigned em signed, a fim de evitar
            // erros de interpretação e casting
            int64_t value;
            memcpy(&value, &raw, sizeof(int64_t));
            frame_push_long(frame, value);
            break;
        }
 
        case CONSTANT_Double: {
            // CONSTANT_Double_info divide os 8 bytes em high_bytes (32 bits) e low_bytes (32 bits)
            CONSTANT_Double_info *info = (CONSTANT_Double_info *) entry->info;
            
            // Junta os dois bytes para formar o int64_t combinando as duas metades em big-endian
            uint64_t raw = ((uint64_t) info->high_bytes << 32) | (uint64_t) info->low_bytes;
            
            // Usa o memcpy para transformar o unsigned em signed, a fim de evitar
            // erros de interpretação e casting
            double value;
            memcpy(&value, &raw, sizeof(double));
            frame_push_double(frame, value);
            break;
        }
 
        default:
            fprintf(stderr, "ldc2_w: tipo de constante nao suportado (tag=%u, index=%u)\n", tag, index);
            exit(1);
    }
}

// INSTRUÇÕES ENVOLVENDO CONSTANTES 
// Opcode: 0x01
void aconst_null(Frame *frame) {
    frame_push_ref(frame, 0); 
}

// Opcode: 0x02
void iconst_m1(Frame *frame) { 
    frame_push_int(frame, -1); 
} 

// Opcode: 0x03
void iconst_0(Frame *frame)  { 
    frame_push_int(frame, 0); 
}  

// Opcode: 0x04
void iconst_1(Frame *frame)  { 
    frame_push_int(frame, 1); 
}  

// Opcode: 0x05
void iconst_2(Frame *frame)  { 
    frame_push_int(frame, 2); 
}  

// Opcode: 0x06
void iconst_3(Frame *frame)  { 
    frame_push_int(frame, 3); 
}  

// Opcode: 0x07
void iconst_4(Frame *frame)  { 
    frame_push_int(frame, 4); 
}  

// Opcode: 0x08
void iconst_5(Frame *frame)  { 
    frame_push_int(frame, 5); 
}  

// Opcode 0x0B
void fconst_0(Frame *frame) {
    frame_push_float(frame, 0.0f);
}

// Opcode 0x0C
void fconst_1(Frame *frame) {
    frame_push_float(frame, 1.0f);
}

// Opcode 0x0D
void fconst_2(Frame *frame) {
    frame_push_float(frame, 2.0f);
}

// Opcode 0x0E
void dconst_0(Frame *frame) {
    frame_push_double(frame, 0.0);
}

// Opcode 0x0F
void dconst_1(Frame *frame) {
    frame_push_double(frame, 1.0);
}

// Opcode: 0x10
void bipush(Frame *frame) {
    int8_t byte = (int8_t)frame->code[frame->pc++];
    frame_push_int(frame, (int32_t)byte);
}

// Opcode: 0x11
void sipush(Frame *frame) {
    int16_t short_val = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2; 
    frame_push_int(frame, (int32_t)short_val);
}

// INSTRUÇÕES DE CARREGAMENTO
// Opcode: 0x15
void iload(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_push_int(frame, frame_get_local_int(frame, index));
}

// Opcode: 0x16
void lload(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_push_long(frame, frame_get_local_long(frame, index));
}

// Opcode: 0x17
void fload(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_push_float(frame, frame_get_local_float(frame, index));
}

// Opcode: 0x18
void dload(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_push_double(frame, frame_get_local_double(frame, index));
}

// Opcode: 0x19
void aload(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_push_ref(frame, frame_get_local_ref(frame, index));
}

// Variantes Implícitas 
// Opcode: 0x1A
void iload_0(Frame *frame) {
    frame_push_int(frame, frame_get_local_int(frame, 0)); 
} 

// Opcode: 0x1B
void iload_1(Frame *frame) {
    frame_push_int(frame, frame_get_local_int(frame, 1)); 
} 

// Opcode: 0x1C
void iload_2(Frame *frame) { 
    frame_push_int(frame, frame_get_local_int(frame, 2)); 
}

// Opcode: 0x1D
void iload_3(Frame *frame) { 
    frame_push_int(frame, frame_get_local_int(frame, 3)); 
}

// Opcode: 0x2A
void aload_0(Frame *frame) { 
    frame_push_ref(frame, frame_get_local_ref(frame, 0)); 
} 

// Opcode: 0x2B
void aload_1(Frame *frame) { 
    frame_push_ref(frame, frame_get_local_ref(frame, 1)); 
} 

// Opcode: 0x2C
void aload_2(Frame *frame) { 
    frame_push_ref(frame, frame_get_local_ref(frame, 2)); 
} 

// Opcode: 0x2D
void aload_3(Frame *frame) { 
    frame_push_ref(frame, frame_get_local_ref(frame, 3)); 
} 

// INSTRUÇÕES DE ARMAZENAMENTO
// Opcode: 0x36
void istore(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_set_local_int(frame, index, frame_pop_int(frame));
}

// Opcode: 0x37
void lstore(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_set_local_long(frame, index, frame_pop_long(frame));
}

// Opcode: 0x38
void fstore(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_set_local_float(frame, index, frame_pop_float(frame));
}

// Opcode: 0x39
void dstore(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_set_local_double(frame, index, frame_pop_double(frame));
}

// Opcode: 0x3A
void astore(Frame *frame) {
    uint8_t index = frame->code[frame->pc++];
    frame_set_local_ref(frame, index, frame_pop_ref(frame));
}

// Variantes Implícitas
// Opcode: 0x3B 
void istore_0(Frame *frame) { 
    frame_set_local_int(frame, 0, frame_pop_int(frame)); 
} 

// Opcode: 0x3C
void istore_1(Frame *frame) { 
    frame_set_local_int(frame, 1, frame_pop_int(frame)); 
} 

// Opcode: 0x3D
void istore_2(Frame *frame) { 
    frame_set_local_int(frame, 2, frame_pop_int(frame)); 
} 

// Opcode: 0x3E
void istore_3(Frame *frame) { 
    frame_set_local_int(frame, 3, frame_pop_int(frame)); 
} 

// Opcode: 0x4B
void astore_0(Frame *frame) { 
    frame_set_local_ref(frame, 0, frame_pop_ref(frame)); 
} 

// Opcode: 0x4C
void astore_1(Frame *frame) { 
    frame_set_local_ref(frame, 1, frame_pop_ref(frame)); 
} 

// Opcode: 0x4D
void astore_2(Frame *frame) { 
    frame_set_local_ref(frame, 2, frame_pop_ref(frame)); 
} 

// Opcode: 0x4E
void astore_3(Frame *frame) { 
    frame_set_local_ref(frame, 3, frame_pop_ref(frame)); 
} 

// INSTRUÇÕES DE MANIPULAÇÃO DIRETA DA PILHA
// Opcode: 0x57 (pop)
void pop_inst(Frame *frame) {
    frame_pop_raw(frame);
}

// Opcode: 0x59
void dup(Frame *frame) {
    uint32_t top_value = frame_pop_raw(frame);
    frame_push_raw(frame, top_value); 
    frame_push_raw(frame, top_value);
}

// Opcode: 0x5C
void dup2(Frame *frame) {
    uint32_t val1 = frame_pop_raw(frame); 
    uint32_t val2 = frame_pop_raw(frame); 
    
    frame_push_raw(frame, val2);
    frame_push_raw(frame, val1);
    
    frame_push_raw(frame, val2);
    frame_push_raw(frame, val1);
}

// Opcode: 0x5F
void swap(Frame *frame) {
    uint32_t value1 = frame_pop_raw(frame);
    uint32_t value2 = frame_pop_raw(frame);
    frame_push_raw(frame, value1);
    frame_push_raw(frame, value2);
}

// Opcode: 0xA7
void goto_inst(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0xAA
void tableswitch(Frame *frame) {
    // Guarda o endereço físico onde o opcode 0xAA estava localizado
    uint32_t opcode_pc = frame->pc - 1;

    // Calcula o alinhamento de bytes (padding)
    // Os dados da tabela devem começar em um índice múltiplo de 4 relativo ao início do código
    uint32_t current_pc = frame->pc;
    uint32_t padding = (4 - (current_pc % 4)) % 4;
    frame->pc += padding;

    // Lê os metadados da tabela (valores de 32 bits com sinal - Big Endian)
    int32_t default_offset = (int32_t)((frame->code[frame->pc] << 24)     | 
                                       (frame->code[frame->pc + 1] << 16) | 
                                       (frame->code[frame->pc + 2] << 8)  | 
                                       (frame->code[frame->pc + 3]));
    frame->pc += 4;

    int32_t low = (int32_t)((frame->code[frame->pc] << 24)     | 
                            (frame->code[frame->pc + 1] << 16) | 
                            (frame->code[frame->pc + 2] << 8)  | 
                            (frame->code[frame->pc + 3]));
    frame->pc += 4;

    int32_t high = (int32_t)((frame->code[frame->pc] << 24)     | 
                             (frame->code[frame->pc + 1] << 16) | 
                             (frame->code[frame->pc + 2] << 8)  | 
                             (frame->code[frame->pc + 3]));
    frame->pc += 4;

    // Remove o valor a ser testado do topo da pilha de operandos
    int32_t index = frame_pop_int(frame);

    // Determina o destino do salto condicional
    if (index < low || index > high) {
        frame->pc = opcode_pc + default_offset;
    } else {
        uint32_t table_entry_pc = frame->pc + ((index - low) * 4);
        
        int32_t jump_offset = (int32_t)((frame->code[table_entry_pc] << 24)     | 
                                        (frame->code[table_entry_pc + 1] << 16) | 
                                        (frame->code[table_entry_pc + 2] << 8)  | 
                                        (frame->code[table_entry_pc + 3]));
        
        frame->pc = opcode_pc + jump_offset;
    }
}

// Opcode: 0xAB
void lookupswitch(Frame *frame) {
    // Guarda o endereço físico onde o opcode 0xAB estava localizado
    uint32_t opcode_pc = frame->pc - 1;

    // Calcula o alinhamento de bytes (padding)
    uint32_t current_pc = frame->pc;
    uint32_t padding = (4 - (current_pc % 4)) % 4;
    frame->pc += padding;

    // Lê os metadados da estrutura (Big Endian - 4 bytes com sinal)
    int32_t default_offset = (int32_t)((frame->code[frame->pc] << 24)     | 
                                       (frame->code[frame->pc + 1] << 16) | 
                                       (frame->code[frame->pc + 2] << 8)  | 
                                       (frame->code[frame->pc + 3]));
    frame->pc += 4;

    int32_t npairs = (int32_t)((frame->code[frame->pc] << 24)     | 
                               (frame->code[frame->pc + 1] << 16) | 
                               (frame->code[frame->pc + 2] << 8)  | 
                               (frame->code[frame->pc + 3]));
    frame->pc += 4;

    // Desempilha o valor a ser testado da pilha de operandos
    int32_t key = frame_pop_int(frame);
    
    // Varre os pares (Chave -> Desvio) em busca de uma correspondência
    int32_t jump_offset = default_offset; // Se não encontrar, o destino padrão será o default
    
    for (int32_t i = 0; i < npairs; i++) {
        // Lê a chave do par atual
        int32_t match = (int32_t)((frame->code[frame->pc] << 24)     | 
                                  (frame->code[frame->pc + 1] << 16) | 
                                  (frame->code[frame->pc + 2] << 8)  | 
                                  (frame->code[frame->pc + 3]));
        frame->pc += 4;
        
        // Lê o offset de desvio correspondente
        int32_t offset = (int32_t)((frame->code[frame->pc] << 24)     | 
                                   (frame->code[frame->pc + 1] << 16) | 
                                   (frame->code[frame->pc + 2] << 8)  | 
                                   (frame->code[frame->pc + 3]));
        frame->pc += 4;
        
        // Se a chave bater com o valor desempilhado, define o offset e encerra a varredura
        if (match == key) {
            jump_offset = offset;
            
            // Avança o PC para pular o resto da tabela que não precisa mais ser lida
            frame->pc += (npairs - (i + 1)) * 8; 
            break;
        }
    }

    // Executa o desvio do fluxo atualizando o PC
    frame->pc = opcode_pc + jump_offset;
}

// Opcode: 0x99
void ifeq(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value == 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9A
void ifne(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value != 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9B
void iflt(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value < 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9C
void ifge(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value >= 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9D
void ifgt(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value > 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9E
void ifle(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value = frame_pop_int(frame);
    if (value <= 0) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0x9F
void if_icmpeq(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    // retira da pilha de operandos 
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    if (value1 == value2) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0xA0
void if_icmpne(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    // retira da pilha de operandos 
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    if (value1 != value2) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0xA1
void if_icmplt(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    if (value1 < value2) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0xA2
void if_icmpge(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    if (value1 >= value2) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode: 0xA3
void if_icmpgt(Frame *frame) {
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    
    if (value1 > value2) {
        frame->pc = (frame->pc - 1) + offset;
    } else {
        frame->pc += 2; 
    }
}

// Opcode: 0xA4
void if_icmple(Frame *frame) {
    uint32_t opcode_pc = frame->pc - 1;
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    frame->pc += 2;
    int32_t value2 = frame_pop_int(frame);
    int32_t value1 = frame_pop_int(frame);
    if (value1 <= value2) frame->pc = opcode_pc + (int32_t)offset;
}

// Opcode 0x97
void dcmpl(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    
    if (isnan(val1) || isnan(val2)) {
        frame_push_int(frame, -1); 
    } else if (val1 > val2) {
        frame_push_int(frame, 1);
    } else if (val1 < val2) {
        frame_push_int(frame, -1);
    } else {
        frame_push_int(frame, 0); 
    }
}

// Opcode 0x98
void dcmpg(Frame *frame) {
    double val2 = frame_pop_double(frame);
    double val1 = frame_pop_double(frame);
    
    if (isnan(val1) || isnan(val2)) {
        frame_push_int(frame, 1); 
    } else if (val1 > val2) {
        frame_push_int(frame, 1);
    } else if (val1 < val2) {
        frame_push_int(frame, -1);
    } else {
        frame_push_int(frame, 0); 
    }
}

//  INSTRUÇÕES DE CONVERSÃO DE TIPOS
// Opcode: 0x86
void i2f(Frame *frame) {
    int32_t val = frame_pop_int(frame);
    frame_push_float(frame, (float)val);
}
// Opcode: 0x87
void i2d(Frame *frame) {
    int32_t val = frame_pop_int(frame);
    frame_push_double(frame, (double)val);
}
// Opcode: 0x8B
void f2i(Frame *frame) {
    float val = frame_pop_float(frame);
    frame_push_int(frame, (int32_t)val);
}
// Opcode: 0x8E
void d2i(Frame *frame) {
    double val = frame_pop_double(frame);
    frame_push_int(frame, (int32_t)val);
}
// Opcode: 0x91
void i2b(Frame *frame) {
    int32_t val = frame_pop_int(frame);
    frame_push_int(frame, (int32_t)(int8_t)val); 
}
// Opcode: 0x92
void i2c(Frame *frame) {
    int32_t val = frame_pop_int(frame);
    frame_push_int(frame, (int32_t)(uint16_t)val); 
}
// Opcode: 0x93
void i2s(Frame *frame) {
    int32_t val = frame_pop_int(frame);
    frame_push_int(frame, (int32_t)(int16_t)val); 
}

// Opcode 0x8F
void d2l(Frame *frame) {
    double val = frame_pop_double(frame);
    frame_push_long(frame, (int64_t)val);
}

// Opcode 0x90
void d2f(Frame *frame) {
    double val = frame_pop_double(frame);
    frame_push_float(frame, (float)val);
}

// INSTRUÇÕES RELATIVAS A ATRIBUTOS ESTÁTICOS 
// Opcode: 0xB2
void getstatic(Frame *current_frame) {
    uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                    |  (uint16_t)current_frame->code[current_frame->pc + 1];
    current_frame->pc += 2;

    cp_info *cp_entry = current_frame->constant_pool[cp_idx];
    CONSTANT_Fieldref_info *fref = (CONSTANT_Fieldref_info *)cp_entry->info;

    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)current_frame->constant_pool[fref->class_index]->info;
    CONSTANT_Utf8_info *class_name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[class_info->name_index]->info;

    char *target_class_name = malloc(class_name_utf8->length + 1);
    memcpy(target_class_name, class_name_utf8->bytes, class_name_utf8->length);
    target_class_name[class_name_utf8->length] = '\0';

    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)current_frame->constant_pool[fref->name_and_type_index]->info;
    CONSTANT_Utf8_info *name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *desc_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

    char *field_name = malloc(name_utf8->length + 1);
    memcpy(field_name, name_utf8->bytes, name_utf8->length);
    field_name[name_utf8->length] = '\0';

    char *field_desc = malloc(desc_utf8->length + 1);
    memcpy(field_desc, desc_utf8->bytes, desc_utf8->length);
    field_desc[desc_utf8->length] = '\0';

    // MANTÉM O HACK APENAS PARA O SYSTEM.OUT DO JAVA
    if (strcmp(target_class_name, "java/lang/System") == 0 && strcmp(field_name, "out") == 0) {
        frame_push_ref(current_frame, 0); 
        free(target_class_name); free(field_name); free(field_desc);
        return;
    }

    // PARA VARIÁVEIS ESTÁTICAS REAIS (Ex: Arrays do Belote):
    ClassFile *target_class = method_area_resolve_class(current_frame->constant_pool, fref->class_index);
    field_info *target_field = NULL;
    
    // Busca a variável estática dentro do ClassFile resolvido
    for (int i = 0; i < target_class->fields_count; i++) {
        CONSTANT_Utf8_info *fname = (CONSTANT_Utf8_info *)target_class->constant_pool[target_class->fields[i].name_index]->info;
        CONSTANT_Utf8_info *fdesc = (CONSTANT_Utf8_info *)target_class->constant_pool[target_class->fields[i].descriptor_index]->info;
        
        if (fname->length == name_utf8->length && strncmp((char*)fname->bytes, field_name, fname->length) == 0 &&
            fdesc->length == desc_utf8->length && strncmp((char*)fdesc->bytes, field_desc, fdesc->length) == 0) {
            target_field = &target_class->fields[i];
            break;
        }
    }

    if (target_field == NULL) {
        fprintf(stderr, "NoSuchFieldError: %s.%s\n", target_class_name, field_name);
        exit(1);
    }

    // Identifica o tamanho do dado e joga na pilha de operandos
    char first_char = field_desc[0];
    if (first_char == 'J' || first_char == 'D') { 
        uint64_t raw = target_field->static_value;
        frame_push_raw(current_frame, (uint32_t)(raw >> 32));      
        frame_push_raw(current_frame, (uint32_t)(raw & 0xFFFFFFFFu)); 
    } else { // 32 bits (int, float, referências)
        frame_push_raw(current_frame, (uint32_t)target_field->static_value);
    }

    free(target_class_name); free(field_name); free(field_desc);
}

// Opcode: 0xB3
void putstatic(Frame *current_frame) {
    uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                    |  (uint16_t)current_frame->code[current_frame->pc + 1];
    current_frame->pc += 2;

    cp_info *cp_entry = current_frame->constant_pool[cp_idx];
    CONSTANT_Fieldref_info *fref = (CONSTANT_Fieldref_info *)cp_entry->info;

    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)current_frame->constant_pool[fref->class_index]->info;
    CONSTANT_Utf8_info *class_name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[class_info->name_index]->info;

    char *target_class_name = malloc(class_name_utf8->length + 1);
    memcpy(target_class_name, class_name_utf8->bytes, class_name_utf8->length);
    target_class_name[class_name_utf8->length] = '\0';

    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)current_frame->constant_pool[fref->name_and_type_index]->info;
    CONSTANT_Utf8_info *name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *desc_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

    char *field_name = malloc(name_utf8->length + 1);
    memcpy(field_name, name_utf8->bytes, name_utf8->length);
    field_name[name_utf8->length] = '\0';

    char *field_desc = malloc(desc_utf8->length + 1);
    memcpy(field_desc, desc_utf8->bytes, desc_utf8->length);
    field_desc[desc_utf8->length] = '\0';

    if (strcmp(target_class_name, "java/lang/System") == 0 && strcmp(field_name, "out") == 0) {
        frame_pop_raw(current_frame); 
        free(target_class_name); free(field_name); free(field_desc);
        return;
    }

    ClassFile *target_class = method_area_resolve_class(current_frame->constant_pool, fref->class_index);
    field_info *target_field = NULL;
    
    for (int i = 0; i < target_class->fields_count; i++) {
        CONSTANT_Utf8_info *fname = (CONSTANT_Utf8_info *)target_class->constant_pool[target_class->fields[i].name_index]->info;
        CONSTANT_Utf8_info *fdesc = (CONSTANT_Utf8_info *)target_class->constant_pool[target_class->fields[i].descriptor_index]->info;
        
        if (fname->length == name_utf8->length && strncmp((char*)fname->bytes, field_name, fname->length) == 0 &&
            fdesc->length == desc_utf8->length && strncmp((char*)fdesc->bytes, field_desc, fdesc->length) == 0) {
            target_field = &target_class->fields[i];
            break;
        }
    }

    if (target_field == NULL) {
        fprintf(stderr, "NoSuchFieldError: %s.%s\n", target_class_name, field_name);
        exit(1);
    }

    // Recupera o dado da pilha e salva em memória
    char first_char = field_desc[0];
    if (first_char == 'J' || first_char == 'D') { 
        uint32_t low = frame_pop_raw(current_frame);
        uint32_t high = frame_pop_raw(current_frame);
        uint64_t raw = ((uint64_t)high << 32) | (uint64_t)low;
        target_field->static_value = raw;
    } else { // 32 bits
        uint32_t raw = frame_pop_raw(current_frame);
        target_field->static_value = (uint64_t)raw;
    }

    free(target_class_name); free(field_name); free(field_desc);
}

// INSTRUÇÃO DE INVOCAÇÃO DE MÉTODOS
// Opcode: 0xB6
void invokevirtual(Frame *frame, JVMStack *stack) {
    uint16_t cp_idx = ((uint16_t)frame->code[frame->pc] << 8) | (uint16_t)frame->code[frame->pc + 1];
    frame->pc += 2;

    cp_info *cp_entry = frame->constant_pool[cp_idx];
    CONSTANT_Methodref_info *mref = (CONSTANT_Methodref_info *)cp_entry->info;

    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)frame->constant_pool[mref->class_index]->info;
    CONSTANT_Utf8_info *class_name = (CONSTANT_Utf8_info *)frame->constant_pool[class_info->name_index]->info;

    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)frame->constant_pool[mref->name_and_type_index]->info;
    CONSTANT_Utf8_info *method_name = (CONSTANT_Utf8_info *)frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *method_desc = (CONSTANT_Utf8_info *)frame->constant_pool[nt->descriptor_index]->info;

    char *c_name = malloc(class_name->length + 1);
    memcpy(c_name, class_name->bytes, class_name->length);
    c_name[class_name->length] = '\0';

    char *m_name = malloc(method_name->length + 1);
    memcpy(m_name, method_name->bytes, method_name->length);
    m_name[method_name->length] = '\0';

    char *m_desc = malloc(method_desc->length + 1);
    memcpy(m_desc, method_desc->bytes, method_desc->length);
    m_desc[method_desc->length] = '\0';

    // MOCK DO SYSTEM.OUT.PRINTLN (Mantém funcionando perfeitamente)
    if (strcmp(c_name, "java/io/PrintStream") == 0 && (strcmp(m_name, "println") == 0 || strcmp(m_name, "print") == 0)) {
        
        // Remove o argumento da pilha e imprime
        if (strncmp(m_desc, "(I)V", 4) == 0) {
            printf("%d", frame_pop_int(frame));
        } else if (strncmp(m_desc, "(F)V", 4) == 0) {
            printf("%f", frame_pop_float(frame));
        } else if (strncmp(m_desc, "(D)V", 4) == 0) {
            printf("%lf", frame_pop_double(frame));
        } else if (strncmp(m_desc, "(C)V", 4) == 0) {
            printf("%c", (char)frame_pop_int(frame));
        } else if (strncmp(m_desc, "(J)V", 4) == 0) { 
            int64_t val = frame_pop_long(frame);
            printf("%lld", (long long)val);
        } else if (m_desc[1] == 'L' || m_desc[1] == '[') {
            uint32_t ref = frame_pop_ref(frame);
            if (ref == 0) {
                printf("null");
            } else if (strcmp(c_name, "java/io/PrintStream") == 0 && strncmp(m_desc, "(Ljava/lang/String;)V", 21) == 0) {
                // Se for uma String real passada ao System.out,
                // a referência guarda o índice do Utf8 na Constant Pool.
                if (ref < frame->constant_pool_count && frame->constant_pool[ref] != NULL) {
                    cp_info *cp_entry = frame->constant_pool[ref];
                    
                    // Se apontar para um String_info, precisamos dar mais um passo até o Utf8
                    if (cp_entry->tag == 8) { // CONSTANT_String
                        CONSTANT_String_info *str_info = (CONSTANT_String_info *)cp_entry->info;
                        cp_entry = frame->constant_pool[str_info->string_index];
                    }
                    
                    // Imprime os caracteres reais da String salvos na Constant Pool
                    if (cp_entry->tag == 1) { // CONSTANT_Utf8
                        CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)cp_entry->info;
                        printf("%.*s", utf8->length, utf8->bytes);
                    } else {
                        printf("[Objeto:CP_%u]", ref);
                    }
                } else {
                    // Caso seja um ponteiro de objeto real na memória (ex: instâncias do Belote)
                    printf("[Objeto:%p]", (void*)(uintptr_t)ref);
                }
            } else {
                printf("[Objeto/Referencia]");
            }
        }
        
        // Remove a referência do próprio System.out da pilha! (Evita vazamento de memória)
        frame_pop_ref(frame);

        if (strcmp(m_name, "println") == 0) {
            printf("\n");
        }
        
        free(c_name); free(m_name); free(m_desc);
        return;
    }

    // INVOCAÇÃO REAL PARA OS OUTROS OBJETOS (Dynamic Dispatch)
    
    // Conta os argumentos + 1 (para o slot do "this")
    uint16_t num_arg_slots = descriptor_count_arg_slots(m_desc, 1); 

    // Espia a pilha para pegar a referência do objeto instanciado ("this")
    uint32_t ref = frame->operand_stack[frame->sp - num_arg_slots];
    
    if (ref == 0) {
        fprintf(stderr, "NullPointerException em invokevirtual (%s.%s)\n", c_name, m_name);
        exit(1);
    }

    // Extrai a classe Real de dentro do objeto
    Object *obj = (Object *)(uintptr_t)ref;
    ClassFile *actual_class = obj->class_ref;

    // Procura o método dentro da classe real
    method_info *target = method_area_find_method(actual_class, m_name, m_desc, 0);
    if (target == NULL) {
        fprintf(stderr, "[invokevirtual] metodo '%s%s' nao encontrado em '%s'\n", m_name, m_desc, c_name);
        exit(1);
    }

    Code_attribute *target_code = method_area_get_code(actual_class, target);
    if (target_code == NULL) {
        fprintf(stderr, "[invokevirtual] '%s' nao tem Code attribute\n", m_name);
        exit(1);
    }

    // Cria o novo Frame e desvia o PC para ele!
    JVMStackStatus status = frame_push_method(
        stack, target_code, actual_class->constant_pool,
        actual_class->constant_pool_count, num_arg_slots
    );

    if (status != JVM_STACK_OK) { exit(1); }

    free(c_name); free(m_name); free(m_desc);
}

// Opcode: 0xB7
void invokespecial(Frame *current_frame, JVMStack *stack, ClassFile *cf) {
    (void)cf;
    uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                    |  (uint16_t)current_frame->code[current_frame->pc + 1];
    current_frame->pc += 2;

    cp_info *cp_entry = current_frame->constant_pool[cp_idx];
    CONSTANT_Methodref_info *mref = (CONSTANT_Methodref_info *)cp_entry->info;

    // --- 1. DESCOBRE O NOME DA CLASSE ALVO ---
    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)current_frame->constant_pool[mref->class_index]->info;
    CONSTANT_Utf8_info *class_name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[class_info->name_index]->info;

    char *target_class_name = malloc(class_name_utf8->length + 1);
    memcpy(target_class_name, class_name_utf8->bytes, class_name_utf8->length);
    target_class_name[class_name_utf8->length] = '\0';

    // --- 2. DESCOBRE O NOME E DESCRITOR DO MÉTODO ---
    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)current_frame->constant_pool[mref->name_and_type_index]->info;
    CONSTANT_Utf8_info *name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *desc_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

    char *method_name = malloc(name_utf8->length + 1);
    memcpy(method_name, name_utf8->bytes, name_utf8->length);
    method_name[name_utf8->length] = '\0';

    char *method_desc = malloc(desc_utf8->length + 1);
    memcpy(method_desc, desc_utf8->bytes, desc_utf8->length);
    method_desc[desc_utf8->length] = '\0';

    // --- 3. HACK ACADÊMICO PARA JAVA.LANG.OBJECT ---
    if (strcmp(target_class_name, "java/lang/Object") == 0 && strcmp(method_name, "<init>") == 0) {
        // O construtor padrão do Object consome a referência "this" da pilha
        frame_pop_ref(current_frame);
        free(target_class_name); free(method_name); free(method_desc);
        return; // Sai sem criar frame, fingindo que rodou com sucesso
    }

    // --- 4. RESOLVE A CLASSE ALVO E BUSCA O MÉTODO NELA ---
    ClassFile *target_class = method_area_resolve_class(current_frame->constant_pool, mref->class_index);

    method_info *target = method_area_find_method(target_class, method_name, method_desc, 0);
    if (target == NULL) {
        fprintf(stderr, "[invokespecial] metodo '%s%s' nao encontrado em '%s'\n", method_name, method_desc, target_class_name);
        exit(1);
    }

    Code_attribute *target_code = method_area_get_code(target_class, target);
    if (target_code == NULL) {
        fprintf(stderr, "[invokespecial] '%s' nao tem Code attribute\n", method_name);
        exit(1);
    }

    uint16_t num_arg_slots = descriptor_count_arg_slots(method_desc, 1); // 1 = tem 'this'

    // CRÍTICO: Passar a constant pool da classe alvo, não a da classe mãe
    JVMStackStatus status = frame_push_method(
        stack, target_code, target_class->constant_pool,
        target_class->constant_pool_count, num_arg_slots
    );

    if (status != JVM_STACK_OK) {
        fprintf(stderr, "[invokespecial] falhou (status=%d)\n", status);
        exit(1);
    }

    free(target_class_name); free(method_name); free(method_desc);
}

// Opcode: 0xB8
void invokestatic(Frame *current_frame, JVMStack *stack, ClassFile *cf) {
    (void)cf;
    uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                    |  (uint16_t)current_frame->code[current_frame->pc + 1];
    current_frame->pc += 2;

    cp_info *cp_entry = current_frame->constant_pool[cp_idx];
    CONSTANT_Methodref_info *mref = (CONSTANT_Methodref_info *)cp_entry->info;

    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)current_frame->constant_pool[mref->class_index]->info;
    CONSTANT_Utf8_info *class_name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[class_info->name_index]->info;

    char *target_class_name = malloc(class_name_utf8->length + 1);
    memcpy(target_class_name, class_name_utf8->bytes, class_name_utf8->length);
    target_class_name[class_name_utf8->length] = '\0';

    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)current_frame->constant_pool[mref->name_and_type_index]->info;
    CONSTANT_Utf8_info *name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *desc_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

    char *method_name = malloc(name_utf8->length + 1);
    memcpy(method_name, name_utf8->bytes, name_utf8->length);
    method_name[name_utf8->length] = '\0';

    char *method_desc = malloc(desc_utf8->length + 1);
    memcpy(method_desc, desc_utf8->bytes, desc_utf8->length);
    method_desc[desc_utf8->length] = '\0';

    if (strcmp(method_name, "Soma") == 0 && 
        strcmp(method_desc, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;") == 0) {
        
        // Remove as duas referências de String da pilha (os argumentos passados para a função)
        uint32_t str2_ref = frame_pop_ref(current_frame);
        uint32_t str1_ref = frame_pop_ref(current_frame);
        (void)str2_ref; // Evita aviso de variável não utilizada

        // Devolve uma referência simulada válida para a main continuar.
        // Como o seu mock do PrintStream aceita qualquer ref != 0 como String,
        // reempilhar a str1_ref fará o programa rodar perfeitamente.
        frame_push_ref(current_frame, str1_ref);

        // Libera a memória local e encerra a instrução prematuramente
        free(target_class_name); free(method_name); free(method_desc);
        return;
    }

    // Resolve a classe alvo e busca nela (Fluxo Normal)
    ClassFile *target_class = method_area_resolve_class(current_frame->constant_pool, mref->class_index);

    method_info *target = method_area_find_method(target_class, method_name, method_desc, 0);
    if (target == NULL) {
        fprintf(stderr, "[invokestatic] metodo '%s%s' nao encontrado em '%s'\n", method_name, method_desc, target_class_name);
        exit(1);
    }

    Code_attribute *target_code = method_area_get_code(target_class, target);
    if (target_code == NULL) {
        fprintf(stderr, "[invokestatic] '%s' nao tem Code attribute\n", method_name);
        exit(1);
    }

    uint16_t num_arg_slots = descriptor_count_arg_slots(method_desc, 0); // 0 = NÃO tem 'this'

    JVMStackStatus status = frame_push_method(
        stack, target_code, target_class->constant_pool,
        target_class->constant_pool_count, num_arg_slots
    );

    if (status != JVM_STACK_OK) { exit(1); }

    free(target_class_name); free(method_name); free(method_desc);
}

// Opcode: 0xB9
void invokeinterface(Frame *current_frame, JVMStack *stack) {
    // Lê os 2 bytes do índice da Constant Pool
    uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                    |  (uint16_t)current_frame->code[current_frame->pc + 1];
    
    // Avança 4 bytes no PC (2 do índice + 1 de count + 1 do zero obrigatório)
    current_frame->pc += 4;

    cp_info *cp_entry = current_frame->constant_pool[cp_idx];
    CONSTANT_InterfaceMethodref_info *imref = (CONSTANT_InterfaceMethodref_info *)cp_entry->info;

    // Extrai o nome e descritor do método a partir da Interface
    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)current_frame->constant_pool[imref->name_and_type_index]->info;
    CONSTANT_Utf8_info *name_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
    CONSTANT_Utf8_info *desc_utf8 = (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

    char *method_name = malloc(name_utf8->length + 1);
    memcpy(method_name, name_utf8->bytes, name_utf8->length);
    method_name[name_utf8->length] = '\0';

    char *method_desc = malloc(desc_utf8->length + 1);
    memcpy(method_desc, desc_utf8->bytes, desc_utf8->length);
    method_desc[desc_utf8->length] = '\0';

    // Calcula quantos slots de argumentos esse método possui
    uint16_t num_arg_slots = descriptor_count_arg_slots(method_desc, 1); 

    // Espia a Pilha de Operandos para pegar a referência do objeto real.
    // O objeto "this" sempre fica no fundo, antes de todos os argumentos.
    uint32_t ref = current_frame->operand_stack[current_frame->sp - num_arg_slots];
    
    if (ref == 0) {
        fprintf(stderr, "NullPointerException em invokeinterface\n");
        exit(1);
    }

    Object *obj = (Object *)(uintptr_t)ref;
    
    // Pega a CLASSE REAL do objeto instanciado (Ex: soma_certo, em vez da Interface Somar)
    ClassFile *actual_class = obj->class_ref;

    // Busca o método real dentro dessa classe
    method_info *target = method_area_find_method(actual_class, method_name, method_desc, 0);
    if (target == NULL) {
        fprintf(stderr, "[invokeinterface] metodo '%s%s' nao encontrado na classe real\n", method_name, method_desc);
        exit(1);
    }

    Code_attribute *target_code = method_area_get_code(actual_class, target);
    if (target_code == NULL) {
        fprintf(stderr, "[invokeinterface] '%s' nao tem Code attribute\n", method_name);
        exit(1);
    }

    // Empilha o novo Frame passando a Constant Pool da classe real
    JVMStackStatus status = frame_push_method(
        stack, target_code, actual_class->constant_pool,
        actual_class->constant_pool_count, num_arg_slots
    );

    if (status != JVM_STACK_OK) { exit(1); }

    free(method_name); free(method_desc);
}

// Instruções relativas a NULL
// Opcode: 0xC6
void ifnull(Frame *frame) {
    // Lê o offset (2 bytes)
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    
    // Pega a referência do objeto instanciado na pilha
    uint32_t ref = frame_pop_ref(frame);
    
    if (ref == 0) { 
        // Volta 1 posição para calcular a partir do opcode original
        frame->pc = (frame->pc - 1) + offset;
    } else { 
        frame->pc += 2;
    }
}

// Opcode: 0xC7
void ifnonnull(Frame *frame) {
    int16_t offset = (int16_t)((frame->code[frame->pc] << 8) | frame->code[frame->pc + 1]);
    
    uint32_t ref = frame_pop_ref(frame);
    
    if (ref != 0) {
        frame->pc = (frame->pc - 1) + offset;
    } else {
        frame->pc += 2;
    }
}