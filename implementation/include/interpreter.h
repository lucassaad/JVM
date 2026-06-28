#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "jvm_stack.h"
#include "class_file.h"

// Função para iniciar o laço de execução Fetch-Decode-Execute
void execute_engine(JVMStack *stack, ClassFile *cf);

#endif 