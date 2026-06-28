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
void iinc(Frame *frame);

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

// Double Loads
void dload_0(Frame *frame);
void dload_1(Frame *frame);
void dload_2(Frame *frame);
void dload_3(Frame *frame);

// Double Stores
void dstore_0(Frame *frame);
void dstore_1(Frame *frame);
void dstore_2(Frame *frame);
void dstore_3(Frame *frame);

// Negação Matemática de Double
void dneg(Frame *frame);

void fastore(Frame *frame);
void bastore(Frame *frame);
void castore(Frame *frame);
void sastore(Frame *frame);

// Instruções relativas aos Objetos
void new(Frame *frame);
void putfield(Frame *frame);
void getfield(Frame *frame);

// Instruções de resolução de constantes da Constant Pool
void ldc(Frame *frame);
void ldc_w(Frame *frame);
void ldc2_w(Frame *frame);

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
void dconst_0(Frame *frame);
void dconst_1(Frame *frame);

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

// Instrução de Desvio incondicional
void goto_inst(Frame *frame);

// Instruções de comparação com zero
void ifeq(Frame *frame);
void ifne(Frame *frame);
void iflt(Frame *frame);
void ifge(Frame *frame);
void ifgt(Frame *frame);
void ifle(Frame *frame);

// Instruções de comparação entre dois valores:
void if_icmpeq(Frame *frame);
void if_icmpne(Frame *frame);
void if_icmplt(Frame *frame);
void if_icmpge(Frame *frame);
void if_icmpgt(Frame *frame);
void if_icmple(Frame *frame);
void dcmpl(Frame *frame);
void dcmpg(Frame *frame);

// Instruções de Conversão de Tipos
void i2f(Frame *frame);
void i2d(Frame *frame);
void f2i(Frame *frame);
void d2i(Frame *frame);
void i2b(Frame *frame);
void i2c(Frame *frame);
void i2s(Frame *frame);
void d2i(Frame *frame);
void d2l(Frame *frame);
void d2f(Frame *frame);

// Instruções Relativas a Atributos Estáticos 
void getstatic(Frame *frame);
void putstatic(Frame *frame);

// Instrução de Invocação 
void invokevirtual(Frame *frame, JVMStack *stack);
void invokespecial(Frame *frame, JVMStack *stack, ClassFile *cf);
void invokestatic(Frame *frame, JVMStack *stack, ClassFile *cf);
void invokeinterface(Frame *frame, JVMStack *stack);

// Instruções relativas a NULL
void ifnull(Frame *frame);
void ifnonnull(Frame *frame);

#endif