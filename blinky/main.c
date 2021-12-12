#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "nrfx_gpiote.h"

#include "gpio_module.h"
#include "color_module.h"
#include "gpio_pwm.h"

#include "nvmc_module.h"

#include "app_timer.h"

void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

pwm_hsv_t volatile hsv_tmp_color;
bool volatile change_value   = false;

blinky_mode_t volatile mode = BLINKY_MODE_NO_INPUT;


/**
 * @brief Function for application main entry.
 */
int main(void)
{

    logs_init();

    /* Configure board. */
    gpio_module_leds_init();

    timer_init();

    gpiote_button_init(&mode, &change_value);

    nvmc_init(&hsv_tmp_color);

    pwm_init(&hsv_tmp_color, &mode, &change_value);


    while (true)
    {
        __WFE();

        NRF_LOG_PROCESS();
        LOG_BACKEND_USB_PROCESS();
    }
}
