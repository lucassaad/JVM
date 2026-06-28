/**
 * @file instruction_viewer.h
 * @brief Disassembler de bytecode para o modo Leitor/Exibidor.
 *
 * Declara a função responsável por percorrer o array de bytes de um atributo
 * Code e imprimir cada instrução em formato legível (offset + mnemônico + operandos),
 * de maneira análoga à saída do comando `javap -c`.
 *
 * Utilizada por print_specific_attribute_info() (viewer.h) ao exibir o conteúdo
 * do atributo Code de um método no modo de operação `-l`.
 */

#ifndef INSTRUCTION_VIEWER_H
#define INSTRUCTION_VIEWER_H

#include "class_file.h"
#include "attributes.h"

/**
 * @brief Percorre e desassembla o bytecode de um atributo Code.
 *
 * Itera pelo array @p code->code byte a byte, decodificando cada opcode e
 * imprimindo em @p out o offset (posição dentro do bytecode), o mnemônico
 * da instrução (e.g. `aload_0`, `invokevirtual`) e os operandos inline
 * decodificados em formato legível.
 *
 * Instruções de largura variável são tratadas corretamente:
 * - `tableswitch` e `lookupswitch`: aplica padding de alinhamento de 4 bytes
 *   e lê os pares (chave, offset) da tabela.
 * - `wide`: lê o opcode modificado e o índice de 2 bytes subsequente.
 * - `multianewarray`: lê índice de 2 bytes e o número de dimensões.
 *
 * @param out  Ponteiro para o arquivo de saída (pode ser stdout ou arquivo de texto).
 * @param cf   Ponteiro para o ClassFile da classe, usado para resolver referências
 *             do constant pool exibidas como operandos (e.g. nome de método em invokevirtual).
 * @param code Ponteiro para o Code_attribute cujo bytecode será desassemblado.
 */
void view_instructions(
    FILE *out,
    ClassFile* cf,
    Code_attribute* code
);

#endif
