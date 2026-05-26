#include <stdio.h>
#include <stdint.h>

#include "../include/instruction_reader.h"

void read_instructions(FILE *out, ClassFile* cf, Code_attribute* code) {
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

            // --- CARGA DE VARIÁVEIS LOCAIS (Fixo) ---
            case 0x1A: fprintf(out, "iload_0\n"); pc += 1; break;
            case 0x1B: fprintf(out, "iload_1\n"); pc += 1; break;
            case 0x1C: fprintf(out, "iload_2\n"); pc += 1; break;
            case 0x1D: fprintf(out, "iload_3\n"); pc += 1; break;
            case 0x2A: fprintf(out, "aload_0\n"); pc += 1; break;
            case 0x2B: fprintf(out, "aload_1\n"); pc += 1; break;
            case 0x2C: fprintf(out, "aload_2\n"); pc += 1; break;
            case 0x2D: fprintf(out, "aload_3\n"); pc += 1; break;

            // --- ARMAZENAMENTO DE VARIÁVEIS (Fixo) ---
            case 0x3B: fprintf(out, "istore_0\n"); pc += 1; break;
            case 0x3C: fprintf(out, "istore_1\n"); pc += 1; break;
            case 0x3D: fprintf(out, "istore_2\n"); pc += 1; break;
            case 0x3E: fprintf(out, "istore_3\n"); pc += 1; break;
            case 0x4B: fprintf(out, "astore_0\n"); pc += 1; break;
            case 0x4C: fprintf(out, "astore_1\n"); pc += 1; break;
            case 0x4D: fprintf(out, "astore_2\n"); pc += 1; break;
            case 0x4E: fprintf(out, "astore_3\n"); pc += 1; break;

            // --- OPERAÇÕES MATEMÁTICAS BÁSICAS ---
            case 0x60: fprintf(out, "iadd\n"); pc += 1; break;
            case 0x64: fprintf(out, "isub\n"); pc += 1; break;
            case 0x68: fprintf(out, "imul\n"); pc += 1; break;
            case 0x6C: fprintf(out, "idiv\n"); pc += 1; break;

            // --- RETORNOS ---
            case 0xAC: fprintf(out, "ireturn\n"); pc += 1; break;
            case 0xB0: fprintf(out, "areturn\n"); pc += 1; break;
            case 0xB1: fprintf(out, "return\n"); pc += 1; break;

            // --- CARGA/ARMAZENAMENTO COM OPERANDO ---
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

            // --- MANIPULAÇÃO DIRETA DA PILHA ---
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

            // --- MATEMÁTICA AVANÇADA ---
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

            // --- OPERAÇÕES LÓGICAS E DE BITS ---
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

            // --- CONVERSÕES DE TIPO ---
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

            default:
                fprintf(out, "unknown opcode 0x%02X\n", opcode);
                pc++;
                break;
        }
    }
}