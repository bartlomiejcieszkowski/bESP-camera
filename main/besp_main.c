#include <stdio.h>
#include <string.h>
#include "micro_rtsp_server.h"

static const char* TAG = "main";

void app_main()
{
    initialize_camera();

    register_micro_rtsp();

    /* Main loop */
    while(true) {
    }
}
