#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "class_file.h"
#include "constant_pool.h"

// Assinaturas das funções de leitura
int read_classfile(ClassFile *cf, FILE *file);

#endif