#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "gpio_module.h"

#define MAX_NUMBER 6

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    uint8_t serial_number[] = {6, 6, 0, 0};
    uint8_t btn_id = 0;
    /* Configure board. */
    gpio_module_leds_init();
    gpio_module_buttons_init();
    /* Toggle LEDs. */
    while (true)
    {
        for (int led_idx = 0; led_idx < LEDS_NUMBER; led_idx++)
        {

            for (int i = 0; i < serial_number[led_idx]; i++)
            {

                gpio_module_led_on(led_idx);

                while (!gpio_module_button_state_get(btn_id))
                {
                    /* wait */
                }

                nrf_delay_ms(500);
                gpio_module_led_off(led_idx);

                while (!gpio_module_button_state_get(btn_id))
                {
                    /* wait */
                }

                nrf_delay_ms(500);
            }
        }
        nrf_delay_ms(1000);
    }
}

/**
 *@}
 **/
