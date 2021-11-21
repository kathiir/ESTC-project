#ifndef COLOR_MODULE_H
#define COLOR_MODULE_H

#include "stdint.h"

void color_hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t * rgb_values);

#endif /* COLOR_MODULE_H */