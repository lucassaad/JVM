/**
 * @file instruction_viewer.c
 * @brief Implementação do disassembler de bytecode para o modo Leitor/Exibidor.
 *
 * Implementa view_instructions(), que percorre o array de bytes do atributo Code
 * de um método e imprime cada instrução no formato:
 *   <offset>: <mnemônico> [operandos]
 *
 * Detalhes de implementação:
 * - Instruções de largura fixa (1 byte): apenas imprime o mnemônico e avança pc += 1.
 * - Instruções com operando de 1 byte (e.g. iload, bipush): lê code[pc+1] e avança pc += 2.
 * - Instruções com operando de 2 bytes (e.g. getstatic, invokevirtual): reconstrói o
 *   índice de 16 bits com (code[pc+1] << 8) | code[pc+2] e avança pc += 3.
 * - tableswitch (0xAA): aplica padding até alinhamento de 4 bytes, depois lê
 *   default, low, high (4 bytes cada) e (high-low+1) offsets de 4 bytes.
 * - lookupswitch (0xAB): aplica padding até alinhamento de 4 bytes, depois lê
 *   default, npairs (4 bytes cada) e npairs pares (match, offset) de 4 bytes cada.
 * - invokeinterface (0xB9): lê índice de 2 bytes + count de 1 byte + 1 byte reservado
 *   (avança pc += 5 no total).
 * - multianewarray (0xC5): lê índice de 2 bytes + número de dimensões de 1 byte
 *   (avança pc += 4 no total).
 * - Opcodes desconhecidos: imprime "unknown opcode 0xXX" e avança pc += 1 para
 *   não travar o laço.
 * - O parâmetro cf é recebido mas não utilizado nesta implementação (marcado com
 *   (void)cf para suprimir warning do compilador).
 */

#include <stdio.h>
#include <stdint.h>

#include "instruction_viewer.h"

