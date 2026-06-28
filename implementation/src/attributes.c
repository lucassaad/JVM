/**
 * @file attributes.c
 * @brief Implementação da leitura de atributos específicos do formato `.class`.
 *
 * Contém a implementação de read_specific_attribute_info(), que atua como
 * dispatcher: com base no nome do atributo, aloca a struct correta, lê os
 * bytes do arquivo convertendo de big-endian para little-endian via byteswap,
 * e retorna o ponteiro genérico para o payload preenchido.
 *
 * Atributos suportados: Code, ConstantValue, Exceptions, InnerClasses,
 * SourceFile, LineNumberTable e LocalVariableTable. Atributos desconhecidos
 * são lidos como buffer bruto de bytes para preservar o avanço correto do
 * ponteiro de leitura no arquivo.
 */

#include <stdlib.h>
#include <string.h>
#include "attributes.h"
#include "utils.h"
#include "reader.h"

void* read_specific_attribute_info(ClassFile *cf, FILE *file, const char* attr_name, uint32_t attr_length) {
    if (strcmp(attr_name, "Code") == 0) {
        Code_attribute *code = malloc(sizeof(Code_attribute));
        
        fread(&code->max_stack, sizeof(uint16_t), 1, file);
        code->max_stack = byteswap_u2(code->max_stack);

        fread(&code->max_locals, sizeof(uint16_t), 1, file);
        code->max_locals = byteswap_u2(code->max_locals);

        fread(&code->code_length, sizeof(uint32_t), 1, file);
        code->code_length = byteswap_u4(code->code_length);

        code->code = malloc(code->code_length);
        fread(code->code, 1, code->code_length, file);

        fread(&code->exception_table_length, sizeof(uint16_t), 1, file);
        code->exception_table_length = byteswap_u2(code->exception_table_length);

        if (code->exception_table_length > 0) {
            code->exception_table = malloc(sizeof(ExceptionTable_info) * code->exception_table_length);
            for (int i = 0; i < code->exception_table_length; i++) {
                fread(&code->exception_table[i].start_pc, sizeof(uint16_t), 1, file);
                code->exception_table[i].start_pc = byteswap_u2(code->exception_table[i].start_pc);
                
                fread(&code->exception_table[i].end_pc, sizeof(uint16_t), 1, file);
                code->exception_table[i].end_pc = byteswap_u2(code->exception_table[i].end_pc);
                
                fread(&code->exception_table[i].handler_pc, sizeof(uint16_t), 1, file);
                code->exception_table[i].handler_pc = byteswap_u2(code->exception_table[i].handler_pc);
                
                fread(&code->exception_table[i].catch_type, sizeof(uint16_t), 1, file);
                code->exception_table[i].catch_type = byteswap_u2(code->exception_table[i].catch_type);
            }
        } else {
            code->exception_table = NULL;
        }

        fread(&code->attributes_count, sizeof(uint16_t), 1, file);
        code->attributes_count = byteswap_u2(code->attributes_count);

        code->attributes = read_attributes_array(cf, code->attributes_count, file);

        return code;
    }
    else if (strcmp(attr_name, "ConstantValue") == 0) {
        ConstantValue_attribute *cv = malloc(sizeof(ConstantValue_attribute));
        fread(&cv->constantvalue_index, sizeof(uint16_t), 1, file);
        cv->constantvalue_index = byteswap_u2(cv->constantvalue_index);
        return cv;
    }
    
    else if (strcmp(attr_name, "Exceptions") == 0) {
        Exceptions_attribute *exc = malloc(sizeof(Exceptions_attribute));
        fread(&exc->number_of_exceptions, sizeof(uint16_t), 1, file);
        exc->number_of_exceptions = byteswap_u2(exc->number_of_exceptions);
        
        exc->exception_index_table = malloc(sizeof(uint16_t) * exc->number_of_exceptions);
        for (int i = 0; i < exc->number_of_exceptions; i++) {
            fread(&exc->exception_index_table[i], sizeof(uint16_t), 1, file);
            exc->exception_index_table[i] = byteswap_u2(exc->exception_index_table[i]);
        }
        return exc;
    }
    
    else if (strcmp(attr_name, "InnerClasses") == 0) {
        InnerClasses_attribute *ic = malloc(sizeof(InnerClasses_attribute));
        fread(&ic->number_of_classes, sizeof(uint16_t), 1, file);
        ic->number_of_classes = byteswap_u2(ic->number_of_classes);
        
        ic->classes = malloc(sizeof(InnerClass_info) * ic->number_of_classes);
        for (int i = 0; i < ic->number_of_classes; i++) {
            fread(&ic->classes[i].inner_class_info_index, sizeof(uint16_t), 1, file);
            ic->classes[i].inner_class_info_index = byteswap_u2(ic->classes[i].inner_class_info_index);
            
            fread(&ic->classes[i].outer_class_info_index, sizeof(uint16_t), 1, file);
            ic->classes[i].outer_class_info_index = byteswap_u2(ic->classes[i].outer_class_info_index);
            
            fread(&ic->classes[i].inner_name_index, sizeof(uint16_t), 1, file);
            ic->classes[i].inner_name_index = byteswap_u2(ic->classes[i].inner_name_index);
            
            fread(&ic->classes[i].inner_class_access_flags, sizeof(uint16_t), 1, file);
            ic->classes[i].inner_class_access_flags = byteswap_u2(ic->classes[i].inner_class_access_flags);
        }
        return ic;
    }
    
    else if (strcmp(attr_name, "SourceFile") == 0) {
        SourceFile_attribute *sf = malloc(sizeof(SourceFile_attribute));
        fread(&sf->sourcefile_index, sizeof(uint16_t), 1, file);
        sf->sourcefile_index = byteswap_u2(sf->sourcefile_index);
        return sf;
    }
    
    else if (strcmp(attr_name, "LineNumberTable") == 0) {
        LineNumberTable_attribute *lnt = malloc(sizeof(LineNumberTable_attribute));
        fread(&lnt->line_number_table_length, sizeof(uint16_t), 1, file);
        lnt->line_number_table_length = byteswap_u2(lnt->line_number_table_length);
        
        lnt->line_number_table = malloc(sizeof(LineNumber_info) * lnt->line_number_table_length);
        for (int i = 0; i < lnt->line_number_table_length; i++) {
            fread(&lnt->line_number_table[i].start_pc, sizeof(uint16_t), 1, file);
            lnt->line_number_table[i].start_pc = byteswap_u2(lnt->line_number_table[i].start_pc);
            
            fread(&lnt->line_number_table[i].line_number, sizeof(uint16_t), 1, file);
            lnt->line_number_table[i].line_number = byteswap_u2(lnt->line_number_table[i].line_number);
        }
        return lnt;
    }
    
    else if (strcmp(attr_name, "LocalVariableTable") == 0) {
        LocalVariableTable_attribute *lvt = malloc(sizeof(LocalVariableTable_attribute));
        fread(&lvt->local_variable_table_length, sizeof(uint16_t), 1, file);
        lvt->local_variable_table_length = byteswap_u2(lvt->local_variable_table_length);
        
        lvt->local_variable_table = malloc(sizeof(LocalVariable_info) * lvt->local_variable_table_length);
        for (int i = 0; i < lvt->local_variable_table_length; i++) {
            fread(&lvt->local_variable_table[i].start_pc, sizeof(uint16_t), 1, file);
            lvt->local_variable_table[i].start_pc = byteswap_u2(lvt->local_variable_table[i].start_pc);
            
            fread(&lvt->local_variable_table[i].length, sizeof(uint16_t), 1, file);
            lvt->local_variable_table[i].length = byteswap_u2(lvt->local_variable_table[i].length);
            
            fread(&lvt->local_variable_table[i].name_index, sizeof(uint16_t), 1, file);
            lvt->local_variable_table[i].name_index = byteswap_u2(lvt->local_variable_table[i].name_index);
            
            fread(&lvt->local_variable_table[i].descriptor_index, sizeof(uint16_t), 1, file);
            lvt->local_variable_table[i].descriptor_index = byteswap_u2(lvt->local_variable_table[i].descriptor_index);
            
            fread(&lvt->local_variable_table[i].index, sizeof(uint16_t), 1, file);
            lvt->local_variable_table[i].index = byteswap_u2(lvt->local_variable_table[i].index);
        }
        return lvt;
    }
    
    else {
        if (attr_length > 0) {
            uint8_t *raw_info = malloc(attr_length);
            fread(raw_info, 1, attr_length, file);
            return raw_info;
        }
        return NULL; 
    }
}