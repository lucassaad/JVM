/**
 * @file instruction.h
 * @brief Declarações de todas as instruções bytecode implementadas pela JVM.
 *
 * Cada função corresponde diretamente a um opcode JVM e recebe o Frame corrente
 * (e, quando necessário, a JVMStack e o ClassFile) para executar sua operação.
 * Os operandos inline do bytecode são lidos diretamente do campo @p code do frame,
 * avançando o PC conforme necessário.
 *
 * As funções estão organizadas nas seguintes categorias:
 * - Aritmética (int, long, double)
 * - Operações lógicas e bit a bit
 * - Arrays (criação, acesso, armazenamento)
 * - Objetos (criação, campos de instância)
 * - Constantes (pool, literais, push imediato)
 * - Carga e armazenamento de variáveis locais
 * - Manipulação da pilha de operandos
 * - Controle de fluxo (desvios, switch, comparações)
 * - Conversão de tipos
 * - Campos estáticos
 * - Invocação de métodos
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include "frame.h"
#include "java_types.h"

// ---------------------------------------------------------------------------------------
// Aritmética de Inteiros (32 bits)
// ---------------------------------------------------------------------------------------

/** @brief `iadd` — soma dois ints do topo da pilha e empilha o resultado. */
void iadd(Frame *frame);
/** @brief `isub` — subtrai o int do topo pelo abaixo dele e empilha o resultado. */
void isub(Frame *frame);
/** @brief `imul` — multiplica dois ints do topo da pilha e empilha o resultado. */
void imul(Frame *frame);
/** @brief `idiv` — divide o segundo int pelo do topo (inteiro) e empilha o quociente. Aborta em divisão por zero. */
void idiv(Frame *frame);
/** @brief `irem` — calcula o resto da divisão inteira entre dois ints e empilha o resultado. Aborta em divisão por zero. */
void irem(Frame *frame);
/** @brief `iinc` — incrementa uma variável local int pelo valor imediato com sinal lido do bytecode. */
void iinc(Frame *frame);

// ---------------------------------------------------------------------------------------
// Aritmética de Longs (64 bits)
// ---------------------------------------------------------------------------------------

/** @brief `ladd` — soma dois longs (2 slots cada) do topo da pilha e empilha o resultado. */
void ladd(Frame *frame);
/** @brief `lsub` — subtrai longs e empilha o resultado. */
void lsub(Frame *frame);
/** @brief `lmul` — multiplica longs e empilha o resultado. */
void lmul(Frame *frame);
/** @brief `ldiv` — divide longs e empilha o quociente. Aborta em divisão por zero. */
void execute_ldiv(Frame *frame);
/** @brief `lrem` — calcula o resto da divisão entre longs e empilha o resultado. Aborta em divisão por zero. */
void lrem(Frame *frame);

// ---------------------------------------------------------------------------------------
// Aritmética de Doubles (64 bits)
// ---------------------------------------------------------------------------------------

/** @brief `dadd` — soma dois doubles (2 slots cada) e empilha o resultado. */
void dadd(Frame *frame);
/** @brief `dsub` — subtrai doubles e empilha o resultado. */
void dsub(Frame *frame);
/** @brief `dmul` — multiplica doubles e empilha o resultado. */
void dmul(Frame *frame);
/** @brief `ddiv` — divide doubles e empilha o resultado. */
void ddiv(Frame *frame);
/** @brief `drem` — calcula o resto da divisão entre doubles e empilha o resultado. */
void drem(Frame *frame);

// ---------------------------------------------------------------------------------------
// Operações Lógicas / Bit a Bit para Inteiros
// ---------------------------------------------------------------------------------------

