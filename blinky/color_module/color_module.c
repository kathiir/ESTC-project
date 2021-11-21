#include "color_module.h"


void color_hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t * rgb_values) {

    uint8_t region;
    uint8_t remainder;

    uint16_t p, q, t;

    if (v == 0) {
        rgb_values[0] = 0;
        rgb_values[1] = 0;
        rgb_values[2] = 0;
        return;
    }

    if (s == 0)
    {
        rgb_values[0] = v;
        rgb_values[1] = v;
        rgb_values[2] = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    if (region == 0) {
        rgb_values[0] = v;
        rgb_values[1] = t;
        rgb_values[2] = p;
    }
    else if (region == 1)
    {
        rgb_values[0] = q;
        rgb_values[1] = v;
        rgb_values[2] = p;
    }
    else if (region == 2)
    {
        rgb_values[0] = p;
        rgb_values[1] = v;
        rgb_values[2] = t;
    }
    else if (region == 3)
    {
        rgb_values[0] = p;
        rgb_values[1] = q;
        rgb_values[2] = v;
    }
    else if (region == 4)
    {
        rgb_values[0] = t;
        rgb_values[1] = p;
        rgb_values[2] = v;
    }
    else
    {
        rgb_values[0] = v;
        rgb_values[1] = p;
        rgb_values[2] = q;
    }
}