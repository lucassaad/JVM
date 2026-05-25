#ifndef VIEWER_H
#define VIEWER_H

#include <stdint.h>
#include "class_file.h"

// Funções principais de exibição
void print_general_information(FILE *out, ClassFile *cf);
//Implementar demais funções de exibição

// Funções auxiliares 
const char* get_java_version_string(uint16_t major_version);
const char* get_class_access_flags_string(uint16_t flags);
const char* get_class_name_string(ClassFile *cf, uint16_t class_index);

#endif