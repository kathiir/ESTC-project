#ifndef COLOR_MODULE_H
#define COLOR_MODULE_H

#include "stdint.h"

#define RGB_LED_TOP_VALUE   255

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_rgb_t;

#endif /* COLOR_MODULE_H */