# Crack the Safe -- the ESP32-U4WDH co-processor's firmware: Bluetooth
# headphones. A separate ESP-IDF project, target esp32; the game itself is the
# ESP32-S3 project one folder up.
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(crack_the_safe_bt)
