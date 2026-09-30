# Rotary HID — Makerfabs MaTouch ESP32-S3 Rotary IPS 2.1" (480x480, touch).
#
# The GUI is authored in EEZ Studio (GUI/RotaryHid.eez-project) and
# exported to main/ui/, which can be deleted and re-exported at any time. Board support lives in components/matouch_board.
#
#   idf.py set-target esp32s3     # first build on a fresh clone
#   idf.py build
cmake_minimum_required(VERSION 3.16)
set(PROJECT_VER "0.0.1")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(rotary_hid)
