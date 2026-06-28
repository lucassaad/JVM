#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include "class_file.h"
#include "attributes.h"

// Assinaturas para resolução de classes e variáveis
ClassFile* method_area_resolve_class(cp_info **constant_pool, uint16_t index);
uint16_t resolve_field_offset(ClassFile *class_ref, cp_info **frame_cp, uint16_t indexbyte);
method_info *method_area_find_method(ClassFile *cf, const char *name, const char *descriptor, uint16_t required_flags);
Code_attribute *method_area_get_code(ClassFile *cf, method_info *method);

typedef struct {
    ClassFile **classes;
    uint16_t class_count;
    uint16_t capacity;
} MethodArea;

extern MethodArea method_area;

void method_area_init(void);
void method_area_register(ClassFile *cf);
ClassFile *method_area_find_loaded(const char *class_name);
ClassFile *method_area_load(const char *class_name);

#endif