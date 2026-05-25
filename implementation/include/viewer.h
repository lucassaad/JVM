#ifndef VIEWER_H
#define VIEWER_H

#include <stdint.h>
#include "class_file.h"

// Funções principais de exibição
void print_general_information(ClassFile *cf);
void print_constant_pool(ClassFile *cf);
void print_fields(ClassFile *cf);
void print_methods(ClassFile *cf) ;

// Funções auxiliares 
const char* get_java_version_string(uint16_t major_version);
void print_class_attributes(ClassFile *cf);
void print_field_access_flags(uint16_t flags);
void print_method_access_flags(uint16_t flags);

#endif