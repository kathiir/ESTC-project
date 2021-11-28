#include "color_module.h"


void color_hsv_to_rgb(const color_hsv_t *hsv_values, color_rgb_t *rgb_values) {

    uint8_t region;
    uint8_t remainder;

    uint16_t p, q, t;

    if (hsv_values->value == 0) {
        rgb_values->red   = 0;
        rgb_values->green = 0;
        rgb_values->blue  = 0;
        return;
    }

    if (hsv_values->saturation == 0)
    {
        rgb_values->red = hsv_values->value;
        rgb_values->green = hsv_values->value;
        rgb_values->blue = hsv_values->value;
        return;
    }

    region = hsv_values->hue / 43;
    remainder = (hsv_values->hue - (region * 43)) * 6;

    p = (hsv_values->value * (255 - hsv_values->saturation)) >> 8;
    q = (hsv_values->value * (255 - ((hsv_values->saturation * remainder) >> 8))) >> 8;
    t = (hsv_values->value * (255 - ((hsv_values->saturation * (255 - remainder)) >> 8))) >> 8;

    if (region == 0) {
        rgb_values->red = hsv_values->value;
        rgb_values->green = t;
        rgb_values->blue = p;
    }
    else if (region == 1)
    {
        rgb_values->red = q;
        rgb_values->green = hsv_values->value;
        rgb_values->blue = p;
    }
    else if (region == 2)
    {
        rgb_values->red = p;
        rgb_values->green = hsv_values->value;
        rgb_values->blue = t;
    }
    else if (region == 3)
    {
        rgb_values->red = p;
        rgb_values->green = q;
        rgb_values->blue = hsv_values->value;
    }
    else if (region == 4)
    {
        rgb_values->red = t;
        rgb_values->green = p;
        rgb_values->blue = hsv_values->value;
    }
    else
    {
        rgb_values->red = hsv_values->value;
        rgb_values->green = p;
        rgb_values->blue = q;
    }
}
