#include "gpio_module.h"

static const uint8_t m_led_list[LEDS_NUMBER] = LEDS_LIST;

static const uint8_t m_btn_list[LEDS_NUMBER] = BUTTONS_LIST;

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

bool gpio_module_button_state_get(uint32_t btn_idx)
{
    ASSERT(btn_idx < BUTTONS_NUMBER);
    uint32_t state = nrf_gpio_pin_read(m_btn_list[btn_idx]);
    return (state == BUTTONS_ACTIVE_STATE);
}

void gpio_module_buttons_init(void)
{
    for (uint8_t btn_idx = 0; btn_idx < BUTTONS_NUMBER; ++btn_idx)
    {
        nrf_gpio_cfg_input(m_btn_list[btn_idx], BUTTON_PULL);
    }
}