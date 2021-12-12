#ifndef USB_CLI_H
#define USB_CLI_H

#include "stdint.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>

#define USB_CLI_MAX_NUMBER_OF_COMMANDS 10

typedef int (* usb_cli_command_handler_t)(char * args, char *response, size_t resp_max_len);

char *get_token(char *rx_ptr, char ** next_token);

int usb_cli_process_command(char * rx_buffer, size_t length, char * response, size_t resp_max_len);

void usb_cli_add_command(char * command, usb_cli_command_handler_t handler);


#endif /* USB_CLI_H */