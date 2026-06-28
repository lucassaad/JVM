#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "instruction.h"
#include <string.h>
#include "class_file.h"
#include "constant_pool.h"
#include "attributes.h"

static method_info *find_method_by_name(ClassFile *cf,
                                         const char *name,
                                         const char *desc) {
    for (int i = 0; i < cf->methods_count; i++) {
        method_info *m = &cf->methods[i];

        CONSTANT_Utf8_info *mname =
            (CONSTANT_Utf8_info *)cf->constant_pool[m->name_index]->info;
        CONSTANT_Utf8_info *mdesc =
            (CONSTANT_Utf8_info *)cf->constant_pool[m->descriptor_index]->info;

        if ((int)mname->length != (int)strlen(name)) continue;
        if (strncmp((char *)mname->bytes, name, mname->length) != 0) continue;
        if ((int)mdesc->length != (int)strlen(desc)) continue;
        if (strncmp((char *)mdesc->bytes, desc, mdesc->length) != 0) continue;

        return m;
    }
    return NULL;
}

static Code_attribute *get_code_attribute(ClassFile *cf, method_info *method) {
    for (int i = 0; i < method->attributes_count; i++) {
        attribute_info *attr = &method->attributes[i];
        CONSTANT_Utf8_info *utf8 =
            (CONSTANT_Utf8_info *)cf->constant_pool[attr->attribute_name_index]->info;

        if (utf8->length == 4 && strncmp((char *)utf8->bytes, "Code", 4) == 0)
            return (Code_attribute *)attr->info;
    }
    return NULL;
}

