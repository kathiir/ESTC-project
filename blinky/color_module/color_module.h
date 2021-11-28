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

typedef struct 
{
    uint8_t hue;
    uint8_t saturation;
    uint8_t value;
} color_hsv_t;

void color_hsv_to_rgb(const color_hsv_t *hsv_values, color_rgb_t *rgb_values);

#endif /* COLOR_MODULE_H */