/** @brief `iand` — AND bit a bit entre dois ints e empilha o resultado. */
void iand(Frame *frame);
/** @brief `ior`  — OR bit a bit entre dois ints e empilha o resultado. */
void ior(Frame *frame);
/** @brief `ixor` — XOR bit a bit entre dois ints e empilha o resultado. */
void ixor(Frame *frame);
/** @brief `ishl` — desloca o int para a esquerda pelo número de bits do topo da pilha. */
void ishl(Frame *frame);
/** @brief `ishr` — desloca o int para a direita com extensão de sinal (aritmético). */
void ishr(Frame *frame);
/** @brief `iushr` — desloca o int para a direita sem extensão de sinal (lógico). */
void iushr(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções relativas a Arrays
// ---------------------------------------------------------------------------------------

/** @brief `newarray` — cria um array de tipo primitivo (lido do bytecode) com o tamanho do topo da pilha. */
void newarray(Frame *frame);
/** @brief `anewarray` — cria um array de referências da classe indicada no constant pool. */
void anewarray(Frame *frame);
/** @brief `arraylength` — empilha o comprimento do array referenciado no topo da pilha. */
void arraylength(Frame *frame);
/** @brief `multianewarray` — cria um array multidimensional (suporte a 2D) com dimensões lidas da pilha. */
void multianewarray_inst(Frame *frame);

/** @brief `iaload` — carrega um int de um array de ints e empilha o valor. */
void iaload(Frame *frame);
/** @brief `aaload` — carrega uma referência de um array de referências e empilha o valor. */
void aaload(Frame *frame);
/** @brief `iastore` — armazena um int em um array de ints. */
void iastore(Frame *frame);
/** @brief `aastore` — armazena uma referência em um array de referências. */
void aastore(Frame *frame);

/** @brief `laload` — carrega um long (2 slots) de um array de longs e empilha o valor. */
void laload(Frame *frame);
/** @brief `daload` — carrega um double (2 slots) de um array de doubles e empilha o valor. */
void daload(Frame *frame);
/** @brief `lastore` — armazena um long em um array de longs. */
void lastore(Frame *frame);
/** @brief `dastore` — armazena um double em um array de doubles. */
void dastore(Frame *frame);

/** @brief `faload`  — carrega um float de um array de floats e empilha o valor. */
void faload(Frame *frame);
/** @brief `baload`  — carrega um byte/boolean de um array e empilha como int com extensão de sinal. */
void baload(Frame *frame);
/** @brief `caload`  — carrega um char de um array e empilha como int sem extensão de sinal. */
void caload(Frame *frame);
/** @brief `saload`  — carrega um short de um array e empilha como int com extensão de sinal. */
void saload(Frame *frame);

/** @brief `fastore` — armazena um float em um array de floats. */
void fastore(Frame *frame);
/** @brief `bastore` — armazena um byte/boolean em um array de bytes. */
void bastore(Frame *frame);
/** @brief `castore` — armazena um char em um array de chars. */
void castore(Frame *frame);
/** @brief `sastore` — armazena um short em um array de shorts. */
void sastore(Frame *frame);

// ---------------------------------------------------------------------------------------
// Load/Store implícitos para double e long (slots fixos 0–3)
// ---------------------------------------------------------------------------------------

/** @brief `dload_0` — carrega double das variáveis locais 0–1 e empilha. */
void dload_0(Frame *frame);
/** @brief `dload_1` — carrega double das variáveis locais 1–2 e empilha. */
void dload_1(Frame *frame);
/** @brief `dload_2` — carrega double das variáveis locais 2–3 e empilha. */
void dload_2(Frame *frame);
/** @brief `dload_3` — carrega double das variáveis locais 3–4 e empilha. */
void dload_3(Frame *frame);

/** @brief `lload_0` — carrega long das variáveis locais 0–1 e empilha. */
void lload_0(Frame *frame);
/** @brief `lload_1` — carrega long das variáveis locais 1–2 e empilha. */
void lload_1(Frame *frame);
/** @brief `lload_2` — carrega long das variáveis locais 2–3 e empilha. */
void lload_2(Frame *frame);
/** @brief `lload_3` — carrega long das variáveis locais 3–4 e empilha. */
void lload_3(Frame *frame);

/** @brief `dstore_0` — desempilha double e armazena nas variáveis locais 0–1. */
void dstore_0(Frame *frame);
/** @brief `dstore_1` — desempilha double e armazena nas variáveis locais 1–2. */
void dstore_1(Frame *frame);
/** @brief `dstore_2` — desempilha double e armazena nas variáveis locais 2–3. */
void dstore_2(Frame *frame);
/** @brief `dstore_3` — desempilha double e armazena nas variáveis locais 3–4. */
void dstore_3(Frame *frame);

/** @brief `dneg` — nega o double do topo da pilha e empilha o resultado. */
void dneg(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções relativas a Objetos
// ---------------------------------------------------------------------------------------

/** @brief `new` — cria uma nova instância da classe indicada no constant pool e empilha a referência. */
void new(Frame *frame);
/** @brief `putfield` — desempilha valor e referência de objeto e armazena o valor no campo de instância indicado. */
void putfield(Frame *frame);
/** @brief `getfield` — desempilha referência de objeto e empilha o valor do campo de instância indicado. */
void getfield(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções de resolução de constantes da Constant Pool
// ---------------------------------------------------------------------------------------

/** @brief `ldc`   — empilha constante int, float ou referência String do constant pool (índice de 1 byte). */
void ldc(Frame *frame);
/** @brief `ldc_w` — empilha constante int, float ou referência String do constant pool (índice de 2 bytes). */
void ldc_w(Frame *frame);
/** @brief `ldc2_w` — empilha constante long ou double (2 slots) do constant pool (índice de 2 bytes). */
void ldc2_w(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções de constantes literais
// ---------------------------------------------------------------------------------------

/** @brief `aconst_null` — empilha a referência nula (null). */
void aconst_null(Frame *frame);
/** @brief `iconst_m1` — empilha a constante int -1. */
void iconst_m1(Frame *frame);
/** @brief `iconst_0`  — empilha a constante int 0. */
void iconst_0(Frame *frame);
/** @brief `iconst_1`  — empilha a constante int 1. */
void iconst_1(Frame *frame);
/** @brief `iconst_2`  — empilha a constante int 2. */
void iconst_2(Frame *frame);
/** @brief `iconst_3`  — empilha a constante int 3. */
void iconst_3(Frame *frame);
/** @brief `iconst_4`  — empilha a constante int 4. */
void iconst_4(Frame *frame);
/** @brief `iconst_5`  — empilha a constante int 5. */
void iconst_5(Frame *frame);
/** @brief `fconst_0`  — empilha a constante float 0.0. */
void fconst_0(Frame *frame);
/** @brief `fconst_1`  — empilha a constante float 1.0. */
void fconst_1(Frame *frame);
/** @brief `fconst_2`  — empilha a constante float 2.0. */
void fconst_2(Frame *frame);
/** @brief `dconst_0`  — empilha a constante double 0.0 (2 slots). */
void dconst_0(Frame *frame);
/** @brief `dconst_1`  — empilha a constante double 1.0 (2 slots). */
void dconst_1(Frame *frame);
/** @brief `bipush` — estende com sinal o byte imediato do bytecode para int e empilha. */
void bipush(Frame *frame);
/** @brief `sipush` — estende com sinal o short imediato (2 bytes) do bytecode para int e empilha. */
void sipush(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções de Carregamento de Variáveis Locais
// ---------------------------------------------------------------------------------------

/** @brief `iload`  — carrega int da variável local de índice lido do bytecode e empilha. */
void iload(Frame *frame);
/** @brief `lload`  — carrega long (2 slots) da variável local de índice lido do bytecode e empilha. */
void lload(Frame *frame);
/** @brief `fload`  — carrega float da variável local de índice lido do bytecode e empilha. */
void fload(Frame *frame);
/** @brief `dload`  — carrega double (2 slots) da variável local de índice lido do bytecode e empilha. */
void dload(Frame *frame);
/** @brief `aload`  — carrega referência da variável local de índice lido do bytecode e empilha. */
void aload(Frame *frame);

/** @brief `iload_0` — carrega int da variável local 0 e empilha. */
void iload_0(Frame *frame);
/** @brief `iload_1` — carrega int da variável local 1 e empilha. */
void iload_1(Frame *frame);
/** @brief `iload_2` — carrega int da variável local 2 e empilha. */
void iload_2(Frame *frame);
/** @brief `iload_3` — carrega int da variável local 3 e empilha. */
void iload_3(Frame *frame);

/** @brief `aload_0` — carrega referência da variável local 0 e empilha. */
void aload_0(Frame *frame);
/** @brief `aload_1` — carrega referência da variável local 1 e empilha. */
void aload_1(Frame *frame);
/** @brief `aload_2` — carrega referência da variável local 2 e empilha. */
void aload_2(Frame *frame);
/** @brief `aload_3` — carrega referência da variável local 3 e empilha. */
void aload_3(Frame *frame);

// ---------------------------------------------------------------------------------------
// Instruções de Armazenamento em Variáveis Locais
// ---------------------------------------------------------------------------------------

/** @brief `istore`  — desempilha int e armazena na variável local de índice lido do bytecode. */
void istore(Frame *frame);
/** @brief `lstore`  — desempilha long (2 slots) e armazena na variável local de índice lido do bytecode. */
void lstore(Frame *frame);
/** @brief `fstore`  — desempilha float e armazena na variável local de índice lido do bytecode. */
void fstore(Frame *frame);
/** @brief `dstore`  — desempilha double (2 slots) e armazena na variável local de índice lido do bytecode. */
void dstore(Frame *frame);
/** @brief `astore`  — desempilha referência e armazena na variável local de índice lido do bytecode. */
void astore(Frame *frame);

/** @brief `istore_0` — desempilha int e armazena na variável local 0. */
void istore_0(Frame *frame);
/** @brief `istore_1` — desempilha int e armazena na variável local 1. */
void istore_1(Frame *frame);
/** @brief `istore_2` — desempilha int e armazena na variável local 2. */
void istore_2(Frame *frame);
/** @brief `istore_3` — desempilha int e armazena na variável local 3. */
void istore_3(Frame *frame);

/** @brief `astore_0` — desempilha referência e armazena na variável local 0. */
void astore_0(Frame *frame);
/** @brief `astore_1` — desempilha referência e armazena na variável local 1. */
void astore_1(Frame *frame);
/** @brief `astore_2` — desempilha referência e armazena na variável local 2. */
void astore_2(Frame *frame);
/** @brief `astore_3` — desempilha referência e armazena na variável local 3. */
void astore_3(Frame *frame);

// ---------------------------------------------------------------------------------------
// Manipulação da Pilha de Operandos
// ---------------------------------------------------------------------------------------

/** @brief `pop`  — descarta o valor do topo da pilha (1 slot). */
void pop_inst(Frame *frame);
/** @brief `dup`  — duplica o valor do topo da pilha (1 slot) e empilha a cópia. */
void dup(Frame *frame);
/** @brief `dup2` — duplica os dois slots do topo da pilha e empilha as cópias. */
void dup2(Frame *frame);
/** @brief `swap` — troca os dois valores do topo da pilha (apenas categoria 1, 1 slot cada). */
void swap(Frame *frame);

// ---------------------------------------------------------------------------------------
// Controle de Fluxo
// ---------------------------------------------------------------------------------------

/** @brief `goto` — desvio incondicional: soma o offset de 2 bytes com sinal ao PC corrente. */
void goto_inst(Frame *frame);

/** @brief `ifeq` — desvia se o int do topo for igual a zero. */
void ifeq(Frame *frame);
/** @brief `ifne` — desvia se o int do topo for diferente de zero. */
void ifne(Frame *frame);
/** @brief `iflt` — desvia se o int do topo for menor que zero. */
void iflt(Frame *frame);
/** @brief `ifge` — desvia se o int do topo for maior ou igual a zero. */
void ifge(Frame *frame);
/** @brief `ifgt` — desvia se o int do topo for maior que zero. */
void ifgt(Frame *frame);
/** @brief `ifle` — desvia se o int do topo for menor ou igual a zero. */
void ifle(Frame *frame);

/** @brief `if_icmpeq` — desvia se os dois ints do topo forem iguais. */
void if_icmpeq(Frame *frame);
/** @brief `if_icmpne` — desvia se os dois ints do topo forem diferentes. */
void if_icmpne(Frame *frame);
/** @brief `if_icmplt` — desvia se o segundo int for menor que o do topo. */
void if_icmplt(Frame *frame);
/** @brief `if_icmpge` — desvia se o segundo int for maior ou igual ao do topo. */
void if_icmpge(Frame *frame);
/** @brief `if_icmpgt` — desvia se o segundo int for maior que o do topo. */
void if_icmpgt(Frame *frame);
/** @brief `if_icmple` — desvia se o segundo int for menor ou igual ao do topo. */
void if_icmple(Frame *frame);

/**
 * @brief `dcmpl` — compara dois doubles e empilha -1, 0 ou 1.
 *
 * Empilha -1 se qualquer operando for NaN (comportamento "less").
 */
void dcmpl(Frame *frame);

/**
 * @brief `dcmpg` — compara dois doubles e empilha -1, 0 ou 1.
 *
 * Empilha +1 se qualquer operando for NaN (comportamento "greater").
 */
void dcmpg(Frame *frame);

/** @brief `ifnull`    — desvia se a referência do topo for null. */
void ifnull(Frame *frame);
/** @brief `ifnonnull` — desvia se a referência do topo não for null. */
void ifnonnull(Frame *frame);

/**
 * @brief `tableswitch` — desvio indexado por tabela densa de offsets.
 *
 * Lê o índice int da pilha e salta para o offset correspondente na tabela.
 * Aplica padding de alinhamento de 4 bytes após o opcode antes de ler os operandos.
 */
void tableswitch(Frame *frame);

/**
 * @brief `lookupswitch` — desvio por tabela esparsa de pares (chave, offset).
 *
 * Busca linearmente a chave int da pilha na tabela e salta para o offset correspondente,
 * ou para o offset default se não encontrada.
 * Aplica padding de alinhamento de 4 bytes após o opcode antes de ler os operandos.
 */
void lookupswitch(Frame *frame);

// ---------------------------------------------------------------------------------------
// Conversão de Tipos
// ---------------------------------------------------------------------------------------

/** @brief `i2f` — converte int para float e empilha o resultado. */
void i2f(Frame *frame);
/** @brief `i2d` — converte int para double (2 slots) e empilha o resultado. */
void i2d(Frame *frame);
/** @brief `f2i` — converte float para int (truncamento) e empilha o resultado. */
void f2i(Frame *frame);
/** @brief `d2i` — converte double para int (truncamento) e empilha o resultado. */
void d2i(Frame *frame);
/** @brief `i2b` — trunca int para byte com extensão de sinal e empilha como int. */
void i2b(Frame *frame);
/** @brief `i2c` — trunca int para char (16 bits sem sinal) e empilha como int. */
void i2c(Frame *frame);
/** @brief `i2s` — trunca int para short com extensão de sinal e empilha como int. */
void i2s(Frame *frame);
/** @brief `d2l` — converte double para long (truncamento) e empilha o resultado (2 slots). */
void d2l(Frame *frame);
/** @brief `d2f` — converte double para float (com perda de precisão) e empilha o resultado. */
void d2f(Frame *frame);

// ---------------------------------------------------------------------------------------
// Campos Estáticos
// ---------------------------------------------------------------------------------------

/**
 * @brief `getstatic` — lê um campo estático da classe indicada no constant pool e empilha o valor.
 *
 * Inclui suporte a mock de `System.out` para viabilizar chamadas a `println`.
 */
void getstatic(Frame *frame);

/** @brief `putstatic` — desempilha um valor e armazena no campo estático indicado no constant pool. */
void putstatic(Frame *frame);

// ---------------------------------------------------------------------------------------
// Invocação de Métodos
// ---------------------------------------------------------------------------------------

/**
 * @brief `invokevirtual` — invoca método de instância com despacho dinâmico pela classe real do objeto.
 *
 * Inclui mock de `System.out.println` para impressão de int, double e String.
 */
void invokevirtual(Frame *frame, JVMStack *stack);

/**
 * @brief `invokespecial` — invoca método de instância sem despacho dinâmico (construtores, métodos privados, super).
 *
 * Inclui mock de `Object.<init>` para ignorar construtores não implementados.
 *
 * @param cf ClassFile da classe corrente, usado na resolução da hierarquia.
 */
void invokespecial(Frame *frame, JVMStack *stack, ClassFile *cf);

/**
 * @brief `invokestatic` — invoca método estático da classe indicada no constant pool.
 *
 * @param cf ClassFile da classe corrente, usado para carregar a classe do método alvo.
 */
void invokestatic(Frame *frame, JVMStack *stack, ClassFile *cf);

/**
 * @brief `invokeinterface` — invoca método de interface com despacho dinâmico pela classe real do objeto.
 */
void invokeinterface(Frame *frame, JVMStack *stack);

#endif
