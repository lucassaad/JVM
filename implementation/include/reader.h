#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "class_file.h"

// Lê e aloca dinamicamente um array de atributos a partir do arquivo.
attribute_info* read_attributes_array(uint16_t count, FILE *file);

// Assinaturas das funções de leitura
int read_classfile(ClassFile *cf, FILE *file);

#endif