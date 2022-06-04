#ifndef GPIO_PWM_H
#define GPIO_PWM_H

#include "nrfx_systick.h"
#include "nrfx_pwm.h"
#include "nrfx_gpiote.h"
#include "app_util_platform.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "gpio_module.h"
#include "color_module.h"

#define NUMBER_OF_PLAYBACKS 1

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

void pwm_set_rgb_color(color_rgb_t * color);

void pwm_init(color_rgb_t * color);

#endif /* GPIO_PWM_H */