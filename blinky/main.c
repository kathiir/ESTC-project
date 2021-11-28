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
#include "nrfx_pwm.h"

#include "gpio_module.h"
#include "color_module.h"

#include "app_timer.h"


#define MAX_NUMBER 6
#define DOUBLE_CLICK_TIMEOUT   (APP_TIMER_CLOCK_FREQ >> 3)
#define LONG_PRESS_TIMEOUT     (APP_TIMER_CLOCK_FREQ >> 1)
#define BUTTON_BLOCK_TIMEOUT   (APP_TIMER_CLOCK_FREQ >> 5)


#define CONTROL_LED_TOP_VALUE       10000
#define CONTROL_LED_SLOW_STEP_VALUE 100
#define CONTROL_LED_FAST_STEP_VALUE 400

#define HUE_TOP_VALUE               25599
#define SATURATION_TOP_VALUE        25599
#define BRIGHTNESS_TOP_VALUE        25599

#define NUMBER_OF_MODES 4
#define MODE_NO_INPUT   0
#define MODE_HUE_MOD    1
#define MODE_SAT_MOD    2
#define MODE_BRT_MOD    3

#define NUMBER_OF_PLAYBACKS 1

typedef enum blinky_mode_e {
    BLINKY_MODE_NO_INPUT = MODE_NO_INPUT,
    BLINKY_MODE_HUE_MOD  = MODE_HUE_MOD,
    BLINKY_MODE_SAT_MOD  = MODE_SAT_MOD,
    BLINKY_MODE_BRT_MOD  = MODE_BRT_MOD
} blinky_mode_t;

typedef struct
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t value;
} hsv_t;

static nrfx_pwm_t m_pwm_control = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t m_pwm_rgb     = NRFX_PWM_INSTANCE(1);

static nrf_pwm_values_individual_t m_control_seq_values;
static nrf_pwm_values_individual_t m_rgb_seq_values;
static bool volatile m_control_value_state_up = true;

static nrf_pwm_sequence_t const    m_control_seq =
{
    .values.p_individual = &m_control_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_control_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};
static nrf_pwm_sequence_t const    m_rgb_seq =
{
    .values.p_individual = &m_rgb_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_rgb_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

uint16_t volatile control_led_value = 0;
hsv_t volatile hsv_tmp_values = { 0, SATURATION_TOP_VALUE, BRIGHTNESS_TOP_VALUE };

blinky_mode_t volatile mode = BLINKY_MODE_NO_INPUT;

bool volatile button_pressed = false;
bool volatile button_blocked = false;
bool volatile change_value   = false;

uint8_t volatile button_press_count = 0;

uint32_t volatile time;

APP_TIMER_DEF(double_click_timeout_timer_id);
APP_TIMER_DEF(long_press_timeout_timer_id);
APP_TIMER_DEF(button_block_timeout_timer_id);


static void button_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (button_blocked)
    {
        return;
    }
    
    button_pressed ^= true;

    change_value = false;

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
        change_value = false;

        button_press_count += 1;
        
        app_timer_start(button_block_timeout_timer_id, BUTTON_BLOCK_TIMEOUT, NULL);

        app_timer_stop(long_press_timeout_timer_id);
        app_timer_stop(double_click_timeout_timer_id);
        app_timer_start(double_click_timeout_timer_id, DOUBLE_CLICK_TIMEOUT, NULL);


        if (button_press_count == 2) 
        {
            mode = (mode + 1) % NUMBER_OF_MODES;
            NRF_LOG_INFO("Mode changed: %d", mode);
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
        change_value = true;
    }
}

static void button_block_timer_timeout_handler(void *p_context)
{
    button_blocked = false;
}

static void pwm_control_evt_handler(nrfx_pwm_evt_type_t event_type) 
{
    if (event_type == NRFX_PWM_EVT_FINISHED)
    {
        nrf_pwm_values_individual_t * p_channels = &m_control_seq_values;

        if (mode == BLINKY_MODE_NO_INPUT) 
        {
            control_led_value = 0;
        } 
        else if (mode == BLINKY_MODE_BRT_MOD)
        {
            control_led_value = CONTROL_LED_TOP_VALUE;
        }
        else
        {
            uint16_t step;

            if (mode == BLINKY_MODE_HUE_MOD)
            {
                step = CONTROL_LED_SLOW_STEP_VALUE;
            }
            else
            {
                step = CONTROL_LED_FAST_STEP_VALUE;
            }
            
            if (m_control_value_state_up) 
            {
                control_led_value += step;
                if (control_led_value >= CONTROL_LED_TOP_VALUE) 
                {
                    control_led_value = CONTROL_LED_TOP_VALUE;
                    m_control_value_state_up = false;
                }
            } 
            else
            {
                control_led_value -= step;
                if (control_led_value > CONTROL_LED_TOP_VALUE || control_led_value <= 0) 
                {
                    control_led_value = 0;
                    m_control_value_state_up = true;
                }
            }
        }

        p_channels->channel_0 = control_led_value;
    }
}

