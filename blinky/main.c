#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "gpio_module.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#define MAX_NUMBER 6

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    logs_init();

    NRF_LOG_INFO("Logging initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    uint8_t serial_number[] = {6, 6, 0, 0};
    uint8_t btn_id = 0;

    /* Configure board. */
    gpio_module_leds_init();

    NRF_LOG_INFO("LEDS initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    gpio_module_buttons_init();

    NRF_LOG_INFO("Buttons initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    uint8_t blink_idx = 0;
    uint8_t led_idx = 0;

    /* Toggle LEDs. */
    while (true)
    {
        LOG_BACKEND_USB_PROCESS();

        if (gpio_module_button_state_get(btn_id))
        {
            if (blink_idx >= serial_number[led_idx])
            {
                led_idx = (led_idx + 1) % LEDS_NUMBER;
                blink_idx = 0;
                /* 
                * Possible to do with while but it might cause infinite  
                * loop and button state would never be read 
                */
                continue;
            }

            NRF_LOG_INFO("LED %d, Iteration %d", led_idx, blink_idx);

            NRF_LOG_PROCESS();
            LOG_BACKEND_USB_PROCESS();

            gpio_module_led_on(led_idx);
            nrf_delay_ms(300);

            gpio_module_led_off(led_idx);
            nrf_delay_ms(300);
            blink_idx++;
        }

        LOG_BACKEND_USB_PROCESS();
    }
}

/**
 *@}
 **/