void view_instructions(FILE *out, ClassFile* cf, Code_attribute* code) {
    (void)cf;
    uint32_t pc = 0;

    while (pc < code->code_length) {
        uint8_t opcode = code->code[pc];

        fprintf(out, "%04d: ", pc);

        switch(opcode)
        {

            case 0x01: fprintf(out, "aconst_null\n"); pc += 1; break;
            case 0x02: fprintf(out, "iconst_m1\n"); pc += 1; break;
            case 0x03: fprintf(out, "iconst_0\n"); pc += 1; break;
            case 0x04: fprintf(out, "iconst_1\n"); pc += 1; break;
            case 0x05: fprintf(out, "iconst_2\n"); pc += 1; break;
            case 0x06: fprintf(out, "iconst_3\n"); pc += 1; break;
            case 0x07: fprintf(out, "iconst_4\n"); pc += 1; break;
            case 0x08: fprintf(out, "iconst_5\n"); pc += 1; break;

            // CARGA DE VARIÁVEIS LOCAIS (Fixo)
            case 0x1A: fprintf(out, "iload_0\n"); pc += 1; break;
            case 0x1B: fprintf(out, "iload_1\n"); pc += 1; break;
            case 0x1C: fprintf(out, "iload_2\n"); pc += 1; break;
            case 0x1D: fprintf(out, "iload_3\n"); pc += 1; break;
            case 0x2A: fprintf(out, "aload_0\n"); pc += 1; break;
            case 0x2B: fprintf(out, "aload_1\n"); pc += 1; break;
            case 0x2C: fprintf(out, "aload_2\n"); pc += 1; break;
            case 0x2D: fprintf(out, "aload_3\n"); pc += 1; break;

            // ARMAZENAMENTO DE VARIÁVEIS (Fixo) 
            case 0x3B: fprintf(out, "istore_0\n"); pc += 1; break;
            case 0x3C: fprintf(out, "istore_1\n"); pc += 1; break;
            case 0x3D: fprintf(out, "istore_2\n"); pc += 1; break;
            case 0x3E: fprintf(out, "istore_3\n"); pc += 1; break;
            case 0x4B: fprintf(out, "astore_0\n"); pc += 1; break;
            case 0x4C: fprintf(out, "astore_1\n"); pc += 1; break;
            case 0x4D: fprintf(out, "astore_2\n"); pc += 1; break;
            case 0x4E: fprintf(out, "astore_3\n"); pc += 1; break;

            // OPERAÇÕES MATEMÁTICAS BÁSICAS 
            case 0x60: fprintf(out, "iadd\n"); pc += 1; break;
            case 0x64: fprintf(out, "isub\n"); pc += 1; break;
            case 0x68: fprintf(out, "imul\n"); pc += 1; break;
            case 0x6C: fprintf(out, "idiv\n"); pc += 1; break;

            // RETORNOS
            case 0xAC: fprintf(out, "ireturn\n"); pc += 1; break;
            case 0xB0: fprintf(out, "areturn\n"); pc += 1; break;
            case 0xB1: fprintf(out, "return\n"); pc += 1; break;

            // CARGA/ARMAZENAMENTO COM OPERANDO
            case 0x15: fprintf(out, "iload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x16: fprintf(out, "lload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x17: fprintf(out, "fload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x18: fprintf(out, "dload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x19: fprintf(out, "aload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x36: fprintf(out, "istore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x37: fprintf(out, "lstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x38: fprintf(out, "fstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x39: fprintf(out, "dstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x3A: fprintf(out, "astore %u\n", code->code[pc + 1]); pc += 2; break;

            // MANIPULAÇÃO DIRETA DA PILHA 
            case 0x57: fprintf(out, "pop\n"); pc += 1; break;
            case 0x58: fprintf(out, "pop2\n"); pc += 1; break;
            case 0x59: fprintf(out, "dup\n"); pc += 1; break;
            case 0x5A: fprintf(out, "dup_x1\n"); pc += 1; break;
            case 0x5B: fprintf(out, "dup_x2\n"); pc += 1; break;
            case 0x5C: fprintf(out, "dup2\n"); pc += 1; break;
            case 0x5D: fprintf(out, "dup2_x1\n"); pc += 1; break;
            case 0x5E: fprintf(out, "dup2_x2\n"); pc += 1; break;
            case 0x5F: fprintf(out, "swap\n"); pc += 1; break;
            case 0xBE: fprintf(out, "arraylength\n"); pc += 1; break;

            // MATEMÁTICA AVANÇADA
            case 0x61: fprintf(out, "ladd\n"); pc += 1; break;
            case 0x62: fprintf(out, "fadd\n"); pc += 1; break;
            case 0x63: fprintf(out, "dadd\n"); pc += 1; break;
            case 0x65: fprintf(out, "lsub\n"); pc += 1; break;
            case 0x66: fprintf(out, "fsub\n"); pc += 1; break;
            case 0x67: fprintf(out, "dsub\n"); pc += 1; break;
            case 0x69: fprintf(out, "lmul\n"); pc += 1; break;
            case 0x6A: fprintf(out, "fmul\n"); pc += 1; break;
            case 0x6B: fprintf(out, "dmul\n"); pc += 1; break;
            case 0x6D: fprintf(out, "ldiv\n"); pc += 1; break;
            case 0x6E: fprintf(out, "fdiv\n"); pc += 1; break;
            case 0x6F: fprintf(out, "ddiv\n"); pc += 1; break;
            case 0x70: fprintf(out, "irem\n"); pc += 1; break;
            case 0x71: fprintf(out, "lrem\n"); pc += 1; break;
            case 0x72: fprintf(out, "frem\n"); pc += 1; break;
            case 0x73: fprintf(out, "drem\n"); pc += 1; break;
            case 0x74: fprintf(out, "ineg\n"); pc += 1; break;
            case 0x75: fprintf(out, "lneg\n"); pc += 1; break;
            case 0x76: fprintf(out, "fneg\n"); pc += 1; break;
            case 0x77: fprintf(out, "dneg\n"); pc += 1; break;

            // OPERAÇÕES LÓGICAS E DE BITS
            case 0x78: fprintf(out, "ishl\n"); pc += 1; break;
            case 0x79: fprintf(out, "lshl\n"); pc += 1; break;
            case 0x7A: fprintf(out, "ishr\n"); pc += 1; break;
            case 0x7B: fprintf(out, "lshr\n"); pc += 1; break;
            case 0x7C: fprintf(out, "iushr\n"); pc += 1; break;
            case 0x7D: fprintf(out, "lushr\n"); pc += 1; break;
            case 0x7E: fprintf(out, "iand\n"); pc += 1; break;
            case 0x7F: fprintf(out, "land\n"); pc += 1; break;
            case 0x80: fprintf(out, "ior\n"); pc += 1; break;
            case 0x81: fprintf(out, "lor\n"); pc += 1; break;
            case 0x82: fprintf(out, "ixor\n"); pc += 1; break;
            case 0x83: fprintf(out, "lxor\n"); pc += 1; break;

            // CONVERSÕES DE TIPO
            case 0x85: fprintf(out, "i2l\n"); pc += 1; break;
            case 0x86: fprintf(out, "i2f\n"); pc += 1; break;
            case 0x87: fprintf(out, "i2d\n"); pc += 1; break;
            case 0x88: fprintf(out, "l2i\n"); pc += 1; break;
            case 0x89: fprintf(out, "l2f\n"); pc += 1; break;
            case 0x8A: fprintf(out, "l2d\n"); pc += 1; break;
            case 0x8B: fprintf(out, "f2i\n"); pc += 1; break;
            case 0x8C: fprintf(out, "f2l\n"); pc += 1; break;
            case 0x8D: fprintf(out, "f2d\n"); pc += 1; break;
            case 0x8E: fprintf(out, "d2i\n"); pc += 1; break;
            case 0x8F: fprintf(out, "d2l\n"); pc += 1; break;
            case 0x90: fprintf(out, "d2f\n"); pc += 1; break;
            case 0x91: fprintf(out, "i2b\n"); pc += 1; break;
            case 0x92: fprintf(out, "i2c\n"); pc += 1; break;
            case 0x93: fprintf(out, "i2s\n"); pc += 1; break;
           
            case 0x12: {
                uint8_t index = code->code[pc + 1];
                fprintf(out, "ldc #%u\n", index);
                pc += 2;

                break;
            }

            case 0x13: { 
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "ldc_w #%u\n", index);
                pc += 3;

                break;
            }
                
            case 0x14: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "ldc2_w #%u\n", index);
                pc += 3;

                break;
            }

            // FIELD ACCES    
            case 0xB2: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "getstatic #%u\n", index);
                pc += 3;

                break;
            }
                

            case 0xB3: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "putstatic #%u\n", index);
                pc += 3;
                
                break;
            }

            case 0xB4: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "getfield #%u\n", index);
                pc += 3;

                break;
            }    

            case 0xB5: {
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "putfield #%u\n", index);
                pc += 3;

                break;
            }

            // METHOD INVOKES
            case 0xB6: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "invokevirtual #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB7: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "invokespecial #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB8: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "invokestatic #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB9: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                uint8_t count = code->code[pc + 3];
                fprintf(out, "invokeinterface #%u count %u\n", index, count);
                pc += 5;

                break;
            }

            // OBJECT CREATION
            case 0xBB: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "new #%u\n", index);
                pc += 3;

                break;
            }    

            case 0xBD: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "anewarray #%u\n", index);
                pc += 3;

                break;
            }
            
            case 0xBC: {
                uint8_t atype = code->code[pc + 1];
                fprintf(out, "newarray %u\n", atype);
                pc += 2;

                break;
            }


            // TYPE CHECKING
            case 0xC0: { 
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                fprintf(out, "checkcast #%u\n", index);
                pc += 3;

                break;
            }

            case 0xC1: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc    + 2];
                fprintf(out, "instanceof #%u\n", index);
                pc += 3;
                break;
            }

            case 0x10: {
                int8_t value = (int8_t) code->code[pc + 1];
                fprintf(out, "bipush %d\n", value);
                pc += 2;
                break;
            }

                        // ARRAY LOAD
            case 0x2E: fprintf(out, "iaload\n"); pc += 1; break;
            case 0x2F: fprintf(out, "laload\n"); pc += 1; break;
            case 0x30: fprintf(out, "faload\n"); pc += 1; break;
            case 0x31: fprintf(out, "daload\n"); pc += 1; break;
            case 0x32: fprintf(out, "aaload\n"); pc += 1; break;
            case 0x33: fprintf(out, "baload\n"); pc += 1; break;
            case 0x34: fprintf(out, "caload\n"); pc += 1; break;
            case 0x35: fprintf(out, "saload\n"); pc += 1; break;

            // ARRAY STORE
            case 0x4F: fprintf(out, "iastore\n"); pc += 1; break;
            case 0x50: fprintf(out, "lastore\n"); pc += 1; break;
            case 0x51: fprintf(out, "fastore\n"); pc += 1; break;
            case 0x52: fprintf(out, "dastore\n"); pc += 1; break;
            case 0x53: fprintf(out, "aastore\n"); pc += 1; break;
            case 0x54: fprintf(out, "bastore\n"); pc += 1; break;
            case 0x55: fprintf(out, "castore\n"); pc += 1; break;
            case 0x56: fprintf(out, "sastore\n"); pc += 1; break;

            // PUSH
            case 0x11: {
                int16_t value = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "sipush %d\n", value);
                pc += 3;
                break;
            }

            // FLOAT CONSTANTS
            case 0x0B: fprintf(out, "fconst_0\n"); pc += 1; break;
            case 0x0C: fprintf(out, "fconst_1\n"); pc += 1; break;
            case 0x0D: fprintf(out, "fconst_2\n"); pc += 1; break;

            // LONG CONSTANTS
            case 0x09: fprintf(out, "lconst_0\n"); pc += 1; break;
            case 0x0A: fprintf(out, "lconst_1\n"); pc += 1; break;

            // DOUBLE CONSTANTS
            case 0x0E: fprintf(out, "dconst_0\n"); pc += 1; break;
            case 0x0F: fprintf(out, "dconst_1\n"); pc += 1; break;

            // CONTROLE DE FLUXO
            case 0x84: {
                uint8_t index = code->code[pc + 1];
                int8_t  cont  = (int8_t) code->code[pc + 2];
                fprintf(out, "iinc %u %d\n", index, cont);
                pc += 3;
                break;
            }

            case 0x99: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifeq %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9A: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifne %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9B: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "iflt %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9C: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifge %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9D: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifgt %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9E: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifle %d\n", offset);
                pc += 3;
                break;
            }
            case 0x9F: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmpeq %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA0: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmpne %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA1: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmplt %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA2: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmpge %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA3: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmpgt %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA4: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_icmple %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA5: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_acmpeq %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA6: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "if_acmpne %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA7: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "goto %d\n", offset);
                pc += 3;
                break;
            }
            case 0xC6: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifnull %d\n", offset);
                pc += 3;
                break;
            }
            case 0xC7: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "ifnonnull %d\n", offset);
                pc += 3;
                break;
            }

            // LONG COMPARE
            case 0x94: fprintf(out, "lcmp\n"); pc += 1; break;
            case 0x95: fprintf(out, "fcmpl\n"); pc += 1; break;
            case 0x96: fprintf(out, "fcmpg\n"); pc += 1; break;
            case 0x97: fprintf(out, "dcmpl\n"); pc += 1; break;
            case 0x98: fprintf(out, "dcmpg\n"); pc += 1; break;

            // MISC
            case 0xC2: fprintf(out, "monitorenter\n"); pc += 1; break;
            case 0xC3: fprintf(out, "monitorexit\n"); pc += 1; break;
            case 0xBF: fprintf(out, "athrow\n"); pc += 1; break;
            case 0xA8: {
                int16_t offset = (int16_t)((code->code[pc + 1] << 8) | code->code[pc + 2]);
                fprintf(out, "jsr %d\n", offset);
                pc += 3;
                break;
            }
            case 0xA9: {
                fprintf(out, "ret %u\n", code->code[pc + 1]);
                pc += 2;
                break;
            }

            // DOUBLE/FLOAT STORE (Fixo)
            case 0x43: fprintf(out, "fstore_0\n"); pc += 1; break;
            case 0x44: fprintf(out, "fstore_1\n"); pc += 1; break;
            case 0x45: fprintf(out, "fstore_2\n"); pc += 1; break;
            case 0x46: fprintf(out, "fstore_3\n"); pc += 1; break;
            case 0x47: fprintf(out, "dstore_0\n"); pc += 1; break;
            case 0x48: fprintf(out, "dstore_1\n"); pc += 1; break;
            case 0x49: fprintf(out, "dstore_2\n"); pc += 1; break;
            case 0x4A: fprintf(out, "dstore_3\n"); pc += 1; break;

            // DOUBLE/FLOAT LOAD (Fixo)
            case 0x22: fprintf(out, "fload_0\n"); pc += 1; break;
            case 0x23: fprintf(out, "fload_1\n"); pc += 1; break;
            case 0x24: fprintf(out, "fload_2\n"); pc += 1; break;
            case 0x25: fprintf(out, "fload_3\n"); pc += 1; break;
            case 0x26: fprintf(out, "dload_0\n"); pc += 1; break;
            case 0x27: fprintf(out, "dload_1\n"); pc += 1; break;
            case 0x28: fprintf(out, "dload_2\n"); pc += 1; break;
            case 0x29: fprintf(out, "dload_3\n"); pc += 1; break;
            case 0x1E: fprintf(out, "lload_0\n"); pc += 1; break;
            case 0x1F: fprintf(out, "lload_1\n"); pc += 1; break;
            case 0x20: fprintf(out, "lload_2\n"); pc += 1; break;
            case 0x21: fprintf(out, "lload_3\n"); pc += 1; break;

            // RETORNOS
            case 0xAF: fprintf(out, "dreturn\n"); pc += 1; break;
            case 0xAD: fprintf(out, "lreturn\n"); pc += 1; break;
            case 0xAE: fprintf(out, "freturn\n"); pc += 1; break;

            // MULTIANEWARRAY
            case 0xC5: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                uint8_t dimensions = code->code[pc + 3];
                fprintf(out, "multianewarray #%u %u\n", index, dimensions);
                pc += 4;
                break;
            }

            // TABLESWITCH
            case 0xAA: {
                uint32_t base_pc = pc;
                pc++;
                // alinha para múltiplo de 4
                while (pc % 4 != 0) pc++;
                int32_t def = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                int32_t low = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                int32_t high = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                fprintf(out, "tableswitch [%d-%d] default:%d\n", low, high, def);
                int32_t count = high - low + 1;
                for (int32_t k = 0; k < count; k++) {
                    int32_t offset = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                    fprintf(out, "          case %d: %d\n", low + k, offset);
                }
                (void)base_pc;
                break;
            }

            // LOOKUPSWITCH
            case 0xAB: {
                uint32_t base_pc = pc;
                pc++;
                while (pc % 4 != 0) pc++;
                int32_t def = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                int32_t npairs = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                fprintf(out, "lookupswitch npairs:%d default:%d\n", npairs, def);
                for (int32_t k = 0; k < npairs; k++) {
                    int32_t match = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                    int32_t offset = (int32_t)((code->code[pc] << 24) | (code->code[pc+1] << 16) | (code->code[pc+2] << 8) | code->code[pc+3]); pc += 4;
                    fprintf(out, "          case %d: %d\n", match, offset);
                }
                (void)base_pc;
                break;
            }

            default:
                fprintf(out, "unknown opcode 0x%02X\n", opcode);
                pc++;
                break;
        }
    }
}