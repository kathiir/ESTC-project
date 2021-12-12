#ifndef GPIO_BUTTON_H
#define GPIO_BUTTON_H

#include "nrfx_gpiote.h"

#include "gpio_module.h"
#include "color_module.h"
#include "gpio_pwm.h"

#include "nvmc_module.h"

#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"


void timer_init(void);
void gpiote_button_init(blinky_mode_t * mode, bool * change_value);


#endif /* GPIO_BUTTON_H */