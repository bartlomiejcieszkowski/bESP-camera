

#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#if CONFIG_NEWLIB_STDIN_LINE_ENDING_CR
#define RX_LINE_ENDING ESP_LINE_ENDINGS_CR
#elif CONFIG_NEWLIB_STDIN_LINE_ENDING_LF
#define RX_LINE_ENDING ESP_LINE_ENDINGS_LF
#else
#define RX_LINE_ENDING ESP_LINE_ENDINGS_CRLF
#endif

#if CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR
#define TX_LINE_ENDING ESP_LINE_ENDINGS_CR
#elif CONFIG_NEWLIB_STDOUT_LINE_ENDING_LF
#define TX_LINE_ENDING ESP_LINE_ENDINGS_LF
#else
#define TX_LINE_ENDING ESP_LINE_ENDINGS_CRLF
#endif

static const char* simple_console_prompt = NULL;

void initialize_simple_console(void)
{
	setvbuf(stdin, NULL, _IONBF, 0);

	esp_vfs_dev_uart_set_rx_line_endings(RX_LINE_ENDING);
	esp_vfs_dev_uart_set_tx_line_endings(TX_LINE_ENDING);

	const uart_config_t uart_config = {
		.baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.use_ref_tick = true
	};
	
	ESP_ERROR_CHECK(uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

	// send buf size, rcv buf size, event queue size, event queue, interrupt flags
	ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));

	// interrupt driven - blocking read and write
	esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

	esp_console_config_t console_config = {
		.max_cmdline_args = 8,
		.max_cmdline_lenght = 256,
#if CONFIG_LOG_COLORS
		.hint_color = atoi(LOG_COLOR_CYAN)
#endif
	};
	ESP_ERROR_CHECK(esp_console_init(&console_config));

	// setup linenoise
	linenoiseSetMultiLine(1);
	linenoiseHistorySetMaxLen(100);

	linenoiseSetCompletionCallback(&esp_console_get_completion);
	linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);
	

}

int prepare_simple_console(const char* prompt, const char* prompt_color)
{
	int probe_status = linenoiseProbe();
	if (probe_status) {
		printf("\n"
			"Your terminal application does not support escape sequences.\n"
			"Line editing and history features are disabled.\n"
			"On Windows, try using Putty instead.\n");
		linenoiseSetDumbMode(1);
	}
#if CONFIG_LOG_COLORS
	else if (prompt_color) {
		prompt = prompt_color;
	}
#endif //CONFIG_LOG_COLORS
	simple_console_prompt = prompt;
}

inline void simple_console_loop_body(void)
{
	char *line = linenoise(simple_console_prompt);
	if (line == NULL) {
		continue;
	}

	linenoiseHistoryAdd(line);

	int ret;
	esp_err_t err = esp_console_run(line, &ret);
	if (err == ESP_ERR_NOT_FOUND) {
		printf("Unrecognized command\n");
	} else if (err == ESP_ERR_INVALID_ARG) {
	} else if (err == ESP_OK && ret) {
		printf("Command returned non-zero error code: 0x%x\n");
	} else if (err != ESP_OK) {
#if CONFIG_ESP_ERR_TO_NAME_LOOKUP
		printf("Internal error: 0x%x (%s)\n", err, esp_err_to_name(err));
#else
		printf("Internal error: 0x%x\n", err);
#endif
	}
	linenoiseFree(line);
}

/**
 * This function can be used as a param to xTaskCreate
 */
void simple_console_task(void *param)
{
	(void)param;
	prepare_simple_console();
	do {
		simple_console_loop_body();
	} while(1);
}
