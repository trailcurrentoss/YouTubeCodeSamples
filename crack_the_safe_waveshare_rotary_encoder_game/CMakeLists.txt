# Crack the Safe -- a safe-cracking game for the Waveshare
# ESP32-S3-Knob-Touch-LCD-1.8 (360x360 round touch LCD, detent knob, DRV2605
# haptics).
#
# The GUI is authored in EEZ Studio (GUI/CrackTheSafe.eez-project) and
# exported to main/ui/, which can be deleted and re-exported at any time.
# Board support lives in components/knob_board.
#
#   idf.py set-target esp32s3     # first build on a fresh clone
#   idf.py build
cmake_minimum_required(VERSION 3.16)
set(PROJECT_VER "0.1.0")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(crack_the_safe)
