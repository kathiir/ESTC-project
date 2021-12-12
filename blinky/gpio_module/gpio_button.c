#include "gpio_button.h"

#define MAX_NUMBER 6
#define DOUBLE_CLICK_TIMEOUT   (APP_TIMER_CLOCK_FREQ >> 3)
#define LONG_PRESS_TIMEOUT     (APP_TIMER_CLOCK_FREQ >> 1)
#define BUTTON_BLOCK_TIMEOUT   (APP_TIMER_CLOCK_FREQ >> 5)

static bool volatile button_pressed = false;
static bool volatile button_blocked = false;

static uint8_t volatile button_press_count = 0;

APP_TIMER_DEF(double_click_timeout_timer_id);
APP_TIMER_DEF(long_press_timeout_timer_id);
APP_TIMER_DEF(button_block_timeout_timer_id);

static blinky_mode_t * m_mode;
static bool * m_change_value;

static void button_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (button_blocked)
    {
        return;
    }

    button_pressed ^= true;

    *m_change_value = false;

    if (button_pressed)
    {

        if (button_press_count == 0)
        {
            app_timer_start(long_press_timeout_timer_id, LONG_PRESS_TIMEOUT, NULL);
        }
    }
    else
    {
        button_blocked = true;
        *m_change_value = false;

        button_press_count += 1;

        app_timer_start(button_block_timeout_timer_id, BUTTON_BLOCK_TIMEOUT, NULL);

        app_timer_stop(long_press_timeout_timer_id);
        app_timer_stop(double_click_timeout_timer_id);
        app_timer_start(double_click_timeout_timer_id, DOUBLE_CLICK_TIMEOUT, NULL);


        if (button_press_count == 2)
        {
            *m_mode = (*m_mode + 1) % NUMBER_OF_MODES;
            NRF_LOG_INFO("Mode changed: %d", *m_mode);

            if (*m_mode == 0) {
                save_state();
            }
        }

    }
}

static void double_click_timer_timeout_handler(void *p_context)
{
    button_press_count = 0;
}

static void long_press_timer_timeout_handler(void *p_context)
{
    if (button_pressed)
    {
        *m_change_value = true;
    }
}

static void button_block_timer_timeout_handler(void *p_context)
{
    button_blocked = false;
}



void timer_init(void)
{
    ret_code_t ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    app_timer_create(&double_click_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &double_click_timer_timeout_handler);

    app_timer_create(&long_press_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &long_press_timer_timeout_handler);

    app_timer_create(&button_block_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &button_block_timer_timeout_handler);

}

void gpiote_button_init(blinky_mode_t * mode, bool * change_value)
{
    m_mode = mode;
    m_change_value = change_value;
    nrfx_gpiote_init();
    gpio_module_buttons_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    config.pull = BUTTON_PULL;
    nrfx_gpiote_in_init(BUTTON_1, &config, &button_evt_handler);
    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}