static void pwm_rgb_evt_handler(nrfx_pwm_evt_type_t event_type) 
{
    if (event_type == NRFX_PWM_EVT_FINISHED)
    {
        if (change_value && (mode != BLINKY_MODE_NO_INPUT)) 
        {
            nrf_pwm_values_individual_t * p_channels = &m_rgb_seq_values;

            color_hsv_t hsv_values;
            color_rgb_t rgb_led_values;
            
            if (mode == BLINKY_MODE_HUE_MOD)
            {
                hsv_tmp_values.hue = (hsv_tmp_values.hue + 1) % HUE_TOP_VALUE;
                NRF_LOG_INFO("Hue value: %d", hsv_tmp_values.hue);
            }
            else if (mode == BLINKY_MODE_SAT_MOD)
            {
                hsv_tmp_values.saturation = (hsv_tmp_values.saturation + 1) % SATURATION_TOP_VALUE;
                NRF_LOG_INFO("Saturation value: %d", hsv_tmp_values.saturation);
            }
            else if (mode == BLINKY_MODE_BRT_MOD)
            {
                hsv_tmp_values.value = (hsv_tmp_values.value + 1) % BRIGHTNESS_TOP_VALUE;
                NRF_LOG_INFO("Brightness value: %d", hsv_tmp_values.value);
            }

            hsv_values.hue = hsv_tmp_values.hue / 100;
            hsv_values.saturation = hsv_tmp_values.saturation / 100;
            hsv_values.value = hsv_tmp_values.value / 100;
            
            color_hsv_to_rgb(&hsv_values, &rgb_led_values);
            
            p_channels->channel_0 = rgb_led_values.red;
            p_channels->channel_1 = rgb_led_values.green;
            p_channels->channel_2 = rgb_led_values.blue;
        }
        
    }
}


void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void timer_init(void)
{
    ret_code_t ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    app_timer_create(&double_click_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &double_click_timer_timeout_handler);

    app_timer_create(&long_press_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &long_press_timer_timeout_handler);

    app_timer_create(&button_block_timeout_timer_id, APP_TIMER_MODE_SINGLE_SHOT, &button_block_timer_timeout_handler);

}

void gpiote_button_init(void)
{
    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    config.pull = BUTTON_PULL;
    nrfx_gpiote_in_init(BUTTON_1, &config, &button_evt_handler);
    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}


void pwm_rgb_init(void){

    nrfx_pwm_config_t config = 
    {
        .output_pins = 
        {
            LED_2 | NRFX_PWM_PIN_INVERTED,
            LED_3 | NRFX_PWM_PIN_INVERTED,
            LED_4 | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock = NRF_PWM_CLK_1MHz,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = RGB_LED_TOP_VALUE,
        .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = NRF_PWM_STEP_AUTO
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm_rgb, &config, pwm_rgb_evt_handler));

    color_hsv_t hsv = 
    { 
        hsv_tmp_values.hue / 100, 
        hsv_tmp_values.saturation / 100, 
        hsv_tmp_values.value / 100 
    };

    color_rgb_t rgb;

    color_hsv_to_rgb(&hsv, &rgb);

    m_rgb_seq_values.channel_0 = rgb.red;
    m_rgb_seq_values.channel_1 = rgb.green;
    m_rgb_seq_values.channel_2 = rgb.blue;
    m_rgb_seq_values.channel_3 = 0;

    nrfx_pwm_simple_playback(&m_pwm_rgb, &m_rgb_seq, NUMBER_OF_PLAYBACKS, NRFX_PWM_FLAG_LOOP);
}

void pwm_control_init(void){
    nrfx_pwm_config_t config = 
    {
        .output_pins = 
        {
            LED_1 | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock = NRF_PWM_CLK_1MHz,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = CONTROL_LED_TOP_VALUE,
        .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = NRF_PWM_STEP_AUTO
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm_control, &config, pwm_control_evt_handler));

    m_control_seq_values.channel_0 = 0;
    m_control_seq_values.channel_1 = 0;
    m_control_seq_values.channel_2 = 0;
    m_control_seq_values.channel_3 = 0;

    nrfx_pwm_simple_playback(&m_pwm_control, &m_control_seq, NUMBER_OF_PLAYBACKS, NRFX_PWM_FLAG_LOOP);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    logs_init();

    /* Configure board. */
    gpio_module_leds_init();

    gpio_module_buttons_init();

    timer_init();

    gpiote_button_init();

    pwm_rgb_init();
    pwm_control_init();

    /* Toggle LEDs. */
    while (true)
    {
        __WFE();

        NRF_LOG_PROCESS();
        LOG_BACKEND_USB_PROCESS();
    }
}
