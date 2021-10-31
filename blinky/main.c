#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#define MAX_NUMBER 6

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    uint8_t serial_number[] = {6, 6, 0, 0};
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);

    /* Toggle LEDs. */
    while (true)
    {
        for (int i = 0; i < MAX_NUMBER; i++)
        {
            for (int led_idx = 0; led_idx < LEDS_NUMBER; led_idx++)
            {
                if (serial_number[led_idx] > i)
                {
                    bsp_board_led_on(led_idx);
                }
            }
            nrf_delay_ms(500);
            bsp_board_leds_off();
            nrf_delay_ms(500);
        }
        nrf_delay_ms(2000);
    }
}

/**
 *@}
 **/
