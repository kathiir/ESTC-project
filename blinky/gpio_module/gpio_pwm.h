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


#define CONTROL_LED_TOP_VALUE       10000
#define CONTROL_LED_SLOW_STEP_VALUE 100
#define CONTROL_LED_FAST_STEP_VALUE 400

#define HUE_TOP_VALUE               25599
#define SATURATION_TOP_VALUE        25599
#define BRIGHTNESS_TOP_VALUE        25599

#define NUMBER_OF_PLAYBACKS 1

#ifndef GPIO_PWM_DUTY_CYCLE_PERIOD_US
#define GPIO_PWM_DUTY_CYCLE_PERIOD_US 1000
#endif

#define NUMBER_OF_MODES 4
#define MODE_NO_INPUT   0
#define MODE_HUE_MOD    1
#define MODE_SAT_MOD    2
#define MODE_BRT_MOD    3

typedef enum blinky_mode_e
{
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
} pwm_hsv_t;

/**
 * @brief Blink with known duty cycle
 *
 * @param led_idx index of LED to blink (valid value)
 * @param duty_cycle duty cycle in percent value [0, 100]
 */
void pwm_led_duty_cycle(uint8_t led_idx, uint8_t duty_cycle);

void pwm_set_hsv_color(uint8_t hue, uint8_t saturation, uint8_t value);
void pwm_set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

void pwm_init(pwm_hsv_t * hsv, blinky_mode_t * mode, bool * change_value);

#endif /* GPIO_PWM_H */