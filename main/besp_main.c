#include <stdio.h>
#include <string.h>
#include "micro_rtsp_server.h"
#include "simple_console.h"

static const char* TAG = "main";

static const char* prompt = "besp> ";
static const char* prompt_color = LOG_COLOR_I "besp> " LOG_RESET_COLOR;

void app_main()
{
    initialize_camera();
    initialize_simple_console();

    register_micro_rtsp();

    /* console task has a main loop */
    simple_console_task(NULL);
}
