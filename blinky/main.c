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
#include "gpio_pwm.h"

#include "app_timer.h"

#define MAX_NUMBER 6
#define BLINK_PERIOD 600
#define DOUBLE_CLICK_TIMEOUT (APP_TIMER_CLOCK_FREQ >> 1)
#define DOUBLE_CLICK_MIN_TIMEOUT (APP_TIMER_CLOCK_FREQ >> 4)

bool volatile smooth_blinking_on = false;
bool volatile timed_out = true;

uint32_t volatile time;

APP_TIMER_DEF(timeout_timer_id);

void button_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (action == NRF_GPIOTE_POLARITY_HITOLO)
    {
        if (!timed_out && app_timer_cnt_diff_compute(app_timer_cnt_get(), time) < DOUBLE_CLICK_TIMEOUT && app_timer_cnt_diff_compute(app_timer_cnt_get(), time) > DOUBLE_CLICK_MIN_TIMEOUT)
        {
            smooth_blinking_on ^= true;
            timed_out = true;
        }
        else if (app_timer_cnt_diff_compute(app_timer_cnt_get(), time) > DOUBLE_CLICK_TIMEOUT)
        {
            time = app_timer_cnt_get();
            timed_out = false;
        }
    }
}

static void timer_timeout_handler(void *p_context)
{
}

void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void systick_init(void)
{
    nrfx_systick_init();
}

void timer_init(void)
{
    ret_code_t ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    app_timer_create(&timeout_timer_id, APP_TIMER_MODE_REPEATED, &timer_timeout_handler);
    app_timer_start(timeout_timer_id, 200000000, NULL);
    time = app_timer_cnt_get();
}

void gpiote_button_init(void)
{
    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    config.pull = BUTTON_PULL;
    nrfx_gpiote_in_init(BUTTON_1, &config, &button_evt_handler);
    nrfx_gpiote_in_event_enable(BUTTON_1, true);
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

    systick_init();

    NRF_LOG_INFO("SysTick initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    timer_init();

    NRF_LOG_INFO("App timer initialized");
    NRF_LOG_PROCESS();
    LOG_BACKEND_USB_PROCESS();

    gpiote_button_init();

    NRF_LOG_INFO("GPIOTE initialized");
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

        if (smooth_blinking_on)
        {
            duty_cycle = (timer < BLINK_PERIOD / 2)
                             ? (timer * 200 / BLINK_PERIOD)
                             : (100 - (timer * 200 / BLINK_PERIOD) % 100);
            timer++;
        }

        NRF_LOG_INFO("LED %d, Iteration %d, Duty cycle %d",
                     led_idx, blink_idx, duty_cycle);

        NRF_LOG_FLUSH();
        LOG_BACKEND_USB_PROCESS();

        pwm_led_duty_cycle(led_idx, duty_cycle);

        if (timer > BLINK_PERIOD)
        {
            timer = 0;
            duty_cycle = 0;
            blink_idx++;
        }

        LOG_BACKEND_USB_PROCESS();
    }
}

/**
 *@}
 **/
