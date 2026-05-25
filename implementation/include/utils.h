#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "class_file.h"

uint16_t byteswap_u2(uint16_t val);
uint32_t byteswap_u4(uint32_t val);
void deep_free(ClassFile *cf);

#endif