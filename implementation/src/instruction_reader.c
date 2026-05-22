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
           
            case 0x12:
                uint8_t index = code->code[pc + 1];
                printf("ldc #%u\n", index);
                pc += 2;

                break;

            case 0x13:
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("ldc_w #%u\n", index);
                pc += 3;

                break;
                
            case 0x14:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("ldc2_w #%u\n", index);
                pc += 3;

                break;


            // FIELD ACCES    
            case 0xB2:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("getstatic #%u\n", index);
                pc += 3;

                break;
                

            case 0xB3:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("putstatic #%u\n", index);
                pc += 3;
                
                break;

            case 0xB4:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("getfield #%u\n", index);
                pc += 3;

                break;
                
            case 0xB5:
                uint16_t index =(code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("putfield #%u\n", index);
                pc += 3;

                break;
                

            // METHOD INVOKES
            case 0xB6:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokevirtual #%u\n", index);
                pc += 3;

                break;

            case 0xB7:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokespecial #%u\n", index);
                pc += 3;

                break;

            case 0xB8:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("invokestatic #%u\n", index);
                pc += 3;

                break;

            case 0xB9:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                uint8_t count = code->code[pc + 3];
                printf("invokeinterface #%u count %u\n", index, count);
                pc += 5;

                break;
            

            // OBJECT CREATION
            case 0xBB:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("new #%u\n", index);
                pc += 3;

                break;

            case 0xBD:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("anewarray #%u\n", index);
                pc += 3;

                break;

            case 0xBC:
                uint8_t atype = code->code[pc + 1];
                printf("newarray %u\n", atype);
                pc += 2;

                break;


            // TYPE CHECKING
            case 0xC0:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc + 2];
                printf("checkcast #%u\n", index);
                pc += 3;

                break;

            case 0xC1:
                uint16_t index = (code->code[pc + 1] << 8) | code->code[pc    + 2];
                printf("instanceof #%u\n", index);
                pc += 3;
                break;

            default:
                printf("unknown opcode 0x%02X\n", opcode);
                pc++;
                break;
        }
    }
}