#ifndef VIEWER_H
#define VIEWER_H

#include "class_file.h"
#include "constant_pool.h"

// Funções principais de exibição
void print_general_information(ClassFile *cf);
void print_constant_pool(ClassFile *cf);
//Implementar demais funções de exibição

// Funções auxiliares 
const char* get_java_version_string(uint16_t major_version);

#endif