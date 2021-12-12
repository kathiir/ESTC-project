#ifndef USB_MODULE_H
#define USB_MODULE_H

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"

#include "usb_cli.h"

#define USB_CLI_CMD_MAX_LEN 256
#define USB_TX_MAX_LEN      256

void usb_init(void);

#endif /* USB_MODULE_H */