#include <stdint.h>

// Inverte 2 bytes (u2 / uint16_t)
uint16_t byteswap_u2(uint16_t val) {
    return (val >> 8) | (val << 8);
}

// Inverte 4 bytes (u4 / uint32_t)
uint32_t byteswap_u4(uint32_t val) {
    return ((val >> 24) & 0x000000FF) |
           ((val >> 8)  & 0x0000FF00) |
           ((val << 8)  & 0x00FF0000) |
           ((val << 24) & 0xFF000000);
}