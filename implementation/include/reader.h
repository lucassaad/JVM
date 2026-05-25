#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "class_file.h"
#include "constant_pool.h"

// Lê e aloca dinamicamente um array de atributos a partir do arquivo.
attribute_info* read_attributes_array(uint16_t count, FILE *file);

//---------------------------------------------------------------------------------------
// Funções para Validação

// Função para validar o this_class
int validate_this_class(ClassFile *cf);

// Função para validar o super_class
int validate_super_class(ClassFile *cf);

// Função para validar interfaces
int validate_interfaces(ClassFile *cf);

// Função para validar fields
int validate_fields(ClassFile *cf);

// Função para validar methods
int validate_methods(ClassFile *cf);

// Função para validar attributes
int validate_attributes(ClassFile *cf);
//---------------------------------------------------------------------------------------

// Assinaturas das funções de leitura
int read_classfile(ClassFile *cf, FILE *file);

#endif