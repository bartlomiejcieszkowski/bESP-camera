#ifndef SIMPLE_CONSOLE_H
#define SIMPLE_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

void initialize_console(void);
int prepare_simple_console(const char* prompt, const char* prompt_color);
void simple_console_loop_body(void);
void simple_console_task(void *param);

#ifdef __cplusplus
}
#endif
#endif //SIMPLE_CONSOLE_H
