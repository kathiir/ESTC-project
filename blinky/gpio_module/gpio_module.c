#include "gpio_module.h"

static const uint8_t m_led_list[LEDS_NUMBER] = LEDS_LIST;

void gpio_module_led_on(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_write(m_led_list[led_idx], LEDS_ACTIVE_STATE);
}

void gpio_module_led_off(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_write(m_led_list[led_idx], !LEDS_ACTIVE_STATE);
}

void gpio_module_led_invert(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_toggle(m_led_list[led_idx]);
}

void gpio_module_leds_on(void)
{
    for (size_t led_idx = 0; led_idx < LEDS_NUMBER; led_idx++)
    {
        gpio_module_led_on(led_idx);
    }
}

void gpio_module_leds_off(void)
{
    for (size_t led_idx = 0; led_idx < LEDS_NUMBER; led_idx++)
    {
        gpio_module_led_off(led_idx);
    }
}

void gpio_module_leds_init(void)
{
    for (uint8_t led_idx = 0; led_idx < LEDS_NUMBER; ++led_idx)
    {
        nrf_gpio_cfg_output(m_led_list[led_idx]);
    }

    gpio_module_leds_off();
}