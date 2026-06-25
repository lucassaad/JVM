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


// Instruções envolvendo constantes
void aconst_null(Frame *frame);
void iconst_m1(Frame *frame);
void iconst_0(Frame *frame);
void iconst_1(Frame *frame);
void iconst_2(Frame *frame);
void iconst_3(Frame *frame);
void iconst_4(Frame *frame);
void iconst_5(Frame *frame);
void bipush(Frame *frame);
void sipush(Frame *frame);

// Instruções de Carregamento
void iload(Frame *frame);
void lload(Frame *frame);
void fload(Frame *frame);
void dload(Frame *frame);
void aload(Frame *frame);

// Variantes implícitas 
void iload_0(Frame *frame);
void iload_1(Frame *frame);
void iload_2(Frame *frame);
void iload_3(Frame *frame);

void aload_0(Frame *frame);
void aload_1(Frame *frame);
void aload_2(Frame *frame);
void aload_3(Frame *frame);

// Instruções de Armazenamento
void istore(Frame *frame);
void lstore(Frame *frame);
void fstore(Frame *frame);
void dstore(Frame *frame);
void astore(Frame *frame);

// Variantes implícitas
void istore_0(Frame *frame);
void istore_1(Frame *frame);
void istore_2(Frame *frame);
void istore_3(Frame *frame);

void astore_0(Frame *frame);
void astore_1(Frame *frame);
void astore_2(Frame *frame);
void astore_3(Frame *frame);

// Instruções de Manipulação da Pilha
void pop_inst(Frame *frame); 
void dup(Frame *frame);
void swap(Frame *frame);

#endif