# TrailCurrent Power Dashboard — Waveshare ESP32-P4-WIFI6-Touch-LCD-7B.
#
# 7" 1024x600 IPS + capacitive touch. The GUI is authored in EEZ Studio
# (GUI/TrailCurrentPowerDash.eez-project) and exported to main/UI/.
#
# First build on a fresh clone:
#   idf.py set-target esp32p4      # required: the BSP is esp32p4-only
#   idf.py build

cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(powerdash)
