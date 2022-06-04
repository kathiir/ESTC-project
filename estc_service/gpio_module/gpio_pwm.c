#include "gpio_pwm.h"


static nrfx_pwm_t m_pwm_rgb     = NRFX_PWM_INSTANCE(0);

static nrf_pwm_values_individual_t m_rgb_seq_values;

static nrf_pwm_sequence_t const    m_rgb_seq =
{
    .values.p_individual = &m_rgb_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_rgb_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static void pwm_rgb_evt_handler(nrfx_pwm_evt_type_t event_type)
{
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

void pwm_set_rgb_color(color_rgb_t * color)
{
    m_rgb_seq_values.channel_0 = color->red;
    m_rgb_seq_values.channel_1 = color->green;
    m_rgb_seq_values.channel_2 = color->blue;
    m_rgb_seq_values.channel_3 = 0;
}

void pwm_init(color_rgb_t * color)
{
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

    m_rgb_seq_values.channel_0 = color->red;
    m_rgb_seq_values.channel_1 = color->green;
    m_rgb_seq_values.channel_2 = color->blue;
    m_rgb_seq_values.channel_3 = 0;

    nrfx_pwm_simple_playback(&m_pwm_rgb, &m_rgb_seq, NUMBER_OF_PLAYBACKS, NRFX_PWM_FLAG_LOOP);
}