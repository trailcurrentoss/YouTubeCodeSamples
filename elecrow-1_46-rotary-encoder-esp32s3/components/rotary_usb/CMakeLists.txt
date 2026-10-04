idf_component_register(
    SRCS "src/rotary_usb.c"
    INCLUDE_DIRS "include"
    REQUIRES espressif__esp_tinyusb
    PRIV_REQUIRES esp_app_format esp_system log
)
