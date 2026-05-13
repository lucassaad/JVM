#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <stdint.h>

typedef struct CONSTANT_Class_info{
    uint8_t tag;
    uint16_t name_index;
} CONSTANT_Class_info;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} CONSTANT_Fieldref_info;


typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} CONSTANT_Methodref_info;


typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} CONSTANT_InterfaceMethodref_info;


typedef struct {
    uint8_t tag;
    uint16_t string_index;
} CONSTANT_String_info;


typedef struct {
    uint8_t tag;
    uint32_t bytes;
} CONSTANT_Integer_info;


typedef struct {
    uint8_t tag;
    uint32_t bytes;
} CONSTANT_Float_info;


typedef struct {
    uint8_t tag;
    uint32_t high_bytes;
    uint32_t low_bytes;
} CONSTANT_Long_info;


typedef struct {
    uint8_t tag;
    uint32_t high_bytes;
    uint32_t low_bytes;
} CONSTANT_Double_info;


typedef struct {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
} CONSTANT_NameAndType_info;


typedef struct {
    uint8_t tag;
    uint16_t length;
    uint8_t bytes[];
} CONSTANT_Utf8_info;


typedef struct {
    uint8_t tag;
    uint8_t reference_kind;
    uint16_t reference_index;
} CONSTANT_MethodHandle_info;


typedef struct {
    uint8_t tag;
    uint16_t descriptor_index;
} CONSTANT_MethodType_info;


typedef struct {
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} CONSTANT_Dynamic_info;


typedef struct {
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} CONSTANT_InvokeDynamic_info;


typedef struct {
    uint8_t tag;
    uint16_t name_index;
} CONSTANT_Module_info;


typedef struct {
    uint8_t tag;
    uint16_t name_index;
} CONSTANT_Package_info;


typedef enum cp_tags {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12, 
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_Dynamic = 17,
    CONSTANT_InvokeDynamic = 18,
    CONSTANT_Module = 19,
    CONSTANT_Package = 20
} cp_tags;

#endif
