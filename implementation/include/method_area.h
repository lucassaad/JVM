#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include "class_file.h"

// Assinaturas para resolução de classes e variáveis
ClassFile* method_area_resolve_class(cp_info **constant_pool, uint16_t index);
uint16_t resolve_field_offset(ClassFile *class_ref, cp_info **frame_cp, uint16_t indexbyte);

#endif