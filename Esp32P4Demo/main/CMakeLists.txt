idf_component_register(
    SRCS
        "main.c"
        "sd_card.c"
        "sd_test.c"
        "wifi_manager.c"
        "web_server.c"
        "dns_portal.c"
        "audio_recorder.c"
        "record_button.c"
        "peregrine_voice.c"
        "sd_config.c"
        "ai_camera.cpp"
        "webcam.c"
    INCLUDE_DIRS
        "."
    REQUIRES
        esp_driver_sdmmc
        sdmmc
        fatfs
        esp_psram
        spi_flash
        esp_timer
        esp_wifi
        esp_netif
        esp_event
        esp_http_server
        esp_http_client      # POSTs push-to-talk audio to Peregrine
        esp_app_format
        nvs_flash
        json
        lwip
        esp_driver_i2s
        esp_driver_i2c       # shared i2c_master bus: codec + camera SCCB
        esp_driver_jpeg      # hardware JPEG encoder for the MJPEG stream
        esp_ringbuf          # encoded-frame queue between webcam task and streamer
        es8311               # vendored, ported to i2c_master (components/es8311)
        espressif__esp_video
        espressif__esp-dl
        espressif__coco_detect
        espressif__esp_h264  # hardware H.264 encoder for the webcam stream
    # Web assets are linked into the binary rather than kept on a filesystem,
    # so the UI always matches the firmware serving it and survives a card
    # being reformatted or removed. Referenced in web_server.c as
    # _binary_<name>_start / _end.
    EMBED_FILES
        "web/setup.html"
        "web/dashboard.html"
        "web/style.css"
        "web/setup.js"
        "web/dashboard.js"
)
