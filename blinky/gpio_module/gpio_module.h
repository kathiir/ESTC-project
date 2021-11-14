#ifndef GPIO_MODULE_H
#define GPIO_MODULE_H

#include "pca10059.h"

void gpio_module_led_on(uint32_t led_idx);

void gpio_module_led_off(uint32_t led_idx);

void gpio_module_led_invert(uint32_t led_idx);

void gpio_module_leds_on(void);

void gpio_module_leds_off(void);

void gpio_module_leds_init(void);

bool gpio_module_button_state_get(uint32_t button_idx);

void gpio_module_buttons_init(void);


#endif /* GPIO_MODULE_H */