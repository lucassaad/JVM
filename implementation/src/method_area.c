#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include "method_area.h"
#include "constant_pool.h"  
#include "reader.h"


extern FILE *exec_log;

// inicia a Method Area global
MethodArea method_area = {
    NULL,
    0,
    0
};

// Função para inicializar a Method Area
void method_area_init(void)
{
    method_area.capacity = 8;
    method_area.class_count = 0;
    method_area.classes = malloc(sizeof(ClassFile *) * method_area.capacity);

    if (method_area.classes == NULL) {
        fprintf(stderr, "Erro ao inicializar Method Area.\n");
        exit(1);
    }
}

// Função para registrar uma classe na Method Area
void method_area_register(ClassFile *cf)
{
    if (method_area.class_count == method_area.capacity) {

        method_area.capacity *= 2;

        method_area.classes = realloc(method_area.classes, sizeof(ClassFile *) * method_area.capacity);

        if (method_area.classes == NULL) {
            fprintf(stderr, "Erro ao expandir Method Area.\n");
            exit(1);
        }
    }

    method_area.classes[method_area.class_count++] = cf;
}

// Função para buscar uma classe carregada na Method Area pelo nome
ClassFile *method_area_find_loaded(const char *class_name)
{
    for (uint16_t i = 0; i < method_area.class_count; i++) {

        ClassFile *cf = method_area.classes[i];

        CONSTANT_Class_info *this_class =
            (CONSTANT_Class_info *)
            cf->constant_pool[cf->this_class]->info;

        CONSTANT_Utf8_info *utf8 =
            (CONSTANT_Utf8_info *)
            cf->constant_pool[this_class->name_index]->info;

        if (utf8->length == strlen(class_name) && strncmp((char *)utf8->bytes, class_name, utf8->length) == 0) {
            return cf;
        }
    }

    return NULL;
}

// Função para carregar uma classe da Method Area a partir de um arquivo .class
ClassFile *method_area_load(const char *class_name)
{
    char filename[512];

    snprintf(filename, sizeof(filename), "exemplos/%s.class", class_name);

    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
        return NULL;

    ClassFile *cf = malloc(sizeof(ClassFile));

    if (cf == NULL) {
        fclose(fp);
        return NULL;
    }

    if (read_classfile(cf, fp) != 0) {
        fclose(fp);
        free(cf);
        return NULL;
    }

    fclose(fp);

    method_area_register(cf);

    return cf;
}

ClassFile *method_area_resolve_class(cp_info **constant_pool, uint16_t index) {
    CONSTANT_Class_info *class_info = (CONSTANT_Class_info *)constant_pool[index]->info;

    CONSTANT_Utf8_info *utf8 = (CONSTANT_Utf8_info *)constant_pool[class_info->name_index]->info;

    char class_name[256];

    memcpy(class_name, utf8->bytes, utf8->length);

    class_name[utf8->length] = '\0';

    ClassFile *cf = method_area_find_loaded(class_name);

    if (cf != NULL) return cf;

    // Substitua o printf antigo por este condicional:
    if (exec_log != NULL) {
        fprintf(exec_log, "[Method Area] Carregando classe dinamicamente: %s...\n", class_name);
    }

    cf = method_area_load(class_name);

    if (cf == NULL) {
        fprintf(stderr, "Nao foi possivel carregar %s.class\n", class_name);
        exit(1);
    }

    return cf;
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