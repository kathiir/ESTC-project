#include "gpio_pwm.h"

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