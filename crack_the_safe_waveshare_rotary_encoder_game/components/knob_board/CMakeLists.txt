idf_component_register(
    SRCS
        "src/knob_board.c"
        "src/board_display.c"
        "src/board_touch.c"
        "src/board_knob.c"
        "src/board_haptic.c"
        "src/board_audio.c"
        "src/board_coproc.c"
        "src/board_battery.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES
        esp_driver_i2c
        esp_lcd
        lvgl__lvgl
    PRIV_REQUIRES
        esp_adc
        esp_driver_gpio
        esp_driver_i2s
        esp_driver_ledc
        esp_driver_spi
        esp_driver_uart
        esp_timer
        espressif__esp_lvgl_port
        espressif__esp_lcd_st77916
        espressif__esp_lcd_touch_cst816s
)
