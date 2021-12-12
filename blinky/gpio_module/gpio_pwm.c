#include "gpio_pwm.h"


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

static uint16_t volatile control_led_value = 0;

static pwm_hsv_t * m_hsv_tmp_values;
static blinky_mode_t * m_mode;
static bool * m_change_value;

static void pwm_control_evt_handler(nrfx_pwm_evt_type_t event_type)
{
    if (event_type == NRFX_PWM_EVT_FINISHED)
    {
        nrf_pwm_values_individual_t * p_channels = &m_control_seq_values;

        if (*m_mode == BLINKY_MODE_NO_INPUT)
        {
            control_led_value = 0;
        }
        else if (*m_mode == BLINKY_MODE_BRT_MOD)
        {
            control_led_value = CONTROL_LED_TOP_VALUE;
        }
        else
        {
            uint16_t step;

            if (*m_mode == BLINKY_MODE_HUE_MOD)
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
        if (*m_change_value && (*m_mode != BLINKY_MODE_NO_INPUT))
        {
            nrf_pwm_values_individual_t * p_channels = &m_rgb_seq_values;

            color_hsv_t hsv_values;
            color_rgb_t rgb_led_values;

            if (*m_mode == BLINKY_MODE_HUE_MOD)
            {
                m_hsv_tmp_values->hue = (m_hsv_tmp_values->hue + 1) % HUE_TOP_VALUE;
                NRF_LOG_INFO("Hue value: %d", m_hsv_tmp_values->hue);
            }
            else if (*m_mode == BLINKY_MODE_SAT_MOD)
            {
                m_hsv_tmp_values->saturation = (m_hsv_tmp_values->saturation + 1) % SATURATION_TOP_VALUE;
                NRF_LOG_INFO("Saturation value: %d", m_hsv_tmp_values->saturation);
            }
            else if (*m_mode == BLINKY_MODE_BRT_MOD)
            {
                m_hsv_tmp_values->value = (m_hsv_tmp_values->value + 1) % BRIGHTNESS_TOP_VALUE;
                NRF_LOG_INFO("Brightness value: %d", m_hsv_tmp_values->value);
            }

            hsv_values.hue = m_hsv_tmp_values->hue / 100;
            hsv_values.saturation = m_hsv_tmp_values->saturation / 100;
            hsv_values.value = m_hsv_tmp_values->value / 100;

            color_hsv_to_rgb(&hsv_values, &rgb_led_values);

            p_channels->channel_0 = rgb_led_values.red;
            p_channels->channel_1 = rgb_led_values.green;
            p_channels->channel_2 = rgb_led_values.blue;
        }

    }
}

void pwm_led_duty_cycle(uint8_t led_idx, uint8_t duty_cycle)
{
    ASSERT(led_idx < LEDS_NUMBER);
    ASSERT(duty_cycle <= 100);

    gpio_module_led_on(led_idx);
    /*
    * We can replace it with test function and cycle through it but it will be same in the end.
    * See nrfx_systick_delay_us() and nrfx_systick_test().
    */
    nrfx_systick_delay_us(GPIO_PWM_DUTY_CYCLE_PERIOD_US * duty_cycle / 100);
    gpio_module_led_off(led_idx);
    nrfx_systick_delay_us(GPIO_PWM_DUTY_CYCLE_PERIOD_US * (100 - duty_cycle) / 100);
}

static void pwm_rgb_init(pwm_hsv_t * hsv_tmp)
{
    m_hsv_tmp_values = hsv_tmp;

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
        m_hsv_tmp_values->hue / 100,
        m_hsv_tmp_values->saturation / 100,
        m_hsv_tmp_values->value / 100
    };

    color_rgb_t rgb;

    color_hsv_to_rgb(&hsv, &rgb);

    m_rgb_seq_values.channel_0 = rgb.red;
    m_rgb_seq_values.channel_1 = rgb.green;
    m_rgb_seq_values.channel_2 = rgb.blue;
    m_rgb_seq_values.channel_3 = 0;

    nrfx_pwm_simple_playback(&m_pwm_rgb, &m_rgb_seq, NUMBER_OF_PLAYBACKS, NRFX_PWM_FLAG_LOOP);
}

static void pwm_control_init(void)
{
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

void pwm_init(pwm_hsv_t * hsv, blinky_mode_t * mode, bool * change_value)
{
    m_mode = mode;
    m_change_value = change_value;
    pwm_rgb_init(hsv);
    pwm_control_init();
}