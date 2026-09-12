idf_component_register(
    SRCS
        "main.c"
        "wifi_link.c"
        "rf_bench.c"
        "results.c"
        "console_cmds.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_wifi
        esp_netif
        esp_event
        esp_timer
        nvs_flash
        console
        lwip
)
