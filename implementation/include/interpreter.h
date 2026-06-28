/**
 * @file interpreter.h
 * @brief Motor de execução da JVM: laço Fetch-Decode-Execute.
 *
 * Declara a função principal do interpretador, responsável por conduzir
 * o ciclo de execução de bytecode enquanto houver frames na JVMStack.
 * A cada iteração, o opcode apontado pelo PC do frame corrente é buscado,
 * decodificado e despachado para a função de instrução correspondente.
 * O resultado de cada instrução reflete-se na pilha de operandos e nas
 * variáveis locais do frame, e o PC é avançado conforme necessário.
 * Toda a execução é registrada no arquivo de log "execucao_jvm.txt".
 */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "jvm_stack.h"
#include "class_file.h"

/**
 * @brief Inicia e conduz o laço de execução Fetch-Decode-Execute da JVM.
 *
 * Executa continuamente enquanto a @p stack não estiver vazia:
 * -# **Fetch**: lê o opcode no offset @p pc do bytecode do frame corrente.
 * -# **Decode**: identifica a instrução pelo opcode via tabela de despacho (switch).
 * -# **Execute**: invoca a função de instrução correspondente, que opera sobre
 *    a pilha de operandos e as variáveis locais do frame.
 *
 * Ao final de cada instrução de retorno de método (`ireturn`, `lreturn`, `return`, etc.),
 * o frame é desempilhado e a execução continua no frame chamador até que a pilha
 * esvazie, indicando o término do programa.
 *
 * @param stack Ponteiro para a JVMStack já inicializada e com o frame inicial empilhado.
 * @param cf    ClassFile da classe principal, repassado às instruções que necessitam
 *              de resolução de classe (invokestatic, invokespecial).
 */
void execute_engine(JVMStack *stack, ClassFile *cf);

#endif
