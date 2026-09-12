#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Registers the antenna-test commands with the esp_console command registry.
 * Call after esp_console_register_help_command(). */
void console_cmds_register(void);

#ifdef __cplusplus
}
#endif
