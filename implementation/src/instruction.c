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