idf_component_register(
    SRCS "src/mesh_node.c" "src/mesh_phone.c" "src/mesh_ble.c" "src/mesh_pb.c" "src/sx1262.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES esp_driver_spi esp_driver_gpio esp_timer nvs_flash
    PRIV_REQUIRES bt mbedtls esp_hw_support espressif__esp_hosted
)
