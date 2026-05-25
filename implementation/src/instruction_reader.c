#include <stdio.h>
#include <stdint.h>

#include "../include/instruction_reader.h"

void read_instructions(ClassFile* cf, Code_attribute* code) {
    uint32_t pc = 0;

    while (pc < code->code_length) {
        uint8_t opcode = code->code[pc];

        printf("%04d: ", pc);

        switch(opcode)
        {

            case 0x01: printf("aconst_null\n"); pc += 1; break;
            case 0x02: printf("iconst_m1\n"); pc += 1; break;
            case 0x03: printf("iconst_0\n"); pc += 1; break;
            case 0x04: printf("iconst_1\n"); pc += 1; break;
            case 0x05: printf("iconst_2\n"); pc += 1; break;
            case 0x06: printf("iconst_3\n"); pc += 1; break;
            case 0x07: printf("iconst_4\n"); pc += 1; break;
            case 0x08: printf("iconst_5\n"); pc += 1; break;

            // --- CARGA DE VARIÁVEIS LOCAIS (Fixo) ---
            case 0x1A: printf("iload_0\n"); pc += 1; break;
            case 0x1B: printf("iload_1\n"); pc += 1; break;
            case 0x1C: printf("iload_2\n"); pc += 1; break;
            case 0x1D: printf("iload_3\n"); pc += 1; break;
            case 0x2A: printf("aload_0\n"); pc += 1; break;
            case 0x2B: printf("aload_1\n"); pc += 1; break;
            case 0x2C: printf("aload_2\n"); pc += 1; break;
            case 0x2D: printf("aload_3\n"); pc += 1; break;

            // --- ARMAZENAMENTO DE VARIÁVEIS (Fixo) ---
            case 0x3B: printf("istore_0\n"); pc += 1; break;
            case 0x3C: printf("istore_1\n"); pc += 1; break;
            case 0x3D: printf("istore_2\n"); pc += 1; break;
            case 0x3E: printf("istore_3\n"); pc += 1; break;
            case 0x4B: printf("astore_0\n"); pc += 1; break;
            case 0x4C: printf("astore_1\n"); pc += 1; break;
            case 0x4D: printf("astore_2\n"); pc += 1; break;
            case 0x4E: printf("astore_3\n"); pc += 1; break;

            // --- OPERAÇÕES MATEMÁTICAS BÁSICAS ---
            case 0x60: printf("iadd\n"); pc += 1; break;
            case 0x64: printf("isub\n"); pc += 1; break;
            case 0x68: printf("imul\n"); pc += 1; break;
            case 0x6C: printf("idiv\n"); pc += 1; break;

            // --- RETORNOS ---
            case 0xAC: printf("ireturn\n"); pc += 1; break;
            case 0xB0: printf("areturn\n"); pc += 1; break;
            case 0xB1: printf("return\n"); pc += 1; break;

            // --- CARGA/ARMAZENAMENTO COM OPERANDO ---
            case 0x15: printf("iload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x16: printf("lload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x17: printf("fload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x18: printf("dload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x19: printf("aload %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x36: printf("istore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x37: printf("lstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x38: printf("fstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x39: printf("dstore %u\n", code->code[pc + 1]); pc += 2; break;
            case 0x3A: printf("astore %u\n", code->code[pc + 1]); pc += 2; break;

            // --- MANIPULAÇÃO DIRETA DA PILHA ---
            case 0x57: printf("pop\n"); pc += 1; break;
            case 0x58: printf("pop2\n"); pc += 1; break;
            case 0x59: printf("dup\n"); pc += 1; break;
            case 0x5A: printf("dup_x1\n"); pc += 1; break;
            case 0x5B: printf("dup_x2\n"); pc += 1; break;
            case 0x5C: printf("dup2\n"); pc += 1; break;
            case 0x5D: printf("dup2_x1\n"); pc += 1; break;
            case 0x5E: printf("dup2_x2\n"); pc += 1; break;
            case 0x5F: printf("swap\n"); pc += 1; break;
            case 0xBE: printf("arraylength\n"); pc += 1; break;

            // --- MATEMÁTICA AVANÇADA ---
            case 0x61: printf("ladd\n"); pc += 1; break;
            case 0x62: printf("fadd\n"); pc += 1; break;
            case 0x63: printf("dadd\n"); pc += 1; break;
            case 0x65: printf("lsub\n"); pc += 1; break;
            case 0x66: printf("fsub\n"); pc += 1; break;
            case 0x67: printf("dsub\n"); pc += 1; break;
            case 0x69: printf("lmul\n"); pc += 1; break;
            case 0x6A: printf("fmul\n"); pc += 1; break;
            case 0x6B: printf("dmul\n"); pc += 1; break;
            case 0x6D: printf("ldiv\n"); pc += 1; break;
            case 0x6E: printf("fdiv\n"); pc += 1; break;
            case 0x6F: printf("ddiv\n"); pc += 1; break;
            case 0x70: printf("irem\n"); pc += 1; break;
            case 0x71: printf("lrem\n"); pc += 1; break;
            case 0x72: printf("frem\n"); pc += 1; break;
            case 0x73: printf("drem\n"); pc += 1; break;
            case 0x74: printf("ineg\n"); pc += 1; break;
            case 0x75: printf("lneg\n"); pc += 1; break;
            case 0x76: printf("fneg\n"); pc += 1; break;
            case 0x77: printf("dneg\n"); pc += 1; break;

            // --- OPERAÇÕES LÓGICAS E DE BITS ---
            case 0x78: printf("ishl\n"); pc += 1; break;
            case 0x79: printf("lshl\n"); pc += 1; break;
            case 0x7A: printf("ishr\n"); pc += 1; break;
            case 0x7B: printf("lshr\n"); pc += 1; break;
            case 0x7C: printf("iushr\n"); pc += 1; break;
            case 0x7D: printf("lushr\n"); pc += 1; break;
            case 0x7E: printf("iand\n"); pc += 1; break;
            case 0x7F: printf("land\n"); pc += 1; break;
            case 0x80: printf("ior\n"); pc += 1; break;
            case 0x81: printf("lor\n"); pc += 1; break;
            case 0x82: printf("ixor\n"); pc += 1; break;
            case 0x83: printf("lxor\n"); pc += 1; break;

            // --- CONVERSÕES DE TIPO ---
            case 0x85: printf("i2l\n"); pc += 1; break;
            case 0x86: printf("i2f\n"); pc += 1; break;
            case 0x87: printf("i2d\n"); pc += 1; break;
            case 0x88: printf("l2i\n"); pc += 1; break;
            case 0x89: printf("l2f\n"); pc += 1; break;
            case 0x8A: printf("l2d\n"); pc += 1; break;
            case 0x8B: printf("f2i\n"); pc += 1; break;
            case 0x8C: printf("f2l\n"); pc += 1; break;
            case 0x8D: printf("f2d\n"); pc += 1; break;
            case 0x8E: printf("d2i\n"); pc += 1; break;
            case 0x8F: printf("d2l\n"); pc += 1; break;
            case 0x90: printf("d2f\n"); pc += 1; break;
            case 0x91: printf("i2b\n"); pc += 1; break;
            case 0x92: printf("i2c\n"); pc += 1; break;
            case 0x93: printf("i2s\n"); pc += 1; break;
           
            case 0x12: {
                uint8_t index = code->code[pc + 1];
                printf("ldc #%u\n", index);
                pc += 2;

                break;
            }

            case 0x13: { 
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("ldc_w #%u\n", index);
                pc += 3;

                break;
            }
                
            case 0x14: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("ldc2_w #%u\n", index);
                pc += 3;

                break;
            }

            // FIELD ACCES    
            case 0xB2: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("getstatic #%u\n", index);
                pc += 3;

                break;
            }
                

            case 0xB3: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("putstatic #%u\n", index);
                pc += 3;
                
                break;
            }

            case 0xB4: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("getfield #%u\n", index);
                pc += 3;

                break;
            }    

            case 0xB5: {
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("putfield #%u\n", index);
                pc += 3;

                break;
            }

            // METHOD INVOKES
            case 0xB6: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokevirtual #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB7: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokespecial #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB8: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokestatic #%u\n", index);
                pc += 3;

                break;
            }

            case 0xB9: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                uint8_t count = code->code[pc + 3];
                printf("invokeinterface #%u count %u\n", index, count);
                pc += 5;

                break;
            }

            // OBJECT CREATION
            case 0xBB: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("new #%u\n", index);
                pc += 3;

                break;
            }    

            case 0xBD: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("anewarray #%u\n", index);
                pc += 3;

                break;
            }
            
            case 0xBC: {
                uint8_t atype = code->code[pc + 1];
                printf("newarray %u\n", atype);
                pc += 2;

                break;
            }


            // TYPE CHECKING
            case 0xC0: { 
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("checkcast #%u\n", index);
                pc += 3;

                break;
            }

            case 0xC1: {
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc    + 2];
                printf("instanceof #%u\n", index);
                pc += 3;
                break;
            }

            case 0x10: {
                int8_t value = (int8_t) code->code[pc + 1];
                printf("bipush %d\n", value);
                pc += 2;
                break;
            }

            default:
                printf("unknown opcode 0x%02X\n", opcode);
                pc++;
                break;
        }
    }
}