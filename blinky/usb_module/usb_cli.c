#include "usb_cli.h"



static char * m_commands[USB_CLI_MAX_NUMBER_OF_COMMANDS] = { NULL };
static usb_cli_command_handler_t m_command_handlers[USB_CLI_MAX_NUMBER_OF_COMMANDS];

/**
 * Sliiightly modified
 * Nice and simple strtok implementation from kind person on stack overflow without usage of static vars
 * also change space after token for termination symbols
 */
char * get_token(char *rx_ptr, char ** next_token)
{
    char *start = rx_ptr;
    char *end = 0;

    /* Scan leading delimiters.  */
    while (isspace(*start))
    {
        start++;
    }

    if (*start == '\0')
    {
        return NULL;
    }

    /* Find the end of the token.  */
    end = start;
    while ((*end != '\0') && !isspace(*end))
    {
        end++;
    }
    if (*end == '\0')
    {
        *next_token = end;
        return start;
    }

    /* Terminate the token and make *SAVE_PTR point past it.  */
    *end = '\0';
    *next_token = end + 1;

    return start;
}

static int usb_cli_handle_command(char * command, char * args, char *response, size_t resp_max_len)
{

    for (int i = 0; i < USB_CLI_MAX_NUMBER_OF_COMMANDS; i++)
    {
        if (m_commands[i] == NULL)
        {
            return snprintf(response, resp_max_len, "Unrecognized command \r\n");
        }

        if (0 == strcmp(m_commands[i], command))
        {
            return m_command_handlers[i](args, response, resp_max_len);
        }
    }

    return snprintf(response, resp_max_len, "Unrecognized command \r\n");
}

int usb_cli_process_command(char *rx_buffer, size_t length, char *response, size_t resp_max_len)
{
    char * token;
    char * next;

    token = get_token(rx_buffer, &next);

    if (token == NULL)
    {
        return 0;
    }

    return usb_cli_handle_command(token, next, response, resp_max_len);
}

void usb_cli_add_command(char * command, usb_cli_command_handler_t handler)
{
    int i = 0;

    while (m_commands[i] != NULL || i == USB_CLI_MAX_NUMBER_OF_COMMANDS)
    {
        i++;
    }

    m_commands[i] = command;
    m_command_handlers[i] = handler;
}