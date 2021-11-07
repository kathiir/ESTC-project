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

    uint8_t serial_number[] = {6, 6, 0, 0};
    uint8_t btn_id = 0;

    /* Configure board. */
    gpio_module_leds_init();
    NRF_LOG_INFO("LEDS initialized");

    gpio_module_buttons_init();
    NRF_LOG_INFO("Buttons initialized");

    NRF_LOG_INFO("Starting program");

    LOG_BACKEND_USB_PROCESS();
    NRF_LOG_PROCESS();

    /* Toggle LEDs. */
    while (true)
    {
        // LOG_BACKEND_USB_PROCESS();
        // NRF_LOG_PROCESS();

        NRF_LOG_INFO("Starting loop");

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();

        for (int led_idx = 0; led_idx < LEDS_NUMBER; led_idx++)
        {
            // LOG_BACKEND_USB_PROCESS();
            // NRF_LOG_PROCESS();

            NRF_LOG_INFO("Iterating over LED %d", led_idx);

            LOG_BACKEND_USB_PROCESS();
            NRF_LOG_PROCESS();

            for (int i = 0; i < serial_number[led_idx]; i++)
            {
                // LOG_BACKEND_USB_PROCESS();
                // NRF_LOG_PROCESS();

                NRF_LOG_INFO("Iteration %d", i);

                LOG_BACKEND_USB_PROCESS();
                NRF_LOG_PROCESS();
                
                NRF_LOG_INFO("LED %d is ON", led_idx);

                LOG_BACKEND_USB_PROCESS();
                NRF_LOG_PROCESS();

                while (!gpio_module_button_state_get(btn_id))
                {
                    LOG_BACKEND_USB_PROCESS();
                    NRF_LOG_PROCESS();

                    /* wait */
                }
                // LOG_BACKEND_USB_PROCESS();
                // NRF_LOG_PROCESS();

                gpio_module_led_on(led_idx);

                nrf_delay_ms(300);

                while (!gpio_module_button_state_get(btn_id))
                {
                    LOG_BACKEND_USB_PROCESS();
                    NRF_LOG_PROCESS();

                    /* wait */
                }

                gpio_module_led_off(led_idx);
                nrf_delay_ms(500);

                NRF_LOG_INFO("LED %d is OFF", led_idx);

                LOG_BACKEND_USB_PROCESS();
                NRF_LOG_PROCESS();
            }
        }
        nrf_delay_ms(1000);
    }
}

/**
 *@}
 **/
