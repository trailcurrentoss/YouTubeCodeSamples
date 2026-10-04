# Rotary HID — Elecrow CrowPanel 1.46" Rotary (360x360 JD9855, touch, ESP32-S3).
#
# The GUI is authored in EEZ Studio (GUI/RotaryHid.eez-project) and
# exported to main/ui/, which can be deleted and re-exported at any time. Board support lives in components/crowpanel_board.
#
#   idf.py set-target esp32s3     # first build on a fresh clone
#   idf.py build
cmake_minimum_required(VERSION 3.16)
set(PROJECT_VER "0.0.1")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(rotary_hid)
