#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "gpio_module.h"
#include "gpio_pwm.h"

#define MAX_NUMBER 6
#define BLINK_PERIOD 600

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

    nrfx_systick_init();

    NRF_LOG_INFO("SysTick initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    uint8_t blink_idx = 0;
    uint8_t led_idx = 0;
    uint16_t timer = 0;
    uint8_t duty_cycle = 0;

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

            duty_cycle = (timer < BLINK_PERIOD / 2) 
                        ? (timer * 200 / BLINK_PERIOD) 
                        : (100 - (timer * 200 / BLINK_PERIOD) % 100); 

            NRF_LOG_INFO("LED %d, Iteration %d, Duty cycle %d", 
                         led_idx, blink_idx, duty_cycle);

            NRF_LOG_PROCESS();
            LOG_BACKEND_USB_PROCESS();

            pwm_led_duty_cycle(led_idx, duty_cycle);

            timer++;
            if (timer > BLINK_PERIOD)
            {
                timer = 0;
                duty_cycle = 0;
                blink_idx++;
            }
        }

        LOG_BACKEND_USB_PROCESS();
    }
}

/**
 *@}
 **/
