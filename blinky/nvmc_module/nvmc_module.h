#ifndef NVMC_MODULE_H
#define NVMC_MODULE_H

#include "nrfx_nvmc.h"
#include "color_module.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "gpio_pwm.h"

#define MASK (0xFFFFFFFF)

void nvmc_write(uint32_t source);

uint32_t nvmc_read();

void nvmc_init(pwm_hsv_t * hsv);

void save_state(void);
void restore_state(void);

#endif /* NVMC_MODULE_H */