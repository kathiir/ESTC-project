#include "usb_cli_commands.h"


static int command_set_rgb_color(char * args, char *response, size_t resp_max_len)
{
    int num_of_args = 3;
    uint8_t arg[3];
    for (int i = 0; i < num_of_args; i++)
    {
        char * token = get_token(args, &args);
        if (token == NULL)
        {
            return snprintf(response, resp_max_len, "Invalid parameter count, see help\r\n");
        }
        int ret = sscanf(token, "%hhu", arg + i);
        if (ret < 0)
        {
            return snprintf(response, resp_max_len, "Invalid parameters, see help\r\n");
        }
    }

    pwm_set_rgb_color(arg[0], arg[1], arg[2]);

    save_state();

    return snprintf(response, resp_max_len, "Color set to rgb %d %d %d \r\n", arg[0], arg[1], arg[2]);
}

static int command_set_hsv_color(char * args, char *response, size_t resp_max_len)
{
    int num_of_args = 3;
    uint8_t arg[3];
    for (int i = 0; i < num_of_args; i++)
    {
        char * token = get_token(args, &args);
        if (token == NULL)
        {
            return snprintf(response, resp_max_len, "Invalid parameter count, see help \r\n");
        }
        int ret = sscanf(token, "%hhu", arg + i);
        if (ret < 0)
        {
            return snprintf(response, resp_max_len, "Invalid parameters, see help \r\n");
        }
    }

    pwm_set_hsv_color(arg[0], arg[1], arg[2]);

    save_state();

    return snprintf(response, resp_max_len, "Color set to hsv %d %d %d \r\n", arg[0], arg[1], arg[2]);
}

static int command_help(char * args, char *response, size_t resp_max_len)
{
    int len = snprintf(response, resp_max_len, "Help: \r\n");
    len = len + snprintf(response + len, resp_max_len - len, "rgb [0-255] [0-255] [0-255] \r\n");
    len = len + snprintf(response + len, resp_max_len - len, "hsv [0-255] [0-255] [0-255] \r\n");

    return len;
}


void usb_cli_init_commands(void)
{
    usb_cli_add_command("rgb", command_set_rgb_color);
    usb_cli_add_command("hsv", command_set_hsv_color);
    usb_cli_add_command("help", command_help);
}