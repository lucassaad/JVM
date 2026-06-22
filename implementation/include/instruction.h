#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include "frame.h"
#include "java_types.h"

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