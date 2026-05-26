#ifndef VIEWER_H
#define VIEWER_H

#include <stdint.h>
#include "class_file.h"
#include "attributes.h"

//---------------------------------------------------------------------------------------
// Funções Principais de Exibição
// Função para exibir as informações gerais do arquivo .class 
void print_general_information(FILE *out, ClassFile *cf);

// Função para exibir todas as entradas mapeadas na tabela do Pool de Constantes
void print_constant_pool(FILE *out, ClassFile *cf);

// Função para exibir a lista de interfaces 
void print_interfaces(FILE *out, ClassFile *cf);

// Função para exibir os campos (variáveis) declarados na classe
void print_fields(FILE *out, ClassFile *cf);

// Função para exibir os métodos declarados na classe
void print_methods(FILE *out, ClassFile *cf);

// Função para exibir os atributos 
void print_attributes(FILE *out, ClassFile *cf);

//---------------------------------------------------------------------------------------
// Funções Auxiliares 
// Função para converter o número da versão Major para a sua versão Java 
const char* get_java_version_string(uint16_t major_version);

// Função para converter a máscara de bits das flags da classe em uma string 
const char* get_class_access_flags_string(uint16_t flags);

// Função para busca e retornar o nome da classe 
const char* get_class_name_string(ClassFile *cf, uint16_t class_index);

// Função auxiliar para exibir as flags de acesso específicas de um field
void print_field_access_flags(FILE *out, uint16_t flags);

// Função auxiliar para exibir as flags de acesso específicas de um method
void print_method_access_flags(FILE *out, uint16_t flags);

// Função para decodificar os bytes brutos e exibe informações detalhadas de um atributo
void print_specific_attribute_info(FILE *out, ClassFile *cf, attribute_info *attr);

// Função para decodificar structs de atributos e exibir na tela
void print_specific_attribute_info(FILE *out, ClassFile *cf, attribute_info *attr);

//---------------------------------------------------------------------------------------

#endif