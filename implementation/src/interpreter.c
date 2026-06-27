#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "instruction.h" 

void execute_engine(JVMStack *stack) {
    // print para debug
    printf("Entrou no execute_engine\n");

    // Laço contínuo de execução
    while (!jvm_stack_is_empty(stack)) {
        
        Frame *current_frame = stack->current_frame;

        // Lógica de busca (Fetch) do byte atual usando o PC
        uint8_t opcode = current_frame->code[current_frame->pc];
        printf("PC=%u OPCODE=0x%02X\n", current_frame->pc - 1, opcode);

        // Atualização do PC para o opcode (1 byte). 
        current_frame->pc++;

        // Motor de Despacho com switch(opcode)
        switch (opcode) {
             // INSTRUÇÕES DE RESOLUÇÃO DE CONSTANTES
            case 0x12: ldc(current_frame); break;    // ldc      (índice ocupa 1 byte)
            case 0x13: ldc_w(current_frame); break;  // ldc_w    (índice ocupa 2 bytes)
            case 0x14: ldc2_w(current_frame); break; // ldc2_w   (long/double, índice ocupa 2 bytes)

            // INSTRUÇÕES MATEMÁTICAS
            case 0x60: iadd(current_frame); break;
            case 0x64: isub(current_frame); break;
            case 0x68: imul(current_frame); break;
            case 0x6C: idiv(current_frame); break;
            case 0x70: irem(current_frame); break;

            case 0x61: ladd(current_frame); break;
            case 0x65: lsub(current_frame); break;
            case 0x69: lmul(current_frame); break;
            case 0x6D: execute_ldiv(current_frame); break; // ldiv
            case 0x71: lrem(current_frame); break;

            case 0x63: dadd(current_frame); break;
            case 0x67: dsub(current_frame); break;
            case 0x6B: dmul(current_frame); break;
            case 0x6F: ddiv(current_frame); break;
            case 0x73: drem(current_frame); break;

            // OPERAÇÕES LÓGICAS E BIT-A-BIT (Inteiros)
            case 0x78: ishl(current_frame); break;
            case 0x7A: ishr(current_frame); break;
            case 0x7C: iushr(current_frame); break;
            case 0x7E: iand(current_frame); break;
            case 0x80: ior(current_frame); break;
            case 0x82: ixor(current_frame); break;

            // INSTRUÇÕES DE ARRAYS
            case 0xBC: newarray(current_frame); break;
            case 0xBD: anewarray(current_frame); break; 
            case 0xBE: arraylength(current_frame); break;
            
            case 0x2E: iaload(current_frame); break; // int
            case 0x2F: laload(current_frame); break; // long
            case 0x30: faload(current_frame); break; // float
            case 0x31: daload(current_frame); break; // double
            case 0x32: aaload(current_frame); break; // object ref
            case 0x33: baload(current_frame); break; // byte/boolean
            case 0x34: caload(current_frame); break; // char
            case 0x35: saload(current_frame); break; // short

            case 0x4F: iastore(current_frame); break; // int
            case 0x50: lastore(current_frame); break; // long
            case 0x51: fastore(current_frame); break; // float
            case 0x52: dastore(current_frame); break; // double
            case 0x53: aastore(current_frame); break; // object ref
            case 0x54: bastore(current_frame); break; // byte/boolean
            case 0x55: castore(current_frame); break; // char
            case 0x56: sastore(current_frame); break; // short

            // INSTRUÇÕES DE OBJETOS
            case 0xBB: new(current_frame); break;  
            case 0xB4: getfield(current_frame); break;  
            case 0xB5: putfield(current_frame); break;
            
            // INSTRUÇÕES DE DESVIO
            case 0xA7: goto_inst(current_frame); break;

            case 0x99: ifeq(current_frame); break;
            case 0x9A: ifne(current_frame); break;
            case 0x9B: iflt(current_frame); break;
            case 0x9C: ifge(current_frame); break;
            case 0x9D: ifgt(current_frame); break;
            case 0x9E: ifle(current_frame); break;

            case 0x9F: if_icmpeq(current_frame); break;
            case 0xA0: if_icmpne(current_frame); break;
            case 0xA1: if_icmplt(current_frame); break;
            case 0xA2: if_icmpge(current_frame); break;
            case 0xA3: if_icmpgt(current_frame); break;
            case 0xA4: if_icmple(current_frame); break;

            // DEFAULT (Instrução não mapeada)
            default:
                fprintf(stderr, "Opcode desconhecido: 0x%02X na posicao %d\n", opcode, current_frame->pc - 1);
                exit(1);
        }
        
        // Sincroniza o PC global da Stack com o PC do frame atual
        pc_register = current_frame->pc;
    }
}