#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include "frame.h"
#include "java_types.h"


// Opcodes Matemáticos 
// Aritmética de Inteiros (32 bits)
void iadd(Frame *frame);
void isub(Frame *frame);
void imul(Frame *frame);
void idiv(Frame *frame);
void irem(Frame *frame);

// Aritmética de Longs (64 bits)
void ladd(Frame *frame);
void lsub(Frame *frame);
void lmul(Frame *frame);
void execute_ldiv(Frame *frame);
void lrem(Frame *frame);

// Aritmética de Doubles (64 bits)
void dadd(Frame *frame);
void dsub(Frame *frame);
void dmul(Frame *frame);
void ddiv(Frame *frame);
void drem(Frame *frame);

// Operações Lógicas / Bit a Bit para Inteiros
void iand(Frame *frame);
void ior(Frame *frame);
void ixor(Frame *frame);
void ishl(Frame *frame);
void ishr(Frame *frame);
void iushr(Frame *frame);

// Instruções relativas aos Arrays
void newarray(Frame *frame);
void anewarray(Frame *frame);
void arraylength(Frame *frame);
void iaload(Frame *frame);
void aaload(Frame *frame);
void iastore(Frame *frame);
void aastore(Frame *frame);

// Adições: Opcodes para Arrays de 64 bits
void laload(Frame *frame);
void daload(Frame *frame);
void lastore(Frame *frame);
void dastore(Frame *frame);

// Opcodes para os demais tipos de Arrays primitivos
void faload(Frame *frame);
void baload(Frame *frame);
void caload(Frame *frame);
void saload(Frame *frame);

void fastore(Frame *frame);
void bastore(Frame *frame);
void castore(Frame *frame);
void sastore(Frame *frame);

// Instruções relativas aos Objetos
void new(Frame *frame);
void putfield(Frame *frame);
void getfield(Frame *frame);
#endif