void execute_engine(JVMStack *stack, ClassFile *cf)  {
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
            // INSTRUÇÕES ENVOLVENDO CONSTANTES
            case 0x01: aconst_null(current_frame); break;
            case 0x02: iconst_m1(current_frame); break;
            case 0x03: iconst_0(current_frame); break;
            case 0x04: iconst_1(current_frame); break;
            case 0x05: iconst_2(current_frame); break;
            case 0x06: iconst_3(current_frame); break;
            case 0x07: iconst_4(current_frame); break;
            case 0x08: iconst_5(current_frame); break;
            case 0x10: bipush(current_frame); break;
            case 0x11: sipush(current_frame); break;

             // INSTRUÇÕES DE RESOLUÇÃO DE CONSTANTES
            case 0x12: ldc(current_frame); break;    // ldc      (índice ocupa 1 byte)
            case 0x13: ldc_w(current_frame); break;  // ldc_w    (índice ocupa 2 bytes)
            case 0x14: ldc2_w(current_frame); break; // ldc2_w   (long/double, índice ocupa 2 bytes)

            // INSTRUÇÕES DE CARREGAMENTO
            case 0x15: iload(current_frame); break;
            case 0x16: lload(current_frame); break;
            case 0x17: fload(current_frame); break;
            case 0x18: dload(current_frame); break;
            case 0x19: aload(current_frame); break;

            case 0x1A: iload_0(current_frame); break;
            case 0x1B: iload_1(current_frame); break;
            case 0x1C: iload_2(current_frame); break;
            case 0x1D: iload_3(current_frame); break;

            case 0x2A: aload_0(current_frame); break;
            case 0x2B: aload_1(current_frame); break;
            case 0x2C: aload_2(current_frame); break;
            case 0x2D: aload_3(current_frame); break;

            // INSTRUÇÕES DE ARMAZENAMENTO
            case 0x36: istore(current_frame); break;
            case 0x37: lstore(current_frame); break;
            case 0x38: fstore(current_frame); break;
            case 0x39: dstore(current_frame); break;
            case 0x3A: astore(current_frame); break;

            case 0x3B: istore_0(current_frame); break;
            case 0x3C: istore_1(current_frame); break;
            case 0x3D: istore_2(current_frame); break;
            case 0x3E: istore_3(current_frame); break;

            case 0x4B: astore_0(current_frame); break;
            case 0x4C: astore_1(current_frame); break;
            case 0x4D: astore_2(current_frame); break;
            case 0x4E: astore_3(current_frame); break;

            // INSTRUÇÕES DE MANIPULAÇÃO DA PILHA
            case 0x57: pop_inst(current_frame); break; 
            case 0x59: dup(current_frame); break;
            case 0x5F: swap(current_frame); break;

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
            
            // ── INVOKESTATIC
            case 0xB8: {
                // Lê o índice de 2 bytes que vem depois do opcode no bytecode
                uint16_t cp_idx = ((uint16_t)current_frame->code[current_frame->pc] << 8)
                                |  (uint16_t)current_frame->code[current_frame->pc + 1];
                current_frame->pc += 2;

                // Vai na Constant Pool e pega o Methodref
                cp_info *cp_entry = current_frame->constant_pool[cp_idx];
                CONSTANT_Methodref_info *mref = (CONSTANT_Methodref_info *)cp_entry->info;

                // Do Methodref pega o NameAndType, que tem o nome e o descriptor do método
                CONSTANT_NameAndType_info *nt =
                    (CONSTANT_NameAndType_info *)
                    current_frame->constant_pool[mref->name_and_type_index]->info;

                CONSTANT_Utf8_info *name_utf8 =
                    (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->name_index]->info;
                CONSTANT_Utf8_info *desc_utf8 =
                    (CONSTANT_Utf8_info *)current_frame->constant_pool[nt->descriptor_index]->info;

                // Copia para strings com '\0' no final (C precisa disso para comparar)
                char method_name[name_utf8->length + 1];
                memcpy(method_name, name_utf8->bytes, name_utf8->length);
                method_name[name_utf8->length] = '\0';

                char method_desc[desc_utf8->length + 1];
                memcpy(method_desc, desc_utf8->bytes, desc_utf8->length);
                method_desc[desc_utf8->length] = '\0';

                // Localiza o método na ClassFile pelo nome + descriptor
                method_info *target = find_method_by_name(cf, method_name, method_desc);
                if (target == NULL) {
                    fprintf(stderr, "[invokestatic] metodo '%s%s' nao encontrado\n",
                            method_name, method_desc);
                    exit(1);
                }

                // Pega o bytecode (Code attribute) desse método
                Code_attribute *target_code = get_code_attribute(cf, target);
                if (target_code == NULL) {
                    fprintf(stderr, "[invokestatic] '%s' nao tem Code attribute\n", method_name);
                    exit(1);
                }

                // Conta quantos slots de argumento o método espera receber
                // (0 = método estático, sem 'this')
                uint16_t num_arg_slots =
                    descriptor_count_arg_slots(method_desc, /*is_instance_method=*/0);

                // Cria o novo Frame e empilha na JVMStack.
                // frame_push_method já faz tudo:
                //   - aloca o Frame com max_locals/max_stack do Code attribute
                //   - tira os argumentos da pilha do frame atual
                //   - coloca eles nas variáveis locais do novo frame (local[0], local[1], ...)
                //   - faz stack->current_frame apontar para o novo frame
                JVMStackStatus status = frame_push_method(
                    stack,
                    target_code,
                    current_frame->constant_pool,
                    current_frame->constant_pool_count,
                    num_arg_slots
                );

                if (status != JVM_STACK_OK) {
                    fprintf(stderr, "[invokestatic] frame_push_method falhou (status=%d)\n", status);
                    exit(1);
                }

                // Na próxima iteração do while, current_frame vai ser o novo frame
                // e a execução começa do PC=0 do método chamado.
                break;
            }

            // ── RETORNOS ─────────────────────────────────────────────────────────────────
            // frame_pop_method faz tudo:
            //   - lê o valor de retorno da pilha do frame atual (se houver)
            //   - destrói o frame atual
            //   - restaura o frame chamador como current_frame
            //   - empurra o valor de retorno na pilha do frame chamador

            case 0xB1: frame_pop_method(stack, RETURN_VOID);   break; // return  (void)
            case 0xAC: frame_pop_method(stack, RETURN_INT);    break; // ireturn (int)
            case 0xAE: frame_pop_method(stack, RETURN_FLOAT);  break; // freturn (float)
            case 0xAD: frame_pop_method(stack, RETURN_LONG);   break; // lreturn (long)
            case 0xAF: frame_pop_method(stack, RETURN_DOUBLE); break; // dreturn (double)
            case 0xB0: frame_pop_method(stack, RETURN_REF);    break; // areturn (objeto/array)
            
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

            //  INSTRUÇÕES DE CONVERSÃO DE TIPOS
            case 0x86: i2f(current_frame); break;
            case 0x87: i2d(current_frame); break;
            case 0x8B: f2i(current_frame); break;
            case 0x8E: d2i(current_frame); break;
            case 0x91: i2b(current_frame); break;
            case 0x92: i2c(current_frame); break;
            case 0x93: i2s(current_frame); break;

            // INSTRUÇÕES RELATIVAS A ATRIBUTOS ESTÁTICOS
            case 0xB2: getstatic(current_frame); break;
            case 0xB3: putstatic(current_frame); break;

            // INSTRUÇÃO DE INVOCAÇÃO DE MÉTODOS
            case 0xB6: invokevirtual(current_frame); break;

            // DEFAULT (Instrução não mapeada)
            default:
                fprintf(stderr, "Opcode desconhecido: 0x%02X na posicao %d\n", opcode, current_frame->pc - 1);
                exit(1);
        }
        
        // Sincroniza o PC global da Stack com o PC do frame atual
        pc_register = current_frame->pc;
    }
}
