/*
 * clock — time of day for the idle-page analog clock.
 *
 * The pad has no battery-backed RTC and no network, so it cannot know the
 * time by itself. The companion sends `time HH:MM:SS` when it connects and
 * once a minute; between updates the pad counts with esp_timer. Until the
 * first update arrives the time is unknown, and the clock hides its hands
 * rather than show a wrong time.
 *
 * Call from the LVGL task, or with the display lock held.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

void clock_set(int hour, int minute, int second);
bool clock_valid(void);

/* Hand positions in scale units, 0..3600 per revolution (the idle-page
 * Scale's range): hour moves 300 per hour and 5 per minute, minute 60 per
 * minute and 1 per second, second 60 per second. */
int32_t clock_hour_hand(void);
int32_t clock_minute_hand(void);
int32_t clock_second_hand(void);
