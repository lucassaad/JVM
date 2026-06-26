#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include "method_area.h"
#include "constant_pool.h"  

ClassFile* method_area_resolve_class(cp_info **constant_pool, uint16_t index) {
    // TODO: Implementar a busca real da classe na Área de Métodos global.
    // O index aponta para CONSTANT_Class_info no constant_pool.
    
    // Por enquanto, para compilar a Issue #82 com sucesso:
    printf("Aviso: method_area_resolve_class invocada, mas a busca global ainda nao foi implementada.\n");
    
    // Para evitar que o compilador reclame dos parâmetros não usados:
    (void)constant_pool;
    (void)index;
    
    return NULL; 
}

// Função que calcula o índice real no vetor obj->fields usando as strings (Utf8)
uint16_t resolve_field_offset(ClassFile *class_ref, cp_info **frame_cp, uint16_t indexbyte) {
    
    // Acessa o Constant Pool DO FRAME (quem chamou a instrução)
    cp_info *fieldref_cp = frame_cp[indexbyte];
    CONSTANT_Fieldref_info *fieldref = (CONSTANT_Fieldref_info *)fieldref_cp->info;
    
    cp_info *nt_cp = frame_cp[fieldref->name_and_type_index];
    CONSTANT_NameAndType_info *nt = (CONSTANT_NameAndType_info *)nt_cp->info;
    
    CONSTANT_Utf8_info *target_name = (CONSTANT_Utf8_info *)frame_cp[nt->name_index]->info;
    CONSTANT_Utf8_info *target_desc = (CONSTANT_Utf8_info *)frame_cp[nt->descriptor_index]->info;
    
    // Variável para rastrear a posição física no vetor
    uint16_t memory_offset = 0;
    
    // Varre os fields da classe do objeto
    for (int i = 0; i < class_ref->fields_count; i++) {
        if ((class_ref->fields[i].access_flags & 0x0008) == 0) {
            
            // Extrai as strings do Constant Pool DO OBJETO
            CONSTANT_Utf8_info *field_name = (CONSTANT_Utf8_info *)class_ref->constant_pool[class_ref->fields[i].name_index]->info;
            CONSTANT_Utf8_info *field_desc = (CONSTANT_Utf8_info *)class_ref->constant_pool[class_ref->fields[i].descriptor_index]->info;
            
            // Compara byte a byte para ver se é o field correto
            if (target_name->length == field_name->length &&
                strncmp((char*)target_name->bytes, (char*)field_name->bytes, target_name->length) == 0 &&
                target_desc->length == field_desc->length &&
                strncmp((char*)target_desc->bytes, (char*)field_desc->bytes, target_desc->length) == 0) {
                return memory_offset; // Encontrou!
            }
            
            char first_char = (char)field_desc->bytes[0];
            if (first_char == 'J' || first_char == 'D') {
                memory_offset += 2; 
            } else {
                memory_offset += 1; 
            }
        }
    }
    
    fprintf(stderr, "NoSuchFieldError: Field nao encontrado na resolucao de offset.\n");
    exit(1);
}

method_info *method_area_find_method(ClassFile *cf, const char *name, const char *descriptor, uint16_t required_flags) {
    for (uint16_t i = 0; i < cf->methods_count; i++) {

        method_info *method = &cf->methods[i];

        CONSTANT_Utf8_info *method_name =
            (CONSTANT_Utf8_info *)
            cf->constant_pool[method->name_index]->info;

        CONSTANT_Utf8_info *method_desc =
            (CONSTANT_Utf8_info *)
            cf->constant_pool[method->descriptor_index]->info;

        if ((method->access_flags & required_flags) != required_flags)
            continue;

        if (method_name->length != strlen(name))
            continue;

        if (method_desc->length != strlen(descriptor))
            continue;

        if (strncmp((char *)method_name->bytes,
                    name,
                    method_name->length) != 0)
            continue;

        if (strncmp((char *)method_desc->bytes,
                    descriptor,
                    method_desc->length) != 0)
            continue;

        return method;
    }

    return NULL;
}

Code_attribute *method_area_get_code(ClassFile *cf, method_info *method) {

    for (uint16_t i = 0; i < method->attributes_count; i++) {

        attribute_info *attr = &method->attributes[i];

        CONSTANT_Utf8_info *utf8 =
            (CONSTANT_Utf8_info *) cf->constant_pool[attr->attribute_name_index]->info;

        if (utf8->length == 4 &&
            strncmp((char *)utf8->bytes, "Code", 4) == 0) {

            return (Code_attribute *) attr->info;
        }
    }

    return NULL;
}