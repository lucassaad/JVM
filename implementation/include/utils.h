#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Inverte a ordem dos bytes de um valor de 16 bits (u2).
uint16_t byteswap_u2(uint16_t val);

// Inverte a ordem dos bytes de um valor de 32 bits (u4).
uint32_t byteswap_u4(uint32_t val);

#endif