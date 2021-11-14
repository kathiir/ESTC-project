#ifndef GPIO_PWM_H
#define GPIO_PWM_H

#include "gpio_module.h"
#include "nrfx_systick.h"

#ifndef GPIO_PWM_DUTY_CYCLE_PERIOD_US
#define GPIO_PWM_DUTY_CYCLE_PERIOD_US 1000
#endif

/**
 * @brief Blink with known duty cycle
 *
 * @param led_idx index of LED to blink (valid value)
 * @param duty_cycle duty cycle in percent value [0, 100]
 */
void pwm_led_duty_cycle(uint8_t led_idx, uint8_t duty_cycle);





#endif /* GPIO_PWM_H */